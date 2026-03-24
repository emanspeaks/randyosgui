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
    // System libs are NOT linked here; they're added by each consumer so that
    // lld doesn't try to bundle .so stubs into the .a archive.
    const lib_mod = b.createModule(.{
        .target    = target,
        .optimize  = optimize,
        .link_libc = true,
    });
    lib_mod.addCSourceFiles(.{ .files = lib_sources, .flags = c_flags });
    lib_mod.addIncludePath(b.path("include"));
    lib_mod.addIncludePath(b.path("src"));

    const lib = b.addLibrary(.{
        .name        = "randyosgui",
        .root_module = lib_mod,
        .linkage     = .static,
    });
    b.installArtifact(lib);

    // --- Shared library ---
    // Shared lib IS fully linked, so platform libs go here.
    const shared_mod = b.createModule(.{
        .target    = target,
        .optimize  = optimize,
        .link_libc = true,
    });
    shared_mod.addCSourceFiles(.{ .files = lib_sources, .flags = c_flags });
    shared_mod.addIncludePath(b.path("include"));
    shared_mod.addIncludePath(b.path("src"));
    addPlatformLibs(shared_mod, target);

    const shared = b.addLibrary(.{
        .name        = "randyosgui",
        .root_module = shared_mod,
        .linkage     = .dynamic,
    });
    b.installArtifact(shared);

    // Install public header
    b.installFile("include/randyosgui.h", "include/randyosgui.h");

    // --- Examples ---
    buildExample(b, "hello", "examples/hello/main.c", target, optimize, lib);

    // --- Tests ---
    // Tests use stub platform/renderer so no external libs needed at link time.
    const test_mod = b.createModule(.{
        .target    = target,
        .optimize  = optimize,
        .link_libc = true,
    });
    test_mod.addCSourceFiles(.{
        .files = &.{"tests/test_randyosgui.c"},
        .flags = c_flags,
    });
    test_mod.addIncludePath(b.path("include"));
    test_mod.linkLibrary(lib);

    const test_exe = b.addExecutable(.{
        .name        = "test_randyosgui",
        .root_module = test_mod,
    });

    const run_tests = b.addRunArtifact(test_exe);
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_tests.step);
}

fn addPlatformLibs(mod: *std.Build.Module, target: std.Build.ResolvedTarget) void {
    switch (target.result.os.tag) {
        .linux => {
            mod.linkSystemLibrary("glfw",     .{});
            mod.linkSystemLibrary("vulkan",   .{});
            mod.linkSystemLibrary("freetype", .{});
        },
        .windows => {
            mod.linkSystemLibrary("glfw3",    .{});
            mod.linkSystemLibrary("vulkan-1", .{});
            mod.linkSystemLibrary("freetype", .{});
        },
        .macos => {
            mod.linkSystemLibrary("glfw",     .{});
            mod.linkSystemLibrary("freetype", .{});
            mod.linkFramework("Metal",        .{});
            mod.linkFramework("QuartzCore",   .{});
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
        .target    = target,
        .optimize  = optimize,
        .link_libc = true,
    });
    mod.addCSourceFiles(.{
        .files = &.{src},
        .flags = &.{"-std=c11"},
    });
    mod.addIncludePath(b.path("include"));
    mod.linkLibrary(lib);
    // Examples need the platform libs since they run the real main loop
    addPlatformLibs(mod, target);

    const exe = b.addExecutable(.{
        .name        = name,
        .root_module = mod,
    });
    b.installArtifact(exe);

    const run  = b.addRunArtifact(exe);
    const step = b.step("run-" ++ name, "Run the " ++ name ++ " example");
    step.dependOn(&run.step);
}
