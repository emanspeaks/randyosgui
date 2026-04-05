#include "toolbar.h"

RandyWidgetId randy_toolbar_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TOOLBAR, NULL);
    if (!w) return 0;
    w->pref_h = 26;
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_FIXED;
    w->spacing = 2;
    return w->id;
}

void draw_toolbar(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    (void)r;
    /* Toolbar background with subtle bottom edge */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    /* Top highlight */
    draw_rect(cmd, extent, w->x, w->y, w->w, 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    /* Bottom shadow */
    draw_rect(cmd, extent, w->x, w->y + w->h - 1, w->w, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    /* Left grip (classic Win98 toolbar handle) */
    draw_rect(cmd, extent, w->x + 2, w->y + 2, 1, w->h - 4,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, w->x + 3, w->y + 2, 1, w->h - 4,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x + 5, w->y + 2, 1, w->h - 4,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, w->x + 6, w->y + 2, 1, w->h - 4,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
}
