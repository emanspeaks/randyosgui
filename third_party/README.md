# Third-party Dependencies (Repo-local)

This repository is configured to look for dependency headers and libraries in this folder first.

The repository uses a fetch-only policy for third-party code and binaries. Dependency trees under `third_party/glfw`, `third_party/freetype`, and `third_party/vulkan` are local working copies created by the fetch script and are not intended to be committed.

## Reproducible setup

- Pinned dependency metadata lives in third_party/versions.json.
- Run scripts/fetch_deps.ps1 from repo root to fetch and stage dependencies.
- Build defaults to strict repo-local dependency checks.
- Headers and binary artifacts under third_party/* are intended to stay local and are gitignored.

If you intentionally want to rely on globally installed SDKs/libraries, build with:

- zig build -Dallow-system-deps=true

## Expected layout

- third_party/glfw/include
- third_party/glfw/lib
- third_party/glfw/lib/windows
- third_party/glfw/lib/linux
- third_party/glfw/lib/macos
- third_party/vulkan/include
- third_party/vulkan/lib
- third_party/vulkan/lib/windows
- third_party/vulkan/lib/linux
- third_party/vulkan/lib/macos
- third_party/freetype/include
- third_party/freetype/lib
- third_party/freetype/lib/windows
- third_party/freetype/lib/linux
- third_party/freetype/lib/macos

You only need folders for your current target OS.

## Windows quick start

Run:

- powershell -ExecutionPolicy Bypass -File scripts/fetch_deps.ps1

That script will:

- download GLFW Windows binaries
- download pinned FreeType Windows binaries
- clone pinned Vulkan-Headers and Vulkan-Loader revisions
- populate local third_party include trees
- place local .dll/.lib files in third_party/*/lib/windows
- regenerate third_party/vulkan/lib/windows/vulkan-1.lib via zig dlltool

Only third_party/README.md and third_party/versions.json are meant to stay in Git.

The build links library names using normal linker names, while include and library search paths are extended to these repo-local directories.

## Runtime note

Vulkan applications still require machine-level Vulkan runtime support from the GPU driver.
