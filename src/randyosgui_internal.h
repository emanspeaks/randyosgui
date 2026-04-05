#ifndef RANDY_INTERNAL_H
#define RANDY_INTERNAL_H

#include "../include/randyosgui.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* =========================================================================
 * Internal types Ã¢â‚¬â€ not exposed in the public API
 * ========================================================================= */

/* Size policy Ã¢â‚¬â€ Clay-inspired layout sizing model */
typedef enum {
    SIZE_FIXED   = 0,  /* exact pixel size */
    SIZE_FIT     = 1,  /* shrink to content */
    SIZE_GROW    = 2,  /* expand to fill remaining space */
    SIZE_PERCENT = 3,  /* percentage of parent container */
} SizePolicy;

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
    /* Layout containers */
    WIDGET_VBOX = 16,
    WIDGET_HBOX = 17,
    /* New widget types */
    WIDGET_SEPARATOR = 18,
    WIDGET_SPINBOX = 19,
    WIDGET_SCROLL_AREA = 20,
    WIDGET_MENUBAR = 21,
    WIDGET_MENU_ITEM = 22,
    WIDGET_TOOLBAR = 23,
    WIDGET_COMBOBOX = 24,
    WIDGET_TEXTEDIT = 25,
    WIDGET_LISTBOX = 26,
    WIDGET_IMAGE = 27,
    WIDGET_STACKED = 28,
    WIDGET_TAB_WIDGET = 29,
    WIDGET_ACCORDION = 30,
    WIDGET_TOOLTIP = 31,
    WIDGET_DIALOG = 32,
} WidgetKind;

/* A single widget node in the retained tree */
typedef struct Widget {
    RandyWidgetId     id;
    WidgetKind           kind;

    char*                text;       /* label text or button label */

    /* Tree structure */
    struct Widget*       parent;
    struct Widget*       first_child;
    struct Widget*       next_sibling;

    /* Size policy (Clay-inspired) */
    SizePolicy           size_policy_h;  /* horizontal sizing */
    SizePolicy           size_policy_v;  /* vertical sizing */
    int                  pref_w, pref_h; /* preferred/content size (pixels) */
    int                  min_w, min_h;   /* minimum bounds (0 = none) */
    int                  max_w, max_h;   /* maximum bounds (0 = none) */
    int                  size_percent;   /* used when policy is SIZE_PERCENT */
    int                  grow_weight;    /* relative weight for SIZE_GROW (default 1) */

    /* Container layout properties */
    int                  spacing;        /* space between children (containers only) */
    int                  padding;        /* internal padding (containers only) */

    /* Visibility and enabled state */
    bool                 visible;
    bool                 enabled;

    /* Table cell data (WIDGET_TABLE_HEADER and WIDGET_TABLE_ROW only) */
    char**               cells;      /* owned array of num_cells strings */
    int*                 col_widths; /* owned array of num_cells pixel widths (header only) */
    int                  num_cells;

    RandyClickCallback click_cb;
    void*                click_userdata;
    RandyToggleCallback toggle_cb;
    void*                toggle_userdata;
    RandyValueCallback value_cb;
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
} Widget;

/* Platform-specific window state Ã¢â‚¬â€ defined in platform/platform.c */
typedef struct PlatformWindow PlatformWindow;

/* Renderer state Ã¢â‚¬â€ defined in renderer/renderer.c */
typedef struct RendererContext RendererContext;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    float r;
    float g;
    float b;
} RandyDrawOp;

/* Internal window */
struct RandyWindow {
    RandyContext* ctx;
    PlatformWindow*  platform;
    RendererContext* renderer;

    Widget*          root;           /* implicit root VBOX container */
    uint32_t         next_id;

    /* Basic layout and interaction state */
    bool             needs_layout;
    double           mouse_x;
    double           mouse_y;
    bool             mouse_down;
    bool             prev_mouse_down;
    RandyWidgetId hot_id;
    RandyWidgetId active_id;

    /* Last known framebuffer size for layout invalidation on resize */
    int              layout_fb_w;
    int              layout_fb_h;

    /* Dirty flag Ã¢â‚¬â€ set whenever visible state changes, cleared after render */
    bool             needs_render;
};

/* Internal context */
struct RandyContext {
    RandyWindow** windows;
    uint32_t         window_count;
    uint32_t         window_cap;
};

/* -------------------------------------------------------------------------
 * Internal helpers
 * ---------------------------------------------------------------------- */

static inline void* randy_alloc(size_t size) {
    void* p = malloc(size);
    if (!p) {
        fprintf(stderr, "[randy] allocation failed (%zu bytes)\n", size);
    }
    return p;
}

static inline void* randy_zalloc(size_t size) {
    void* p = calloc(1, size);
    if (!p) {
        fprintf(stderr, "[randy] allocation failed (%zu bytes)\n", size);
    }
    return p;
}

static inline char* randy_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = (char*)randy_alloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

static inline int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* Widget helpers Ã¢â‚¬â€ implemented in randyosgui.c */
Widget* widget_alloc(RandyWindow* win, WidgetKind kind, const char* text);
Widget* widget_find(RandyWindow* win, RandyWidgetId id);
void    widget_add_child(Widget* parent, Widget* child);
void    widget_remove_from_parent(Widget* child);
Widget* widget_next_depth_first(Widget* root, Widget* current);
void    widget_set_default_size_hints(Widget* w);

/* Layout engine Ã¢â‚¬â€ implemented in layout.c */
void    layout_widgets(RandyWindow* win);

/* Input handling Ã¢â‚¬â€ implemented in input.c */
void    update_window_input_and_clicks(RandyWindow* win);

/* Platform interface Ã¢â‚¬â€ implemented in platform/platform.c */
PlatformWindow* platform_window_create(const RandyWindowDesc* desc);
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

/* Renderer interface Ã¢â‚¬â€ implemented in renderer/renderer_vk.c + renderer_draw.c + renderer_widgets.c */
RendererContext* renderer_create(PlatformWindow* win);
void             renderer_destroy(RendererContext* r);
bool             renderer_render(RendererContext* r, Widget* widgets);
size_t           renderer_test_capture_widget_draw_ops(const Widget* widget,
                                                       uint32_t width,
                                                       uint32_t height,
                                                       RandyDrawOp* out_ops,
                                                       size_t max_ops);

#endif /* RANDY_INTERNAL_H */
