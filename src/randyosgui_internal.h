#ifndef RANDYOSGUI_INTERNAL_H
#define RANDYOSGUI_INTERNAL_H

#include "../include/randyosgui.h"
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
    WIDGET_CHECKBOX = 2,
    WIDGET_RADIO = 3,
    WIDGET_TEXTBOX = 4,
    WIDGET_DROPDOWN = 5,
    WIDGET_SLIDER = 6,
    WIDGET_PROGRESS = 7,
    WIDGET_GROUPBOX = 8,
    WIDGET_TAB = 9,
    WIDGET_TREE_ITEM = 10,
    WIDGET_TABLE_HEADER = 11,
    WIDGET_TABLE_ROW = 12,
    WIDGET_FIELD_BORDER = 13,
    WIDGET_STATUS_FIELD = 14,
    WIDGET_SUNKEN_PANEL = 15,
} WidgetKind;

/* A single widget node in the retained tree */
typedef struct Widget {
    RandyosgWidgetId     id;
    WidgetKind           kind;

    char*                text;       /* label text or button label */

    /* Table cell data (WIDGET_TABLE_HEADER and WIDGET_TABLE_ROW only) */
    char**               cells;      /* owned array of num_cells strings */
    int*                 col_widths; /* owned array of num_cells pixel widths (header only) */
    int                  num_cells;

    RandyosgClickCallback click_cb;
    void*                click_userdata;
    RandyosgToggleCallback toggle_cb;
    void*                toggle_userdata;
    RandyosgValueCallback value_cb;
    void*                value_userdata;
    bool                 checked;
    bool                 readonly;
    int                  value;
    int                  min_value;
    int                  max_value;

    /* Layout hints (to be expanded) */
    int                  x, y, w, h;

    /* Derived per-frame interaction state */
    bool                 hovered;
    bool                 pressed;

    struct Widget*       next;       /* singly-linked list per window */
} Widget;

/* Platform-specific window state — defined in platform/platform.c */
typedef struct PlatformWindow PlatformWindow;

/* Renderer state — defined in renderer/renderer.c */
typedef struct RendererContext RendererContext;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    float r;
    float g;
    float b;
} RandyosgDrawOp;

/* Internal window */
struct RandyosgWindow {
    RandyosgContext* ctx;
    PlatformWindow*  platform;
    RendererContext* renderer;

    Widget*          widgets;        /* head of widget list */
    uint32_t         next_id;

    /* Basic layout and interaction state */
    bool             needs_layout;
    double           mouse_x;
    double           mouse_y;
    bool             mouse_down;
    bool             prev_mouse_down;
    RandyosgWidgetId hot_id;
    RandyosgWidgetId active_id;

    /* Last known framebuffer size for layout invalidation on resize */
    int              layout_fb_w;
    int              layout_fb_h;

    /* Dirty flag — set whenever visible state changes, cleared after render */
    bool             needs_render;
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

static inline int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* Widget helpers — implemented in randyosgui.c */
Widget* widget_alloc(RandyosgWindow* win, WidgetKind kind, const char* text);
Widget* widget_find(RandyosgWindow* win, RandyosgWidgetId id);

/* Platform interface — implemented in platform/platform.c */
PlatformWindow* platform_window_create(const RandyosgWindowDesc* desc);
void            platform_window_destroy(PlatformWindow* win);
bool            platform_window_should_close(PlatformWindow* win);
void            platform_window_set_title(PlatformWindow* win, const char* title);
void            platform_window_get_size(PlatformWindow* win, int* w, int* h);
void            platform_window_get_cursor_pos(PlatformWindow* win, double* x, double* y);
bool            platform_window_is_mouse_down(PlatformWindow* win, int button);
void            platform_poll_events(void);
void            platform_wake_event_loop(void);

/* Vulkan surface helpers (called by renderer during init/teardown) */
#include <vulkan/vulkan.h>
bool        platform_check_vulkan_support(void);
const char** platform_get_required_instance_extensions(uint32_t* count);
bool        platform_create_surface(PlatformWindow* win, VkInstance instance,
                                    VkSurfaceKHR* out_surface);
void        platform_destroy_surface(PlatformWindow* win);

/* Renderer interface — implemented in renderer/renderer_vk.c + renderer_draw.c + renderer_widgets.c */
RendererContext* renderer_create(PlatformWindow* win);
void             renderer_destroy(RendererContext* r);
bool             renderer_render(RendererContext* r, Widget* widgets);
size_t           renderer_test_capture_widget_draw_ops(const Widget* widget,
                                                       uint32_t width,
                                                       uint32_t height,
                                                       RandyosgDrawOp* out_ops,
                                                       size_t max_ops);

#endif /* RANDYOSGUI_INTERNAL_H */
