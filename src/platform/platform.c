#include "../randyosgui_internal.h"

/*
 * Platform backend — GLFW
 *
 * GLFW handles window creation, input, and surface creation across
 * Linux (X11/Wayland), macOS, and Windows.  Vulkan surface creation
 * is handled here so the renderer only ever sees a VkSurfaceKHR.
 */

/* TODO: include GLFW + Vulkan headers once the package deps are wired in */
/* #include <GLFW/glfw3.h> */

struct PlatformWindow {
    /* GLFWwindow* handle; */
    int  width;
    int  height;
    char title[256];
    bool should_close;
};

PlatformWindow* platform_window_create(const RandyosgWindowDesc* desc) {
    PlatformWindow* win = (PlatformWindow*)randyosgui_zalloc(sizeof(PlatformWindow));
    if (!win) return NULL;

    win->width        = desc->width;
    win->height       = desc->height;
    win->should_close = false;

    if (desc->title) {
        snprintf(win->title, sizeof(win->title), "%s", desc->title);
    }

    /* TODO: glfwInit() + glfwCreateWindow() + Vulkan surface setup */
    fprintf(stderr, "[randyosgui/platform] stub: window '%s' (%dx%d) created\n",
            win->title, win->width, win->height);

    return win;
}

void platform_window_destroy(PlatformWindow* win) {
    if (!win) return;
    /* TODO: glfwDestroyWindow() */
    free(win);
}

bool platform_window_should_close(PlatformWindow* win) {
    if (!win) return true;
    /* TODO: return glfwWindowShouldClose(win->handle); */
    return win->should_close;
}

void platform_window_set_title(PlatformWindow* win, const char* title) {
    if (!win || !title) return;
    snprintf(win->title, sizeof(win->title), "%s", title);
    /* TODO: glfwSetWindowTitle(win->handle, title); */
}

void platform_window_get_size(PlatformWindow* win, int* w, int* h) {
    if (!win) return;
    if (w) *w = win->width;
    if (h) *h = win->height;
    /* TODO: glfwGetFramebufferSize(win->handle, w, h); */
}

void platform_poll_events(void) {
    /* TODO: glfwPollEvents(); */
}
