# AGENTS

## Project Intent

randyosgui is a cross-platform retained-mode GUI framework with a C-first API and stable ABI, designed to be easy to bind from higher-level languages (for example Python).

## Product Goals

- Prefer a simple, ergonomic C API over heavy framework overhead.
- Deliver modern GPU-accelerated rendering on supported targets.
- Keep platform support intentionally narrow and modern:
  - Wayland Linux
  - Windows 11
  - Current macOS on Intel and Apple Silicon
- Preserve a clean abstraction boundary so app code and language bindings do not depend on platform or renderer internals.

## Visual Direction

- Prioritize clear, practical widget styling inspired by classic desktop UI simplicity.
- Avoid CSS runtime dependencies.
- If references such as 98.css are used, treat them as visual inspiration only and re-implement behavior natively in code.

## Rendering and Platform Strategy

- Build widget rendering on top of backend abstractions that can target Clay-compatible renderers where practical.
- Favor explicit control of the graphics stack while keeping backend selection modular.
- Vulkan is a primary long-term direction for modern cross-platform GPU access.
- Evaluate alternatives at the same layer when they provide meaningful implementation or maintenance advantages.

## Text and Font Requirements

- Support TTF from the outset.
- Prefer FreeType for robust, standard text shaping/rasterization workflows over bitmap-only approaches.
- Keep text pipeline architecture open for future shaping/layout improvements.

## macOS Vulkan Note

- On macOS, Vulkan commonly runs through MoltenVK (Vulkan-over-Metal translation).
- Backend and deployment decisions should account for MoltenVK packaging/runtime expectations.

## Engineering Principles

- C-first public API with opaque handles.
- Predictable memory ownership and explicit lifecycle functions.
- Minimal hidden global state.
- Keep internals modular: platform, renderer, widgets, text.
- Optimize for maintainability, testability, and binding-friendliness.

## Zig Toolchain Notes

- Project Zig version is pinned to 0.15.2. Do not assume APIs from older or newer Zig releases.
- When editing `build.zig`, prefer validating API usage against Zig 0.15.2 stdlib signatures first.
- Known 0.15.2 pattern in this repo:
  - `std.ArrayList(T)` operations are allocator-aware in common paths.
  - Use allocator-aware forms for init/append/deinit/owned-slice conversions (for example `initCapacity(alloc, ...)`, `append(alloc, ...)`, `appendSlice(alloc, ...)`, `deinit(alloc)`, `toOwnedSlice(alloc)`).
- Avoid guessing JSON helpers across Zig versions.
  - If a `std.json` helper name is uncertain for 0.15.2, either verify it in the local Zig stdlib or emit JSON manually with a writer.
- For cross-platform tooling, prefer native Zig build logic over shell-specific scripts unless there is a strong reason otherwise.
