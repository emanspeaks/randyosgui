$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot '..')
Set-Location $repoRoot

$manifestPath = Join-Path $repoRoot 'third_party/versions.json'
if (-not (Test-Path $manifestPath)) {
    throw "Missing dependency manifest: $manifestPath"
}
$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json

function New-Directory([string]$Path) {
    New-Item -ItemType Directory -Path $Path -Force | Out-Null
}

function Assert-Hash([string]$Path, [string]$ExpectedSha256) {
    $actual = (Get-FileHash $Path -Algorithm SHA256).Hash.ToUpperInvariant()
    $expected = $ExpectedSha256.ToUpperInvariant()
    if ($actual -ne $expected) {
        throw "Hash mismatch for $Path. Expected $expected, got $actual"
    }
}

function Save-CheckedFile([string]$Url, [string]$OutFile, [string]$Sha256) {
    if (Test-Path $OutFile) {
        $existing = (Get-FileHash $OutFile -Algorithm SHA256).Hash.ToUpperInvariant()
        if ($existing -eq $Sha256.ToUpperInvariant()) {
            return
        }
        Remove-Item -Force $OutFile
    }

    Invoke-WebRequest -Uri $Url -OutFile $OutFile
    Assert-Hash $OutFile $Sha256
}

function Sync-GitCommit([string]$RepoUrl, [string]$Commit, [string]$DstDir) {
    if (-not (Test-Path $DstDir)) {
        git clone --depth 1 $RepoUrl $DstDir | Out-Null
    }

    Push-Location $DstDir
    try {
        git fetch --depth 1 origin $Commit | Out-Null
        git checkout --force $Commit | Out-Null
        $head = (git rev-parse HEAD).Trim()
        if ($head -ne $Commit) {
            throw "Unable to pin $DstDir to commit $Commit"
        }
    }
    finally {
        Pop-Location
    }
}

$downloads = Join-Path $repoRoot 'third_party/_downloads'
New-Directory $downloads

# Local output layout
New-Directory 'third_party/glfw/include'
New-Directory 'third_party/glfw/lib/windows'
New-Directory 'third_party/vulkan/include'
New-Directory 'third_party/vulkan/lib/windows'
New-Directory 'third_party/freetype/include'
New-Directory 'third_party/freetype/lib/windows'

# GLFW binary package
$glfwZip = Join-Path $downloads 'glfw-3.4.bin.WIN64.zip'
Save-CheckedFile $manifest.glfw.url $glfwZip $manifest.glfw.sha256
$glfwExtract = Join-Path $downloads 'glfw_bin'
if (Test-Path $glfwExtract) { Remove-Item -Recurse -Force $glfwExtract }
Expand-Archive -Path $glfwZip -DestinationPath $glfwExtract -Force
Copy-Item -Recurse -Force (Join-Path $glfwExtract 'glfw-3.4.bin.WIN64/include/GLFW') 'third_party/glfw/include/'
Copy-Item -Force (Join-Path $glfwExtract 'glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib') 'third_party/glfw/lib/windows/glfw3.lib'
Copy-Item -Force (Join-Path $glfwExtract 'glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib') 'third_party/glfw/lib/windows/glfw3dll.lib'
Copy-Item -Force (Join-Path $glfwExtract 'glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll') 'third_party/glfw/lib/windows/glfw3.dll'

# FreeType prebuilt windows package pinned by commit archive hash
$ftZip = Join-Path $downloads 'freetype-windows-binaries.zip'
Save-CheckedFile $manifest.freetype_windows_binaries.url $ftZip $manifest.freetype_windows_binaries.sha256
$ftExtract = Join-Path $downloads 'freetype_bin'
if (Test-Path $ftExtract) { Remove-Item -Recurse -Force $ftExtract }
Expand-Archive -Path $ftZip -DestinationPath $ftExtract -Force
$ftRoot = Get-ChildItem -Path $ftExtract -Directory | Select-Object -First 1
if (-not $ftRoot) { throw 'Failed to extract FreeType package' }
Copy-Item -Recurse -Force (Join-Path $ftRoot.FullName 'include/*') 'third_party/freetype/include/'
$ftLib = Get-ChildItem -Path $ftRoot.FullName -Recurse -Filter freetype.lib |
    Where-Object { $_.FullName -match 'x64|win64|vs' } |
    Select-Object -First 1
$ftDll = Get-ChildItem -Path $ftRoot.FullName -Recurse -Filter freetype.dll |
    Where-Object { $_.FullName -match 'x64|win64|vs' } |
    Select-Object -First 1
if (-not $ftLib) { $ftLib = Get-ChildItem -Path $ftRoot.FullName -Recurse -Filter freetype.lib | Select-Object -First 1 }
if (-not $ftDll) { $ftDll = Get-ChildItem -Path $ftRoot.FullName -Recurse -Filter freetype.dll | Select-Object -First 1 }
if (-not $ftLib) { throw 'Could not find freetype.lib in extracted package' }
if (-not $ftDll) { throw 'Could not find freetype.dll in extracted package' }
Copy-Item -Force $ftLib.FullName 'third_party/freetype/lib/windows/freetype.lib'
Copy-Item -Force $ftDll.FullName 'third_party/freetype/lib/windows/freetype.dll'

# Vulkan headers + vk_video and generated import library
$vkHeadersDir = Join-Path $downloads 'Vulkan-Headers'
Sync-GitCommit $manifest.vulkan_headers.repo $manifest.vulkan_headers.commit $vkHeadersDir
Copy-Item -Recurse -Force (Join-Path $vkHeadersDir 'include/vulkan') 'third_party/vulkan/include/'
Copy-Item -Recurse -Force (Join-Path $vkHeadersDir 'include/vk_video') 'third_party/vulkan/include/'

$vkLoaderDir = Join-Path $downloads 'Vulkan-Loader'
Sync-GitCommit $manifest.vulkan_loader.repo $manifest.vulkan_loader.commit $vkLoaderDir
zig dlltool -m i386:x86-64 -d (Join-Path $vkLoaderDir 'loader/vulkan-1.def') -D vulkan-1.dll -l 'third_party/vulkan/lib/windows/vulkan-1.lib' | Out-Null

# Arimo Nerd Font (sans)
$arimoTar = Join-Path $downloads 'Arimo.tar.xz'
Save-CheckedFile $manifest.arimo_nerd_font.url $arimoTar $manifest.arimo_nerd_font.sha256
New-Directory 'third_party/fonts/arimo'
$arimoTarget = 'third_party/fonts/arimo/ArimoNerdFont-Regular.ttf'
if (-not (Test-Path $arimoTarget)) {
    tar -xf $arimoTar -C 'third_party/fonts/arimo' ArimoNerdFont-Regular.ttf
}

# AtkynsonMono Nerd Font (mono)
$atkinsonTar = Join-Path $downloads 'AtkinsonHyperlegibleMono.tar.xz'
Save-CheckedFile $manifest.atkinson_hyperlegible_mono_nerd_font.url $atkinsonTar $manifest.atkinson_hyperlegible_mono_nerd_font.sha256
New-Directory 'third_party/fonts/atkinsonhyperlegiblemono'
$atkinsonTarget = 'third_party/fonts/atkinsonhyperlegiblemono/AtkynsonMonoNerdFontMono-Regular.otf'
if (-not (Test-Path $atkinsonTarget)) {
    tar -xf $atkinsonTar -C 'third_party/fonts/atkinsonhyperlegiblemono' AtkynsonMonoNerdFontMono-Regular.otf
}

Write-Host 'Dependency fetch complete.'
Write-Host 'Pinned artifacts populated under third_party/ for Windows.'
