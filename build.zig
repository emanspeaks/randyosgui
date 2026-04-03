const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const allow_system_deps = b.option(bool, "allow-system-deps", "Allow relying on globally installed SDK/libs") orelse false;

    validateDependencyLayout(b, target, allow_system_deps);

    const lib_sources: []const []const u8 = &.{
        "src/randyosgui.c",
        "src/platform/platform.c",
        "src/renderer/renderer.c",
    };

    const c_flags: []const []const u8 = &.{
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
    };

    // --- Static library ---
    // System libs are NOT linked here; they're added by each consumer so that
    // lld doesn't try to bundle .so stubs into the .a archive.
    const lib_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    lib_mod.addCSourceFiles(.{ .files = lib_sources, .flags = c_flags });
    lib_mod.addIncludePath(b.path("include"));
    lib_mod.addIncludePath(b.path("src"));
    addThirdPartyIncludePaths(lib_mod, b, target);

    const lib = b.addLibrary(.{
        .name = "randyosgui",
        .root_module = lib_mod,
        .linkage = .static,
    });
    b.installArtifact(lib);

    // --- Shared library ---
    // Shared lib IS fully linked, so platform libs go here.
    const shared_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    shared_mod.addCSourceFiles(.{ .files = lib_sources, .flags = c_flags });
    shared_mod.addIncludePath(b.path("include"));
    shared_mod.addIncludePath(b.path("src"));
    addThirdPartyIncludePaths(shared_mod, b, target);
    addPlatformLibs(shared_mod, b, target);

    const shared = b.addLibrary(.{
        .name = "randyosgui",
        .root_module = shared_mod,
        .linkage = .dynamic,
    });
    b.installArtifact(shared);

    installRuntimeDlls(b, target);

    // Install public header
    b.installFile("include/randyosgui.h", "include/randyosgui.h");

    // --- Examples ---
    buildExample(b, "hello", "examples/hello/main.c", target, optimize, lib);

    // --- Tests ---
    const test_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    test_mod.addCSourceFiles(.{
        .files = &.{"tests/test_randyosgui.c"},
        .flags = c_flags,
    });
    test_mod.addIncludePath(b.path("include"));
    addThirdPartyIncludePaths(test_mod, b, target);
    test_mod.linkLibrary(lib);
    addPlatformLibs(test_mod, b, target);

    const test_exe = b.addExecutable(.{
        .name = "test_randyosgui",
        .root_module = test_mod,
    });

    const run_tests = b.addRunArtifact(test_exe);
    if (target.result.os.tag == .windows) {
        run_tests.addPathDir("third_party/glfw/lib/windows");
        run_tests.addPathDir("third_party/freetype/lib/windows");
    }
    // On headless systems set DISPLAY to a Xvfb instance before running:
    //   Xvfb :99 -screen 0 1024x768x24 & DISPLAY=:99 zig build test
    const test_step = b.step("test", "Run unit tests (needs DISPLAY on headless systems)");
    test_step.dependOn(&run_tests.step);
}

fn installIfPresent(b: *std.Build, src_rel: []const u8, dst_rel: []const u8) void {
    if (pathExists(src_rel)) {
        b.installFile(src_rel, dst_rel);
    }
}

fn installRuntimeDlls(b: *std.Build, target: std.Build.ResolvedTarget) void {
    if (target.result.os.tag != .windows) return;

    installIfPresent(b, "third_party/glfw/lib/windows/glfw3.dll", "bin/glfw3.dll");
    installIfPresent(b, "third_party/freetype/lib/windows/freetype.dll", "bin/freetype.dll");
}

fn ensurePath(b: *std.Build, rel_path: []const u8, what: []const u8) void {
    _ = b;
    if (!pathExists(rel_path)) {
        std.debug.panic(
            "missing {s}: {s}\nRun scripts/fetch_deps.ps1 to populate repo-local third_party dependencies.",
            .{ what, rel_path },
        );
    }
}

fn validateDependencyLayout(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    allow_system_deps: bool,
) void {
    if (allow_system_deps) return;

    switch (target.result.os.tag) {
        .windows => {
            ensurePath(b, "third_party/glfw/include/GLFW/glfw3.h", "GLFW header");
            ensurePath(b, "third_party/vulkan/include/vulkan/vulkan.h", "Vulkan header");
            ensurePath(b, "third_party/vulkan/include/vk_video/vulkan_video_codec_h264std.h", "Vulkan video header");
            ensurePath(b, "third_party/freetype/include/ft2build.h", "FreeType header");

            ensurePath(b, "third_party/glfw/lib/windows/glfw3.lib", "GLFW import library");
            ensurePath(b, "third_party/vulkan/lib/windows/vulkan-1.lib", "Vulkan import library");
            ensurePath(b, "third_party/freetype/lib/windows/freetype.lib", "FreeType import library");
            ensurePath(b, "third_party/glfw/lib/windows/glfw3.dll", "GLFW runtime DLL");
            ensurePath(b, "third_party/freetype/lib/windows/freetype.dll", "FreeType runtime DLL");
        },
        else => {},
    }
}

