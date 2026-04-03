#include "../randyosgui_internal.h"

#if RANDYOSGUI_HAS_VULKAN
#  if defined(__has_include)
#    if __has_include(<GLFW/glfw3.h>)
#      define RANDYOSGUI_HAS_GLFW 1
#    else
#      define RANDYOSGUI_HAS_GLFW 0
#    endif
#  else
#    define RANDYOSGUI_HAS_GLFW 1
#  endif
#else
#  define RANDYOSGUI_HAS_GLFW 0
#endif

#if RANDYOSGUI_HAS_GLFW
#  define GLFW_INCLUDE_VULKAN
#  include <GLFW/glfw3.h>
#endif

#if RANDYOSGUI_HAS_GLFW

/*
 * Platform backend — GLFW 3
 *
 * Responsibilities:
 *   - One-time glfwInit / glfwTerminate (reference-counted via window count)
 *   - Window creation with GLFW_NO_API (Vulkan handles rendering)
 *   - VkSurfaceKHR creation and exposure to the renderer
 *   - Input event polling
 */

/* =========================================================================
 * GLFW init reference count — init once, terminate when last window gone
 * ========================================================================= */

static int g_glfw_refcount = 0;

static bool glfw_acquire(void) {
    if (g_glfw_refcount == 0) {
        if (!glfwInit()) {
            fprintf(stderr, "[randyosgui/platform] glfwInit() failed\n");
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
    VkInstance   instance;  /* borrowed reference — renderer owns lifetime   */
};

PlatformWindow* platform_window_create(const RandyosgWindowDesc* desc) {
    if (!glfw_acquire()) return NULL;

    PlatformWindow* win = (PlatformWindow*)randyosgui_zalloc(sizeof(PlatformWindow));
    if (!win) {
        glfw_release();
        return NULL;
    }

    /* No OpenGL context — Vulkan will own the surface */
    glfwWindowHint(GLFW_CLIENT_API,  GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,   desc->resizable  ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED,   desc->decorated  ? GLFW_TRUE : GLFW_FALSE);

    win->handle = glfwCreateWindow(desc->width, desc->height,
                                   desc->title ? desc->title : "",
                                   NULL, NULL);
    if (!win->handle) {
        fprintf(stderr, "[randyosgui/platform] glfwCreateWindow() failed\n");
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

void platform_poll_events(void) {
    glfwPollEvents();
}

/* =========================================================================
 * Vulkan surface helpers — called by the renderer
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
        fprintf(stderr, "[randyosgui/platform] glfwCreateWindowSurface failed: %d\n",
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

#else

struct PlatformWindow {
    int unused;
};

PlatformWindow* platform_window_create(const RandyosgWindowDesc* desc) {
    (void)desc;
    fprintf(stderr, "[randyosgui/platform] GLFW and Vulkan headers not found; platform backend disabled\n");
    return NULL;
}

void platform_window_destroy(PlatformWindow* win) {
    free(win);
}

bool platform_window_should_close(PlatformWindow* win) {
    (void)win;
    return true;
}

void platform_window_set_title(PlatformWindow* win, const char* title) {
    (void)win;
    (void)title;
}

void platform_window_get_size(PlatformWindow* win, int* w, int* h) {
    (void)win;
    if (w) *w = 0;
    if (h) *h = 0;
}

void platform_poll_events(void) {
}

bool platform_check_vulkan_support(void) {
    return false;
}

const char** platform_get_required_instance_extensions(uint32_t* count) {
    if (count) *count = 0;
    return NULL;
}

bool platform_create_surface(PlatformWindow* win, VkInstance instance,
                             VkSurfaceKHR* out_surface) {
    (void)win;
    (void)instance;
    (void)out_surface;
    return false;
}

void platform_destroy_surface(PlatformWindow* win) {
    (void)win;
}

#endif
