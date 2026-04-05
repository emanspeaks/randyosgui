#include "randyosgui_internal.h"
#include "style.h"

/* =========================================================================
 * Layout constants (now fed from g_style metrics)
 * ========================================================================= */

#define RANDY_LAYOUT_PADDING_X    g_style.content_padding_x
#define RANDY_LAYOUT_PADDING_TOP  g_style.title_bar_height
#define RANDY_LAYOUT_SPACING      g_style.default_spacing
#define RANDY_LABEL_HEIGHT        g_style.label_height
#define RANDY_BUTTON_HEIGHT       g_style.button_height
#define RANDY_CHECKBOX_HEIGHT     g_style.checkbox_height
#define RANDY_TEXTBOX_HEIGHT      g_style.textbox_height
#define RANDY_DROPDOWN_HEIGHT     g_style.dropdown_height
#define RANDY_SLIDER_HEIGHT       g_style.slider_height
#define RANDY_PROGRESS_HEIGHT     g_style.progress_height
#define RANDY_GROUPBOX_HEIGHT     g_style.groupbox_height
#define RANDY_TAB_HEIGHT          g_style.tab_height
#define RANDY_TAB_WIDTH           g_style.tab_width
#define RANDY_TREE_ITEM_HEIGHT    g_style.tree_item_height
#define RANDY_TABLE_HEADER_HEIGHT g_style.table_header_height
#define RANDY_TABLE_ROW_HEIGHT    g_style.table_row_height
#define RANDY_FIELD_BORDER_HEIGHT g_style.field_border_height
#define RANDY_STATUS_FIELD_HEIGHT g_style.status_field_height
#define RANDY_SUNKEN_PANEL_HEIGHT g_style.sunken_panel_height
#define RANDY_LAYOUT_BOTTOM_MARGIN g_style.content_bottom_margin

/* =========================================================================
 * Widget tree helpers
 * ========================================================================= */

void widget_add_child(Widget* parent, Widget* child) {
    if (!parent || !child) return;

    /* Remove from current parent if any */
    widget_remove_from_parent(child);

    child->parent = parent;
    if (!parent->first_child) {
        parent->first_child = child;
    } else {
        Widget* tail = parent->first_child;
        while (tail->next_sibling) tail = tail->next_sibling;
        tail->next_sibling = child;
    }
}

void widget_remove_from_parent(Widget* child) {
    if (!child || !child->parent) return;

    Widget* parent = child->parent;
    if (parent->first_child == child) {
        parent->first_child = child->next_sibling;
    } else {
        Widget* prev = parent->first_child;
        while (prev && prev->next_sibling != child) prev = prev->next_sibling;
        if (prev) prev->next_sibling = child->next_sibling;
    }
    child->parent = NULL;
    child->next_sibling = NULL;
}

/* Depth-first traversal: returns next widget after `current` under `root`.
 * Pass current=NULL to start at root->first_child. */
Widget* widget_next_depth_first(Widget* root, Widget* current) {
    if (!current) return root ? root->first_child : NULL;

    /* Go deeper if possible */
    if (current->first_child) return current->first_child;

    /* Go to next sibling, or walk up and go to parent's next sibling */
    Widget* w = current;
    while (w && w != root) {
        if (w->next_sibling) return w->next_sibling;
        w = w->parent;
    }
    return NULL;
}

/* Set default preferred size and size policy based on widget kind */
void widget_set_default_size_hints(Widget* w) {
    if (!w) return;

    /* Most widgets: grow horizontally, fixed height */
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_FIXED;
    w->grow_weight   = 1;

    switch (w->kind) {
        case WIDGET_LABEL:        w->pref_h = RANDY_LABEL_HEIGHT; break;
        case WIDGET_BUTTON:       w->pref_h = RANDY_BUTTON_HEIGHT; break;
        case WIDGET_CHECKBOX:     w->pref_h = RANDY_CHECKBOX_HEIGHT; break;
        case WIDGET_RADIO:        w->pref_h = RANDY_CHECKBOX_HEIGHT; break;
        case WIDGET_TEXTBOX:      w->pref_h = RANDY_TEXTBOX_HEIGHT; break;
        case WIDGET_DROPDOWN:     w->pref_h = RANDY_DROPDOWN_HEIGHT; break;
        case WIDGET_SLIDER:       w->pref_h = RANDY_SLIDER_HEIGHT; break;
        case WIDGET_PROGRESS:     w->pref_h = RANDY_PROGRESS_HEIGHT; break;
        case WIDGET_GROUPBOX:
            w->pref_h = RANDY_GROUPBOX_HEIGHT;
            w->size_policy_v = SIZE_FIT;
            break;
        case WIDGET_TAB:
            w->pref_h = RANDY_TAB_HEIGHT;
            w->size_policy_h = SIZE_FIT;
            w->pref_w = RANDY_TAB_WIDTH;
            break;
        case WIDGET_TREE_ITEM:    w->pref_h = RANDY_TREE_ITEM_HEIGHT; break;
        case WIDGET_TABLE_HEADER: w->pref_h = RANDY_TABLE_HEADER_HEIGHT; break;
        case WIDGET_TABLE_ROW:    w->pref_h = RANDY_TABLE_ROW_HEIGHT; break;
        case WIDGET_FIELD_BORDER: w->pref_h = RANDY_FIELD_BORDER_HEIGHT; break;
        case WIDGET_STATUS_FIELD: w->pref_h = RANDY_STATUS_FIELD_HEIGHT; break;
        case WIDGET_SUNKEN_PANEL: w->pref_h = RANDY_SUNKEN_PANEL_HEIGHT; break;
        case WIDGET_VBOX:
            w->size_policy_h = SIZE_GROW;
            w->size_policy_v = SIZE_FIT;
            w->spacing = RANDY_LAYOUT_SPACING;
            w->pref_h = 0;
            break;
        case WIDGET_HBOX:
            w->size_policy_h = SIZE_FIT;
            w->size_policy_v = SIZE_FIT;
            w->spacing = RANDY_LAYOUT_SPACING;
            w->pref_h = 0;
            break;
        default:
            w->pref_h = RANDY_LABEL_HEIGHT;
            break;
    }
}

