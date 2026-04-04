/*
 * renderer.c - SPLIT
 *
 * The renderer has been split into three focused translation units:
 *
 *   renderer_vk.c      - Vulkan lifecycle (instance, device, swapchain, sync)
 *   renderer_draw.c    - Win98 theme palette, text system, draw primitives
 *   renderer_widgets.c - Widget dispatch loop, frame render, test capture
 *
 * This file is intentionally empty; it is no longer compiled directly.
 * See build.zig lib_sources for the replacement file list.
 */
