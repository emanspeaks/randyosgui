#include "randyosgui_internal.h"

/* =========================================================================
 * Input / interaction helpers
 * ========================================================================= */

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

/* =========================================================================
 * Input handling (tree-based traversal)
 * ========================================================================= */

void update_window_input_and_clicks(RandyWindow* win) {
    if (!win) return;

    int fb_w = 0;
    int fb_h = 0;
    platform_window_get_size(win->platform, &fb_w, &fb_h);
    if (fb_w != win->layout_fb_w || fb_h != win->layout_fb_h) {
        win->layout_fb_w = fb_w;
        win->layout_fb_h = fb_h;
        win->needs_layout = true;
        win->needs_render = true;
    }

    layout_widgets(win);

    win->prev_mouse_down = win->mouse_down;
    platform_window_get_cursor_pos(win->platform, &win->mouse_x, &win->mouse_y);
    win->mouse_down = platform_window_is_mouse_down(win->platform, 0);

    if (win->mouse_down != win->prev_mouse_down) {
        win->needs_render = true;
    }

    /* Find hot widget via depth-first traversal */
    RandyWidgetId old_hot_id = win->hot_id;
    win->hot_id = 0;
    for (Widget* w = widget_next_depth_first(win->root, NULL); w;
         w = widget_next_depth_first(win->root, w)) {
        w->hovered = false;
        w->pressed = false;
        if (!w->visible || !w->enabled) continue;
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
                /* Deselect all table rows (flat scan of root's children) */
                for (Widget* row = widget_next_depth_first(win->root, NULL); row;
                     row = widget_next_depth_first(win->root, row)) {
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
                /* Deselect all radios at same level */
                Widget* parent_w = w->parent;
                if (parent_w) {
                    for (Widget* r = parent_w->first_child; r; r = r->next_sibling) {
                        if (r->kind == WIDGET_RADIO) r->checked = false;
                    }
                }
                w->checked = true;
                win->needs_render = true;
                if (w->toggle_cb) {
                    w->toggle_cb(w->id, w->checked, w->toggle_userdata);
                }
            } else if (w && w->kind == WIDGET_DROPDOWN && w->click_cb) {
                w->click_cb(w->id, w->click_userdata);
            } else if (w && w->kind == WIDGET_TAB) {
                /* Deselect all tabs at same level */
                Widget* parent_w = w->parent;
                if (parent_w) {
                    for (Widget* t = parent_w->first_child; t; t = t->next_sibling) {
                        if (t->kind == WIDGET_TAB) t->checked = false;
                    }
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
