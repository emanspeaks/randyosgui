#include "randyosgui_internal.h"

#define RANDYOSGUI_LAYOUT_PADDING_X 14
#define RANDYOSGUI_LAYOUT_PADDING_TOP 38
#define RANDYOSGUI_LAYOUT_SPACING 6
#define RANDYOSGUI_LABEL_HEIGHT 18
#define RANDYOSGUI_BUTTON_HEIGHT 23
#define RANDYOSGUI_CHECKBOX_HEIGHT 16
#define RANDYOSGUI_TEXTBOX_HEIGHT 21
#define RANDYOSGUI_DROPDOWN_HEIGHT 21
#define RANDYOSGUI_SLIDER_HEIGHT 24
#define RANDYOSGUI_PROGRESS_HEIGHT 20
#define RANDYOSGUI_GROUPBOX_HEIGHT 54
#define RANDYOSGUI_TAB_HEIGHT 22
#define RANDYOSGUI_TAB_WIDTH 96
#define RANDYOSGUI_TREE_ITEM_HEIGHT 16
#define RANDYOSGUI_TABLE_HEADER_HEIGHT 17
#define RANDYOSGUI_TABLE_ROW_HEIGHT 14
#define RANDYOSGUI_FIELD_BORDER_HEIGHT 22
#define RANDYOSGUI_STATUS_FIELD_HEIGHT 20
#define RANDYOSGUI_SUNKEN_PANEL_HEIGHT 64
#define RANDYOSGUI_LAYOUT_BOTTOM_MARGIN 12
#define RANDYOSGUI_LAYOUT_COLUMN_GAP 8

static int widget_layout_height(const Widget* w) {
    if (!w) return RANDYOSGUI_LABEL_HEIGHT;
    if (w->kind == WIDGET_BUTTON) return RANDYOSGUI_BUTTON_HEIGHT;
    if (w->kind == WIDGET_CHECKBOX) return RANDYOSGUI_CHECKBOX_HEIGHT;
    if (w->kind == WIDGET_RADIO) return RANDYOSGUI_CHECKBOX_HEIGHT;
    if (w->kind == WIDGET_TEXTBOX) return RANDYOSGUI_TEXTBOX_HEIGHT;
    if (w->kind == WIDGET_DROPDOWN) return RANDYOSGUI_DROPDOWN_HEIGHT;
    if (w->kind == WIDGET_SLIDER) return RANDYOSGUI_SLIDER_HEIGHT;
    if (w->kind == WIDGET_PROGRESS) return RANDYOSGUI_PROGRESS_HEIGHT;
    if (w->kind == WIDGET_GROUPBOX) return RANDYOSGUI_GROUPBOX_HEIGHT;
    if (w->kind == WIDGET_TAB) return RANDYOSGUI_TAB_HEIGHT;
    if (w->kind == WIDGET_TREE_ITEM) return RANDYOSGUI_TREE_ITEM_HEIGHT;
    if (w->kind == WIDGET_TABLE_HEADER) return RANDYOSGUI_TABLE_HEADER_HEIGHT;
    if (w->kind == WIDGET_TABLE_ROW) return RANDYOSGUI_TABLE_ROW_HEIGHT;
    if (w->kind == WIDGET_FIELD_BORDER) return RANDYOSGUI_FIELD_BORDER_HEIGHT;
    if (w->kind == WIDGET_STATUS_FIELD) return RANDYOSGUI_STATUS_FIELD_HEIGHT;
    if (w->kind == WIDGET_SUNKEN_PANEL) return RANDYOSGUI_SUNKEN_PANEL_HEIGHT;
    return RANDYOSGUI_LABEL_HEIGHT;
}

