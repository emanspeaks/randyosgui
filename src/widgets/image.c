#include "image.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

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
    /* Qt Fusion: white interior with 1px border */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);

    /* Crosshatch placeholder pattern */
    for (int dy = 4; dy < w->h - 4; dy += 8) {
        draw_rect(cmd, extent, w->x + 2, w->y + dy, w->w - 4, 1,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    }
}
