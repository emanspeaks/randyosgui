#include "image.h"

RandyWidgetId randy_image_create(RandyWindow* win, int width, int height) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_IMAGE, NULL);
    if (!w) return 0;
    w->pref_w = width > 0 ? width : 64;
    w->pref_h = height > 0 ? height : 64;
    w->size_policy_h = SIZE_FIXED;
    w->size_policy_v = SIZE_FIXED;
    return w->id;
}

void draw_image(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent) {
    (void)r;
    /* Placeholder: sunken panel with white interior */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    /* Sunken border */
    draw_rect(cmd, extent, w->x, w->y, w->w, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x, w->y, 1, w->h,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x + w->w - 1, w->y, 1, w->h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, w->x, w->y + w->h - 1, w->w, 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);

    /* Crosshatch placeholder pattern */
    for (int dy = 4; dy < w->h - 4; dy += 8) {
        draw_rect(cmd, extent, w->x + 2, w->y + dy, w->w - 4, 1,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    }
}