static int widget_layout_spacing(const Widget* w) {
    if (!w) return RANDYOSGUI_LAYOUT_SPACING;
    if (w->kind == WIDGET_TREE_ITEM) {
        return (w->next && w->next->kind == WIDGET_TREE_ITEM) ? 3 : RANDYOSGUI_LAYOUT_SPACING;
    }
    if (w->kind == WIDGET_TABLE_HEADER && w->next && w->next->kind == WIDGET_TABLE_ROW) {
        return 0;
    }
    if (w->kind == WIDGET_TABLE_ROW) {
        return (w->next && w->next->kind == WIDGET_TABLE_ROW) ? 0 : RANDYOSGUI_LAYOUT_SPACING;
    }
    if (w->kind == WIDGET_STATUS_FIELD) {
        return (w->next && w->next->kind == WIDGET_STATUS_FIELD) ? 1 : RANDYOSGUI_LAYOUT_SPACING;
    }
    return RANDYOSGUI_LAYOUT_SPACING;
}

static bool widget_is_clickable(const Widget* w) {
    return w && (w->kind == WIDGET_BUTTON ||
                 w->kind == WIDGET_CHECKBOX ||
                 w->kind == WIDGET_RADIO ||
                 w->kind == WIDGET_DROPDOWN ||
                 w->kind == WIDGET_SLIDER ||
                 w->kind == WIDGET_TAB ||
                 w->kind == WIDGET_TABLE_ROW);
}

static bool point_in_widget(const Widget* w, double x, double y) {
    return x >= (double)w->x &&
           y >= (double)w->y &&
           x < (double)(w->x + w->w) &&
           y < (double)(w->y + w->h);
}


static void slider_update_from_cursor(Widget* w, double mouse_x) {
    if (!w || w->kind != WIDGET_SLIDER) return;
    if (w->max_value <= w->min_value) return;

    int track_x0 = w->x + 12;
    int track_x1 = w->x + w->w - 12;
    int track_w = track_x1 - track_x0;
    if (track_w < 1) return;

    int px = (int)mouse_x;
    if (px < track_x0) px = track_x0;
    if (px > track_x1) px = track_x1;

    int range = w->max_value - w->min_value;
    int next = w->min_value + ((px - track_x0) * range) / track_w;
    next = clamp_int(next, w->min_value, w->max_value);
    if (next != w->value) {
        w->value = next;
        if (w->value_cb) {
            w->value_cb(w->id, w->value, w->value_userdata);
        }
    }
}

