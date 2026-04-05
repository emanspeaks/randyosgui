#include "status_field.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_status_field_create(RandyWindow* win,
                                                 const char* text) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_STATUS_FIELD, text);
    return w ? w->id : 0;
}

void draw_status_field(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent) {
    /* Sunken bevel */
    draw_widget_rect(cmd, w, extent,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    Widget text_area = *w;
    text_area.x += 4;
    text_area.y += 1;
    text_area.w -= 8;
    text_area.h -= 1;
    draw_widget_text(r, cmd, &text_area, extent, 3,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
