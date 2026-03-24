const std = @import("std");

pub fn build(b: *std.Build) void {
    const target   = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

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
    const lib = b.addStaticLibrary(.{
        .name   = "randyosgui",
        .target = target,
        .optimize = optimize,
    });
    lib.addCSourceFiles(.{ .files = lib_sources, .flags = c_flags });
    lib.addIncludePath(b.path("include"));
    lib.addIncludePath(b.path("src"));
    lib.linkLibC();
    addPlatformLibs(lib, target);
    b.installArtifact(lib);

    // --- Shared library ---
    const shared = b.addSharedLibrary(.{
        .name   = "randyosgui",
        .target = target,
        .optimize = optimize,
    });
    shared.addCSourceFiles(.{ .files = lib_sources, .flags = c_flags });
    shared.addIncludePath(b.path("include"));
    shared.addIncludePath(b.path("src"));
    shared.linkLibC();
    addPlatformLibs(shared, target);
    b.installArtifact(shared);

    // Install public header
    b.installFile("include/randyosgui.h", "include/randyosgui.h");

    // --- Examples ---
    buildExample(b, "hello", "examples/hello/main.c", target, optimize, lib);

    // --- Tests ---
    const test_exe = b.addExecutable(.{
        .name   = "test_randyosgui",
        .target = target,
        .optimize = optimize,
    });
    test_exe.addCSourceFiles(.{
        .files = &.{"tests/test_randyosgui.c"},
        .flags = c_flags,
    });
    test_exe.addIncludePath(b.path("include"));
    test_exe.linkLibrary(lib);

    const run_tests = b.addRunArtifact(test_exe);
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_tests.step);
}

fn addPlatformLibs(step: *std.Build.Step.Compile, target: std.Build.ResolvedTarget) void {
    switch (target.result.os.tag) {
        .linux => {
            step.linkSystemLibrary("glfw");
            step.linkSystemLibrary("vulkan");
            step.linkSystemLibrary("freetype");
        },
        .windows => {
            step.linkSystemLibrary("glfw3");
            step.linkSystemLibrary("vulkan-1");
            step.linkSystemLibrary("freetype");
        },
        .macos => {
            step.linkSystemLibrary("glfw");
            step.linkSystemLibrary("freetype");
            step.linkFramework("Metal");
            step.linkFramework("QuartzCore");
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
    const exe = b.addExecutable(.{
        .name   = name,
        .target = target,
        .optimize = optimize,
    });
    exe.addCSourceFiles(.{
        .files = &.{src},
        .flags = &.{"-std=c11"},
    });
    exe.addIncludePath(b.path("include"));
    exe.linkLibrary(lib);
    b.installArtifact(exe);

    const run  = b.addRunArtifact(exe);
    const step = b.step("run-" ++ name, "Run the " ++ name ++ " example");
    step.dependOn(&run.step);
}