static void layout_widgets(RandyosgWindow* win) {
    if (!win || !win->needs_layout) return;

    int fb_w = 0, fb_h = 0;
    platform_window_get_size(win->platform, &fb_w, &fb_h);
    if (fb_w <= 0 || fb_h <= 0) return;

    int y = RANDYOSGUI_LAYOUT_PADDING_TOP;
    int max_y = fb_h - RANDYOSGUI_LAYOUT_BOTTOM_MARGIN;
    int content_w = fb_w - (RANDYOSGUI_LAYOUT_PADDING_X * 2);
    if (content_w < 64) content_w = 64;

    int col_count = 1;
    if (content_w >= 420) {
        int estimate_y = RANDYOSGUI_LAYOUT_PADDING_TOP;
        int estimate_tab_h = RANDYOSGUI_TAB_HEIGHT + 2;
        bool in_tab_row_est = false;
        for (Widget* w = win->widgets; w; w = w->next) {
            int h = widget_layout_height(w);
            if (w->kind == WIDGET_TAB) {
                if (!in_tab_row_est) {
                    estimate_y += estimate_tab_h;
                    in_tab_row_est = true;
                }
                if (!w->next || w->next->kind != WIDGET_TAB) in_tab_row_est = false;
                continue;
            }
            estimate_y += h + widget_layout_spacing(w);
        }
        if (estimate_y > max_y) col_count = 2;
    }

    int col_w = content_w;
    if (col_count == 2) {
        col_w = (content_w - RANDYOSGUI_LAYOUT_COLUMN_GAP) / 2;
    }

    int col_index = 0;
    int col_x = RANDYOSGUI_LAYOUT_PADDING_X;

    int tab_x = col_x;
    bool in_tab_row = false;

    for (Widget* w = win->widgets; w; w = w->next) {
        if (w->kind == WIDGET_TAB) {
            int tab_w = RANDYOSGUI_TAB_WIDTH;
            int min_w = 68;
            int approx_w = 0;
            if (w->text) {
                approx_w = 16 + ((int)strlen(w->text) * 7);
            }
            if (approx_w > tab_w) tab_w = approx_w;
            if (tab_w < min_w) tab_w = min_w;

            if (!in_tab_row) {
                tab_x = col_x;
                in_tab_row = true;
            }

            if (tab_x + tab_w > (col_x + col_w)) {
                y += RANDYOSGUI_TAB_HEIGHT + 2;
                tab_x = col_x;
            }

            if (y + RANDYOSGUI_TAB_HEIGHT > max_y && col_index + 1 < col_count) {
                col_index++;
                col_x = RANDYOSGUI_LAYOUT_PADDING_X + (col_index * (col_w + RANDYOSGUI_LAYOUT_COLUMN_GAP));
                y = RANDYOSGUI_LAYOUT_PADDING_TOP;
                tab_x = col_x;
            }

            w->x = tab_x;
            w->y = y;
            w->w = tab_w;
            w->h = RANDYOSGUI_TAB_HEIGHT;
            tab_x += tab_w - 3;

            if (!w->next || w->next->kind != WIDGET_TAB) {
                y += RANDYOSGUI_TAB_HEIGHT + 2;
                in_tab_row = false;
            }
            continue;
        }

        if (in_tab_row) {
            y += RANDYOSGUI_TAB_HEIGHT + 2;
            in_tab_row = false;
        }

        int h = widget_layout_height(w);
        if (y + h > max_y && col_index + 1 < col_count) {
            col_index++;
            col_x = RANDYOSGUI_LAYOUT_PADDING_X + (col_index * (col_w + RANDYOSGUI_LAYOUT_COLUMN_GAP));
            y = RANDYOSGUI_LAYOUT_PADDING_TOP;
        }
        w->x = col_x;
        w->y = y;
        w->w = col_w;
        w->h = h;
        y += h + widget_layout_spacing(w);
    }

    win->needs_layout = false;
}

