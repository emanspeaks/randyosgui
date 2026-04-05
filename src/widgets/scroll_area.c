#include "scroll_area.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

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
    /* Qt Fusion: white bg with 1px border */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);

    /* Scrollbar track on right side */
    int sb_w = 14;
    int sb_x = w->x + w->w - sb_w - 1;
    int sb_y = w->y + 1;
    int sb_h = w->h - 2;
    draw_rect(cmd, extent, sb_x, sb_y, sb_w, sb_h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_rect(cmd, extent, sb_x, sb_y, 1, sb_h,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);

    /* Scrollbar up/down buttons: raised bevel */
    draw_rect(cmd, extent, sb_x + 1, sb_y, sb_w - 1, sb_w,
              g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    draw_bevel(cmd, extent, sb_x + 1, sb_y, sb_w - 1, sb_w, false);
    draw_rect(cmd, extent, sb_x + 1, sb_y + sb_h - sb_w, sb_w - 1, sb_w,
              g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    draw_bevel(cmd, extent, sb_x + 1, sb_y + sb_h - sb_w, sb_w - 1, sb_w, false);

    /* Thumb in middle area */
    int thumb_h = 30;
    int thumb_y = sb_y + sb_w + 2;
    draw_rect(cmd, extent, sb_x + 1, thumb_y, sb_w - 1, thumb_h,
              g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    draw_bevel(cmd, extent, sb_x + 1, thumb_y, sb_w - 1, thumb_h, false);
}