fn pathExists(rel_path: []const u8) bool {
    std.fs.cwd().access(rel_path, .{}) catch return false;
    return true;
}

fn addIncludeIfPresent(mod: *std.Build.Module, b: *std.Build, rel_path: []const u8) void {
    if (pathExists(rel_path)) {
        mod.addIncludePath(b.path(rel_path));
    }
}

fn addLibPathIfPresent(mod: *std.Build.Module, b: *std.Build, rel_path: []const u8) void {
    if (pathExists(rel_path)) {
        mod.addLibraryPath(b.path(rel_path));
    }
}

fn addThirdPartyIncludePaths(
    mod: *std.Build.Module,
    b: *std.Build,
    target: std.Build.ResolvedTarget,
) void {
    _ = target;

    addIncludeIfPresent(mod, b, "third_party/glfw/include");
    addIncludeIfPresent(mod, b, "third_party/vulkan/include");
    addIncludeIfPresent(mod, b, "third_party/freetype/include");
}

fn addThirdPartyLibraryPaths(
    mod: *std.Build.Module,
    b: *std.Build,
    target: std.Build.ResolvedTarget,
) void {
    addLibPathIfPresent(mod, b, "third_party/glfw/lib");
    addLibPathIfPresent(mod, b, "third_party/vulkan/lib");
    addLibPathIfPresent(mod, b, "third_party/freetype/lib");

    switch (target.result.os.tag) {
        .linux => {
            addLibPathIfPresent(mod, b, "third_party/glfw/lib/linux");
            addLibPathIfPresent(mod, b, "third_party/vulkan/lib/linux");
            addLibPathIfPresent(mod, b, "third_party/freetype/lib/linux");
        },
        .windows => {
            addLibPathIfPresent(mod, b, "third_party/glfw/lib/windows");
            addLibPathIfPresent(mod, b, "third_party/vulkan/lib/windows");
            addLibPathIfPresent(mod, b, "third_party/freetype/lib/windows");
        },
        .macos => {
            addLibPathIfPresent(mod, b, "third_party/glfw/lib/macos");
            addLibPathIfPresent(mod, b, "third_party/vulkan/lib/macos");
            addLibPathIfPresent(mod, b, "third_party/freetype/lib/macos");
        },
        else => {},
    }
}

fn addPlatformLibs(mod: *std.Build.Module, b: *std.Build, target: std.Build.ResolvedTarget) void {
    addThirdPartyLibraryPaths(mod, b, target);

    switch (target.result.os.tag) {
        .linux => {
            mod.linkSystemLibrary("glfw", .{});
            mod.linkSystemLibrary("vulkan", .{});
            mod.linkSystemLibrary("freetype", .{});
        },
        .windows => {
            mod.linkSystemLibrary("glfw3", .{});
            mod.linkSystemLibrary("vulkan-1", .{});
            mod.linkSystemLibrary("freetype", .{});
        },
        .macos => {
            mod.linkSystemLibrary("glfw", .{});
            mod.linkSystemLibrary("freetype", .{});
            mod.linkFramework("Metal", .{});
            mod.linkFramework("QuartzCore", .{});
        },
        else => {},
    }
}

fn buildExample(
    b: *std.Build,
    comptime name: []const u8,
    src: []const u8,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    lib: *std.Build.Step.Compile,
) void {
    const mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    mod.addCSourceFiles(.{
        .files = &.{src},
        .flags = &.{"-std=c11"},
    });
    mod.addIncludePath(b.path("include"));
    addThirdPartyIncludePaths(mod, b, target);
    mod.linkLibrary(lib);
    // Examples need the platform libs since they run the real main loop
    addPlatformLibs(mod, b, target);

    const exe = b.addExecutable(.{
        .name = name,
        .root_module = mod,
    });
    b.installArtifact(exe);

    const run = b.addRunArtifact(exe);
    const step = b.step("run-" ++ name, "Run the " ++ name ++ " example");
    step.dependOn(&run.step);
}