static void update_window_input_and_clicks(RandyosgWindow* win) {
    if (!win) return;

    int fb_w = 0;
    int fb_h = 0;
    platform_window_get_size(win->platform, &fb_w, &fb_h);
    if (fb_w != win->layout_fb_w || fb_h != win->layout_fb_h) {
        win->layout_fb_w = fb_w;
        win->layout_fb_h = fb_h;
        win->needs_layout = true;
    }

    layout_widgets(win);

    win->prev_mouse_down = win->mouse_down;
    platform_window_get_cursor_pos(win->platform, &win->mouse_x, &win->mouse_y);
    win->mouse_down = platform_window_is_mouse_down(win->platform, 0);

    if (win->mouse_down != win->prev_mouse_down) {
        win->needs_render = true;
    }

    RandyosgWidgetId old_hot_id = win->hot_id;
    win->hot_id = 0;
    for (Widget* w = win->widgets; w; w = w->next) {
        w->hovered = false;
        w->pressed = false;
        if (!widget_is_clickable(w)) continue;
        if (point_in_widget(w, win->mouse_x, win->mouse_y)) {
            win->hot_id = w->id;
            w->hovered = true;
            break;
        }
    }
    if (win->hot_id != old_hot_id) {
        win->needs_render = true;
    }

    if (!win->prev_mouse_down && win->mouse_down) {
        win->active_id = win->hot_id;
    }

    if (win->active_id != 0) {
        Widget* active = widget_find(win, win->active_id);
        if (widget_is_clickable(active)) {
            active->pressed = win->mouse_down;
            if (active->kind == WIDGET_SLIDER && win->mouse_down) {
                int old_value = active->value;
                slider_update_from_cursor(active, win->mouse_x);
                if (active->value != old_value) win->needs_render = true;
            }
            if (active->kind == WIDGET_TABLE_ROW && win->mouse_down) {
                for (Widget* row = win->widgets; row; row = row->next) {
                    if (row->kind == WIDGET_TABLE_ROW) row->checked = false;
                }
                if (!active->checked) win->needs_render = true;
                active->checked = true;
            }
        }
    }

    if (win->prev_mouse_down && !win->mouse_down) {
        if (win->active_id != 0 && win->active_id == win->hot_id) {
            Widget* w = widget_find(win, win->active_id);
            if (w && w->kind == WIDGET_BUTTON && w->click_cb) {
                w->click_cb(w->id, w->click_userdata);
            } else if (w && w->kind == WIDGET_CHECKBOX) {
                w->checked = !w->checked;
                win->needs_render = true;
                if (w->toggle_cb) {
                    w->toggle_cb(w->id, w->checked, w->toggle_userdata);
                }
            } else if (w && w->kind == WIDGET_RADIO) {
                for (Widget* r = win->widgets; r; r = r->next) {
                    if (r->kind == WIDGET_RADIO) r->checked = false;
                }
                w->checked = true;
                win->needs_render = true;
                if (w->toggle_cb) {
                    w->toggle_cb(w->id, w->checked, w->toggle_userdata);
                }
            } else if (w && w->kind == WIDGET_DROPDOWN && w->click_cb) {
                w->click_cb(w->id, w->click_userdata);
            } else if (w && w->kind == WIDGET_TAB) {
                for (Widget* t = win->widgets; t; t = t->next) {
                    if (t->kind == WIDGET_TAB) t->checked = false;
                }
                w->checked = true;
                win->needs_render = true;
                if (w->click_cb) {
                    w->click_cb(w->id, w->click_userdata);
                }
            } else if (w && w->kind == WIDGET_TABLE_ROW) {
                if (w->click_cb) {
                    w->click_cb(w->id, w->click_userdata);
                }
            }
        }
        win->active_id = 0;
    }
}

/* =========================================================================
 * Context
 * ========================================================================= */

RandyosgContext* randyosgui_init(void) {
    RandyosgContext* ctx = (RandyosgContext*)randyosgui_zalloc(sizeof(RandyosgContext));
    if (!ctx) return NULL;

    ctx->window_cap = 8;
    ctx->windows = (RandyosgWindow**)randyosgui_alloc(
        ctx->window_cap * sizeof(RandyosgWindow*));
    if (!ctx->windows) {
        free(ctx);
        return NULL;
    }
    return ctx;
}

void randyosgui_shutdown(RandyosgContext* ctx) {
    if (!ctx) return;
    /* window_destroy unregisters itself, so always destroy index 0 */
    while (ctx->window_count > 0) {
        randyosgui_window_destroy(ctx->windows[0]);
    }
    free(ctx->windows);
    free(ctx);
}

const char* randyosgui_result_string(RandyosgResult result) {
    switch (result) {
        case RANDYOSG_OK:                 return "OK";
        case RANDYOSG_ERR_NOMEM:          return "out of memory";
        case RANDYOSG_ERR_PLATFORM:       return "platform error";
        case RANDYOSG_ERR_RENDERER:       return "renderer error";
        case RANDYOSG_ERR_INVALID_HANDLE: return "invalid handle";
        default:                          return "unknown error";
    }
}

/* =========================================================================
 * Windows
 * ========================================================================= */

