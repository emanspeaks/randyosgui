#include "separator.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_separator_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_SEPARATOR, NULL);
    if (!w) return 0;
    w->pref_h = 2;
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_FIXED;
    return w->id;
}

void draw_separator(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent) {
    (void)r;
    int mid_y = w->y + w->h / 2;
    /* Etched line: shadow + highlight */
    draw_rect(cmd, extent, w->x, mid_y, w->w, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x, mid_y + 1, w->w, 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
}
