#include "scroll_area.h"

RandyWidgetId randy_scroll_area_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_SCROLL_AREA, NULL);
    if (!w) return 0;
    w->pref_h = 100;
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_GROW;
    return w->id;
}

void draw_scroll_area(RendererContext* r, VkCommandBuffer cmd,
                      const Widget* w, VkExtent2D extent) {
    (void)r;
    /* Sunken border like a standard Win98 sunken panel */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    /* Sunken edges */
    draw_rect(cmd, extent, w->x, w->y, w->w, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x, w->y, 1, w->h,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x + w->w - 1, w->y, 1, w->h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_rect(cmd, extent, w->x, w->y + w->h - 1, w->w, 1,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);

    /* Scrollbar track on right side */
    int sb_w = 16;
    int sb_x = w->x + w->w - sb_w - 1;
    int sb_y = w->y + 1;
    int sb_h = w->h - 2;
    draw_rect(cmd, extent, sb_x, sb_y, sb_w, sb_h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);

    /* Scrollbar up/down buttons */
    draw_rect(cmd, extent, sb_x, sb_y, sb_w, sb_w,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, sb_x, sb_y, sb_w, sb_w, false);
    draw_rect(cmd, extent, sb_x, sb_y + sb_h - sb_w, sb_w, sb_w,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, sb_x, sb_y + sb_h - sb_w, sb_w, sb_w, false);

    /* Thumb in middle area */
    int thumb_h = 30;
    int thumb_y = sb_y + sb_w + 2;
    draw_rect(cmd, extent, sb_x, thumb_y, sb_w, thumb_h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, sb_x, thumb_y, sb_w, thumb_h, false);
}