RandyosgWindow* randyosgui_window_create(RandyosgContext* ctx,
                                          const RandyosgWindowDesc* desc) {
    if (!ctx || !desc) return NULL;

    RandyosgWindow* win = (RandyosgWindow*)randyosgui_zalloc(sizeof(RandyosgWindow));
    if (!win) return NULL;

    win->ctx     = ctx;
    win->next_id = 1;
    win->needs_layout = true;
    win->needs_render = true;

    win->platform = platform_window_create(desc);
    if (!win->platform) {
        free(win);
        return NULL;
    }

    win->renderer = renderer_create(win->platform);
    if (!win->renderer) {
        platform_window_destroy(win->platform);
        free(win);
        return NULL;
    }

    /* Register with context, growing array if needed */
    if (ctx->window_count == ctx->window_cap) {
        uint32_t new_cap = ctx->window_cap * 2;
        RandyosgWindow** new_arr = (RandyosgWindow**)realloc(
            ctx->windows, new_cap * sizeof(RandyosgWindow*));
        if (!new_arr) {
            renderer_destroy(win->renderer);
            platform_window_destroy(win->platform);
            free(win);
            return NULL;
        }
        ctx->windows  = new_arr;
        ctx->window_cap = new_cap;
    }
    ctx->windows[ctx->window_count++] = win;

    return win;
}

void randyosgui_window_destroy(RandyosgWindow* win) {
    if (!win) return;

    /* Unregister from context so shutdown doesn't double-free */
    RandyosgContext* ctx = win->ctx;
    if (ctx) {
        for (uint32_t i = 0; i < ctx->window_count; i++) {
            if (ctx->windows[i] == win) {
                ctx->windows[i] = ctx->windows[--ctx->window_count];
                break;
            }
        }
    }

    Widget* w = win->widgets;
    while (w) {
        Widget* next = w->next;
        free(w->text);
        if (w->cells) {
            for (int i = 0; i < w->num_cells; i++) free(w->cells[i]);
            free(w->cells);
        }
        free(w->col_widths);
        free(w);
        w = next;
    }

    renderer_destroy(win->renderer);
    platform_window_destroy(win->platform);
    free(win);
}

bool randyosgui_window_should_close(RandyosgWindow* win) {
    if (!win) return true;
    return platform_window_should_close(win->platform);
}

void randyosgui_window_set_title(RandyosgWindow* win, const char* title) {
    if (!win) return;
    platform_window_set_title(win->platform, title);
}

void randyosgui_window_get_size(RandyosgWindow* win, int* width, int* height) {
    if (!win) return;
    platform_window_get_size(win->platform, width, height);
}

/* =========================================================================
 * Main loop helpers
 * ========================================================================= */

void randyosgui_poll_events(RandyosgContext* ctx) {
    if (!ctx) return;
    platform_poll_events();
    for (uint32_t i = 0; i < ctx->window_count; i++) {
        update_window_input_and_clicks(ctx->windows[i]);
    }
}

void randyosgui_render(RandyosgWindow* win) {
    if (!win) return;
    if (win->needs_layout) win->needs_render = true;
    if (!win->needs_render) return;
    layout_widgets(win);
    if (renderer_render(win->renderer, win->widgets)) {
        win->needs_render = false;
    } else {
        /* Swapchain recreated or skipped — retry on next iteration */
        platform_wake_event_loop();
    }
}

/* =========================================================================
 * Widgets — internal helpers
 * ========================================================================= */

Widget* widget_alloc(RandyosgWindow* win, WidgetKind kind, const char* text) {
    Widget* w = (Widget*)randyosgui_zalloc(sizeof(Widget));
    if (!w) return NULL;

    w->id   = win->next_id++;
    w->kind = kind;
    w->text = randyosgui_strdup(text);

    /* Append to preserve creation order in layout/input traversal */
    if (!win->widgets) {
        win->widgets = w;
    } else {
        Widget* tail = win->widgets;
        while (tail->next) tail = tail->next;
        tail->next = w;
    }

    win->needs_layout = true;

    return w;
}

Widget* widget_find(RandyosgWindow* win, RandyosgWidgetId id) {
    for (Widget* w = win->widgets; w; w = w->next) {
        if (w->id == id) return w;
    }
    return NULL;
}
