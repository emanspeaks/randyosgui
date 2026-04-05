#include "accordion.h"

#define ACCORDION_HEADER_H 22

RandyWidgetId randy_accordion_create(RandyWindow* win,
                                              const char* label,
                                              bool expanded) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_ACCORDION, label);
    if (!w) return 0;
    w->checked = expanded;
    w->pref_h = expanded ? 80 : ACCORDION_HEADER_H;
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = expanded ? SIZE_FIT : SIZE_FIXED;
    return w->id;
}

void randy_accordion_set_expanded(RandyWindow* win,
                                        RandyWidgetId id,
                                        bool expanded) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_ACCORDION) return;
    w->checked = expanded;

    /* Show/hide children */
    for (Widget* c = w->first_child; c; c = c->next_sibling) {
        c->visible = expanded;
    }

    w->size_policy_v = expanded ? SIZE_FIT : SIZE_FIXED;
    w->pref_h = expanded ? 80 : ACCORDION_HEADER_H;
    win->needs_layout = true;
    win->needs_render = true;
}

bool randy_accordion_get_expanded(RandyWindow* win,
                                        RandyWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    return (w && w->kind == WIDGET_ACCORDION) ? w->checked : false;
}

void draw_accordion(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent) {
    /* Header bar */
    draw_rect(cmd, extent, w->x, w->y, w->w, ACCORDION_HEADER_H,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, ACCORDION_HEADER_H, false);

    /* Expand/collapse indicator */
    int ix = w->x + 6;
    int iy = w->y + 7;
    draw_rect(cmd, extent, ix, iy, 9, 9,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, ix, iy, 9, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, ix + 8, iy, 1, 9,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, ix, iy + 8, 9, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, ix, iy, 1, 9,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    /* Horizontal line (minus) */
    draw_rect(cmd, extent, ix + 2, iy + 4, 5, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    /* Vertical line (plus, only when collapsed) */
    if (!w->checked) {
        draw_rect(cmd, extent, ix + 4, iy + 2, 1, 5,
                  g_style.text.r, g_style.text.g, g_style.text.b);
    }

    /* Header text */
    Widget label_area = *w;
    label_area.x = w->x + 20;
    label_area.w = w->w - 24;
    label_area.h = ACCORDION_HEADER_H;
    draw_widget_text(r, cmd, &label_area, extent, 4,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);

    /* Content area border when expanded */
    if (w->checked && w->h > ACCORDION_HEADER_H) {
        int cy = w->y + ACCORDION_HEADER_H;
        int ch = w->h - ACCORDION_HEADER_H;
        draw_rect(cmd, extent, w->x, cy, 1, ch,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
        draw_rect(cmd, extent, w->x + w->w - 1, cy, 1, ch,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
        draw_rect(cmd, extent, w->x, cy + ch - 1, w->w, 1,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    }
}
