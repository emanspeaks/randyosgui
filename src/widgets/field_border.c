#include "field_border.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_field_border_create(RandyWindow* win,
                                                 const char* text,
                                                 bool disabled_style) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_FIELD_BORDER, text);
    if (!w) return 0;
    w->readonly = disabled_style;
    return w->id;
}

void draw_field_border(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent) {
    float bgr = w->readonly ? g_style.surface.r : g_style.input_background.r;
    float bgg = w->readonly ? g_style.surface.g : g_style.input_background.g;
    float bgb = w->readonly ? g_style.surface.b : g_style.input_background.b;
    draw_widget_rect(cmd, w, extent, bgr, bgg, bgb);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    Widget text_area = *w;
    text_area.x += 4;
    text_area.y += 2;
    text_area.w -= 8;
    text_area.h -= 4;
    draw_widget_text(r, cmd, &text_area, extent, 3,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     bgr, bgg, bgb);
}
