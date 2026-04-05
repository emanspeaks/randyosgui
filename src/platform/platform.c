#include "../randyosgui_internal.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/*
 * Platform backend â€” GLFW 3
 *
 * Responsibilities:
 *   - One-time glfwInit / glfwTerminate (reference-counted via window count)
 *   - Window creation with GLFW_NO_API (Vulkan handles rendering)
 *   - VkSurfaceKHR creation and exposure to the renderer
 *   - Input event polling
 */

/* =========================================================================
 * GLFW init reference count â€” init once, terminate when last window gone
 * ========================================================================= */

static int g_glfw_refcount = 0;

static bool glfw_acquire(void) {
    if (g_glfw_refcount == 0) {
        if (!glfwInit()) {
            fprintf(stderr, "[randy/platform] glfwInit() failed\n");
            return false;
        }
    }
    g_glfw_refcount++;
    return true;
}

static void glfw_release(void) {
    if (g_glfw_refcount > 0) {
        g_glfw_refcount--;
        if (g_glfw_refcount == 0) {
            glfwTerminate();
        }
    }
}

/* =========================================================================
 * PlatformWindow
 * ========================================================================= */

struct PlatformWindow {
    GLFWwindow*  handle;
    VkSurfaceKHR surface;   /* owned by the renderer; stored here for access */
    VkInstance   instance;  /* borrowed reference â€” renderer owns lifetime   */
};

PlatformWindow* platform_window_create(const RandyWindowDesc* desc) {
    if (!glfw_acquire()) return NULL;

    PlatformWindow* win = (PlatformWindow*)randy_zalloc(sizeof(PlatformWindow));
    if (!win) {
        glfw_release();
        return NULL;
    }

    /* No OpenGL context â€” Vulkan will own the surface */
    glfwWindowHint(GLFW_CLIENT_API,  GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,   desc->resizable  ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED,   desc->decorated  ? GLFW_TRUE : GLFW_FALSE);

    win->handle = glfwCreateWindow(desc->width, desc->height,
                                   desc->title ? desc->title : "",
                                   NULL, NULL);
    if (!win->handle) {
        fprintf(stderr, "[randy/platform] glfwCreateWindow() failed\n");
        free(win);
        glfw_release();
        return NULL;
    }

    return win;
}

void platform_window_destroy(PlatformWindow* win) {
    if (!win) return;

    /* Surface is destroyed by the renderer before this is called */
    if (win->handle) {
        glfwDestroyWindow(win->handle);
    }
    free(win);
    glfw_release();
}

bool platform_window_should_close(PlatformWindow* win) {
    if (!win || !win->handle) return true;
    return glfwWindowShouldClose(win->handle);
}

void platform_window_set_title(PlatformWindow* win, const char* title) {
    if (!win || !win->handle || !title) return;
    glfwSetWindowTitle(win->handle, title);
}

void platform_window_get_size(PlatformWindow* win, int* w, int* h) {
    if (!win || !win->handle) return;
    glfwGetFramebufferSize(win->handle, w, h);
}

void platform_window_get_cursor_pos(PlatformWindow* win, double* x, double* y) {
    if (!win || !win->handle) return;
    glfwGetCursorPos(win->handle, x, y);
}

bool platform_window_is_mouse_down(PlatformWindow* win, int button) {
    if (!win || !win->handle) return false;
    return glfwGetMouseButton(win->handle, button) == GLFW_PRESS;
}

void platform_poll_events(void) {
    glfwWaitEvents();
}

void platform_wake_event_loop(void) {
    glfwPostEmptyEvent();
}

/* =========================================================================
 * Vulkan surface helpers â€” called by the renderer
 * ========================================================================= */

bool platform_check_vulkan_support(void) {
    return glfwVulkanSupported() == GLFW_TRUE;
}

const char** platform_get_required_instance_extensions(uint32_t* count) {
    return glfwGetRequiredInstanceExtensions(count);
}

bool platform_create_surface(PlatformWindow* win, VkInstance instance,
                              VkSurfaceKHR* out_surface) {
    if (!win || !win->handle) return false;
    VkResult result = glfwCreateWindowSurface(instance, win->handle,
                                              NULL, out_surface);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "[randy/platform] glfwCreateWindowSurface failed: %d\n",
                result);
        return false;
    }
    /* Stash for later retrieval (e.g. renderer resize callbacks) */
    win->surface  = *out_surface;
    win->instance = instance;
    return true;
}

void platform_destroy_surface(PlatformWindow* win) {
    if (!win || win->surface == VK_NULL_HANDLE) return;
    vkDestroySurfaceKHR(win->instance, win->surface, NULL);
    win->surface  = VK_NULL_HANDLE;
    win->instance = VK_NULL_HANDLE;
}
