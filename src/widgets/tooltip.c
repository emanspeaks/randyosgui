#include "tooltip.h"

RandyWidgetId randy_tooltip_create(RandyWindow* win, const char* text) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TOOLTIP, text);
    if (!w) return 0;
    w->pref_h = 20;
    w->size_policy_h = SIZE_FIT;
    w->size_policy_v = SIZE_FIT;
    w->pref_w = 120;
    w->visible = false; /* tooltips start hidden */
    return w->id;
}

void draw_tooltip(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    /* Classic Win98 tooltip: light yellow background with black border */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.tooltip_background.r, g_style.tooltip_background.g, g_style.tooltip_background.b);
    /* Black 1px border */
    draw_rect(cmd, extent, w->x, w->y, w->w, 1,
              g_style.window_frame.r, g_style.window_frame.g, g_style.window_frame.b);
    draw_rect(cmd, extent, w->x, w->y + w->h - 1, w->w, 1,
              g_style.window_frame.r, g_style.window_frame.g, g_style.window_frame.b);
    draw_rect(cmd, extent, w->x, w->y, 1, w->h,
              g_style.window_frame.r, g_style.window_frame.g, g_style.window_frame.b);
    draw_rect(cmd, extent, w->x + w->w - 1, w->y, 1, w->h,
              g_style.window_frame.r, g_style.window_frame.g, g_style.window_frame.b);

    Widget text_area = *w;
    text_area.x = w->x + 4;
    text_area.w = w->w - 8;
    draw_widget_text(r, cmd, &text_area, extent, 2,
                     g_style.tooltip_text.r, g_style.tooltip_text.g, g_style.tooltip_text.b,
                     g_style.tooltip_background.r, g_style.tooltip_background.g, g_style.tooltip_background.b);
}
