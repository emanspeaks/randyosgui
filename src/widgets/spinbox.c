#include "spinbox.h"
#include "../renderer/renderer_private.h"
#include "../style.h"
#include <stdio.h>

RandyWidgetId randy_spinbox_create(RandyWindow* win,
                                            int min_val, int max_val,
                                            int initial) {
    if (!win) return 0;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", initial);
    Widget* w = widget_alloc(win, WIDGET_SPINBOX, buf);
    if (!w) return 0;
    w->min_value = min_val;
    w->max_value = max_val;
    w->value = clamp_int(initial, min_val, max_val);
    w->pref_h = 21;
    return w->id;
}

void randy_spinbox_set_value(RandyWindow* win, RandyWidgetId id, int value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_SPINBOX) return;
    w->value = clamp_int(value, w->min_value, w->max_value);
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", w->value);
    free(w->text);
    w->text = randy_strdup(buf);
    win->needs_render = true;
}

int randy_spinbox_get_value(RandyWindow* win, RandyWidgetId id) {
    if (!win) return 0;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_SPINBOX) return 0;
    return w->value;
}

void randy_spinbox_set_callback(RandyWindow* win, RandyWidgetId id,
                                      RandyValueCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->value_cb = cb;
    w->value_userdata = userdata;
}

void draw_spinbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    int btn_w = 16;
    int text_w = w->w - btn_w;
    if (text_w < 8) text_w = 8;

    /* Text field area — sunken bevel */
    draw_rect(cmd, extent, w->x, w->y, text_w, w->h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_bevel(cmd, extent, w->x, w->y, text_w, w->h, true);

    /* Draw the value text */
    Widget text_area = *w;
    text_area.w = text_w - 4;
    text_area.x = w->x + 2;
    draw_widget_text(r, cmd, &text_area, extent, 2,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);

    /* Up/down buttons area */
    int bx = w->x + text_w;
    int half_h = w->h / 2;

    /* Up button */
    draw_rect(cmd, extent, bx, w->y, btn_w, half_h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, bx, w->y, btn_w, half_h, false);
    /* Up arrow (small triangle) */
    int ax = bx + btn_w / 2;
    int ay = w->y + half_h / 2 - 1;
    draw_rect(cmd, extent, ax, ay, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax - 1, ay + 1, 3, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax - 2, ay + 2, 5, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);

    /* Down button */
    int by = w->y + half_h;
    int bh = w->h - half_h;
    draw_rect(cmd, extent, bx, by, btn_w, bh,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, bx, by, btn_w, bh, false);
    /* Down arrow */
    int dy = by + bh / 2 - 1;
    draw_rect(cmd, extent, ax - 2, dy, 5, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax - 1, dy + 1, 3, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax, dy + 2, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
}