/* Layout and input handling are in layout.c and input.c respectively */

/* =========================================================================
 * Context
 * ========================================================================= */

RandyContext* randy_init(void) {
    RandyContext* ctx = (RandyContext*)randy_zalloc(sizeof(RandyContext));
    if (!ctx) return NULL;

    ctx->window_cap = 8;
    ctx->windows = (RandyWindow**)randy_alloc(
        ctx->window_cap * sizeof(RandyWindow*));
    if (!ctx->windows) {
        free(ctx);
        return NULL;
    }
    return ctx;
}

void randy_shutdown(RandyContext* ctx) {
    if (!ctx) return;
    while (ctx->window_count > 0) {
        randy_window_destroy(ctx->windows[0]);
    }
    free(ctx->windows);
    free(ctx);
}

const char* randy_result_string(RandyResult result) {
    switch (result) {
        case RANDY_OK:                 return "OK";
        case RANDY_ERR_NOMEM:          return "out of memory";
        case RANDY_ERR_PLATFORM:       return "platform error";
        case RANDY_ERR_RENDERER:       return "renderer error";
        case RANDY_ERR_INVALID_HANDLE: return "invalid handle";
        default:                          return "unknown error";
    }
}

/* =========================================================================
 * Windows
 * ========================================================================= */

/* Recursively free a widget tree */
static void widget_tree_free(Widget* w) {
    if (!w) return;
    /* Free children first */
    Widget* child = w->first_child;
    while (child) {
        Widget* next = child->next_sibling;
        widget_tree_free(child);
        child = next;
    }
    /* Free this widget's owned data */
    free(w->text);
    if (w->cells) {
        for (int i = 0; i < w->num_cells; i++) free(w->cells[i]);
        free(w->cells);
    }
    free(w->col_widths);
    free(w);
}

RandyWindow* randy_window_create(RandyContext* ctx,
                                          const RandyWindowDesc* desc) {
    if (!ctx || !desc) return NULL;

    RandyWindow* win = (RandyWindow*)randy_zalloc(sizeof(RandyWindow));
    if (!win) return NULL;

    win->ctx     = ctx;
    win->next_id = 1;
    win->needs_layout = true;
    win->needs_render = true;

    /* Create implicit root VBOX container */
    Widget* root = (Widget*)randy_zalloc(sizeof(Widget));
    if (!root) { free(win); return NULL; }
    root->id      = 0;  /* root has no public ID */
    root->kind    = WIDGET_VBOX;
    root->visible = true;
    root->enabled = true;
    root->spacing = RANDY_LAYOUT_SPACING;
    root->grow_weight = 1;
    root->size_policy_h = SIZE_GROW;
    root->size_policy_v = SIZE_GROW;
    win->root = root;

    win->platform = platform_window_create(desc);
    if (!win->platform) {
        free(root);
        free(win);
        return NULL;
    }

    win->renderer = renderer_create(win->platform);
    if (!win->renderer) {
        platform_window_destroy(win->platform);
        free(root);
        free(win);
        return NULL;
    }

    /* Register with context */
    if (ctx->window_count == ctx->window_cap) {
        uint32_t new_cap = ctx->window_cap * 2;
        RandyWindow** new_arr = (RandyWindow**)realloc(
            ctx->windows, new_cap * sizeof(RandyWindow*));
        if (!new_arr) {
            renderer_destroy(win->renderer);
            platform_window_destroy(win->platform);
            free(root);
            free(win);
            return NULL;
        }
        ctx->windows  = new_arr;
        ctx->window_cap = new_cap;
    }
    ctx->windows[ctx->window_count++] = win;

    return win;
}

