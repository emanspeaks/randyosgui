#ifndef RANDYOSGUI_INTERNAL_H
#define RANDYOSGUI_INTERNAL_H

#include "randyosgui.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* =========================================================================
 * Internal types — not exposed in the public API
 * ========================================================================= */

/* Widget kinds */
typedef enum {
    WIDGET_LABEL  = 0,
    WIDGET_BUTTON = 1,
} WidgetKind;

/* A single widget node in the retained tree */
typedef struct Widget {
    RandyosgWidgetId     id;
    WidgetKind           kind;

    char*                text;       /* label text or button label */
    RandyosgClickCallback click_cb;
    void*                click_userdata;

    /* Layout hints (to be expanded) */
    int                  x, y, w, h;

    struct Widget*       next;       /* singly-linked list per window */
} Widget;

/* Platform-specific window state — defined in platform/platform.c */
typedef struct PlatformWindow PlatformWindow;

/* Renderer state — defined in renderer/renderer.c */
typedef struct RendererContext RendererContext;

/* Internal window */
struct RandyosgWindow {
    RandyosgContext* ctx;
    PlatformWindow*  platform;
    RendererContext* renderer;

    Widget*          widgets;        /* head of widget list */
    uint32_t         next_id;
};

/* Internal context */
struct RandyosgContext {
    RandyosgWindow** windows;
    uint32_t         window_count;
    uint32_t         window_cap;
};

/* -------------------------------------------------------------------------
 * Internal helpers
 * ---------------------------------------------------------------------- */

static inline void* randyosgui_alloc(size_t size) {
    void* p = malloc(size);
    if (!p) {
        fprintf(stderr, "[randyosgui] allocation failed (%zu bytes)\n", size);
    }
    return p;
}

static inline void* randyosgui_zalloc(size_t size) {
    void* p = calloc(1, size);
    if (!p) {
        fprintf(stderr, "[randyosgui] allocation failed (%zu bytes)\n", size);
    }
    return p;
}

static inline char* randyosgui_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = (char*)randyosgui_alloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

/* Platform interface — implemented in platform/platform.c */
PlatformWindow* platform_window_create(const RandyosgWindowDesc* desc);
void            platform_window_destroy(PlatformWindow* win);
bool            platform_window_should_close(PlatformWindow* win);
void            platform_window_set_title(PlatformWindow* win, const char* title);
void            platform_window_get_size(PlatformWindow* win, int* w, int* h);
void            platform_poll_events(void);

/* Vulkan surface helpers (called by renderer during init/teardown) */
#include <vulkan/vulkan.h>
bool        platform_check_vulkan_support(void);
const char** platform_get_required_instance_extensions(uint32_t* count);
bool        platform_create_surface(PlatformWindow* win, VkInstance instance,
                                    VkSurfaceKHR* out_surface);
void        platform_destroy_surface(PlatformWindow* win);

/* Renderer interface — implemented in renderer/renderer.c */
RendererContext* renderer_create(PlatformWindow* win);
void             renderer_destroy(RendererContext* r);
void             renderer_render(RendererContext* r, Widget* widgets);

#endif /* RANDYOSGUI_INTERNAL_H */