void randy_window_destroy(RandyWindow* win) {
    if (!win) return;

    /* Unregister from context */
    RandyContext* ctx = win->ctx;
    if (ctx) {
        for (uint32_t i = 0; i < ctx->window_count; i++) {
            if (ctx->windows[i] == win) {
                ctx->windows[i] = ctx->windows[--ctx->window_count];
                break;
            }
        }
    }

    /* Free entire widget tree */
    widget_tree_free(win->root);

    renderer_destroy(win->renderer);
    platform_window_destroy(win->platform);
    free(win);
}

bool randy_window_should_close(RandyWindow* win) {
    if (!win) return true;
    return platform_window_should_close(win->platform);
}

void randy_window_set_title(RandyWindow* win, const char* title) {
    if (!win) return;
    platform_window_set_title(win->platform, title);
}

void randy_window_get_size(RandyWindow* win, int* width, int* height) {
    if (!win) return;
    platform_window_get_size(win->platform, width, height);
}

/* =========================================================================
 * Main loop helpers
 * ========================================================================= */

void randy_poll_events(RandyContext* ctx) {
    if (!ctx) return;
    platform_poll_events();
    for (uint32_t i = 0; i < ctx->window_count; i++) {
        update_window_input_and_clicks(ctx->windows[i]);
    }
}

void randy_render(RandyWindow* win) {
    if (!win) return;
    if (win->needs_layout) win->needs_render = true;
    if (!win->needs_render) return;
    layout_widgets(win);
    if (renderer_render(win->renderer, win->root)) {
        win->needs_render = false;
    } else {
        platform_wake_event_loop();
    }
}

/* =========================================================================
 * Widgets â€” internal helpers
 * ========================================================================= */

Widget* widget_alloc(RandyWindow* win, WidgetKind kind, const char* text) {
    Widget* w = (Widget*)randy_zalloc(sizeof(Widget));
    if (!w) return NULL;

    w->id      = win->next_id++;
    w->kind    = kind;
    w->text    = randy_strdup(text);
    w->visible = true;
    w->enabled = true;

    /* Set default size hints based on widget kind */
    widget_set_default_size_hints(w);

    /* Append to root container */
    widget_add_child(win->root, w);

    win->needs_layout = true;

    return w;
}

Widget* widget_find(RandyWindow* win, RandyWidgetId id) {
    if (!win || !win->root) return NULL;
    for (Widget* w = widget_next_depth_first(win->root, NULL); w;
         w = widget_next_depth_first(win->root, w)) {
        if (w->id == id) return w;
    }
    return NULL;
}

/* =========================================================================
 * Layout containers â€” public API
 * ========================================================================= */

RandyWidgetId randy_vbox_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_VBOX, NULL);
    return w ? w->id : 0;
}

RandyWidgetId randy_hbox_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_HBOX, NULL);
    return w ? w->id : 0;
}

void randy_container_set_spacing(RandyWindow* win,
                                       RandyWidgetId id,
                                       int spacing) {
    Widget* w = widget_find(win, id);
    if (w && (w->kind == WIDGET_VBOX || w->kind == WIDGET_HBOX)) {
        w->spacing = spacing;
        win->needs_layout = true;
    }
}

void randy_container_set_padding(RandyWindow* win,
                                       RandyWidgetId id,
                                       int padding) {
    Widget* w = widget_find(win, id);
    if (w && (w->kind == WIDGET_VBOX || w->kind == WIDGET_HBOX)) {
        w->padding = padding;
        win->needs_layout = true;
    }
}

/* =========================================================================
 * Widget tree management â€” public API
 * ========================================================================= */

void randy_widget_add_child(RandyWindow* win,
                                  RandyWidgetId parent_id,
                                  RandyWidgetId child_id) {
    if (!win) return;
    Widget* parent = widget_find(win, parent_id);
    Widget* child  = widget_find(win, child_id);
    if (!parent || !child) return;
    widget_add_child(parent, child);
    win->needs_layout = true;
}

/* =========================================================================
 * Widget common properties â€” public API
 * ========================================================================= */

void randy_widget_set_size_policy(RandyWindow* win,
                                        RandyWidgetId id,
                                        RandySizePolicy h,
                                        RandySizePolicy v) {
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->size_policy_h = (SizePolicy)h;
    w->size_policy_v = (SizePolicy)v;
    win->needs_layout = true;
}

void randy_widget_set_visible(RandyWindow* win,
                                    RandyWidgetId id,
                                    bool visible) {
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->visible = visible;
    win->needs_layout = true;
    win->needs_render = true;
}

bool randy_widget_get_visible(RandyWindow* win,
                                    RandyWidgetId id) {
    Widget* w = widget_find(win, id);
    return w ? w->visible : false;
}

void randy_widget_set_enabled(RandyWindow* win,
                                    RandyWidgetId id,
                                    bool enabled) {
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->enabled = enabled;
    win->needs_render = true;
}

bool randy_widget_get_enabled(RandyWindow* win,
                                    RandyWidgetId id) {
    Widget* w = widget_find(win, id);
    return w ? w->enabled : false;
}
