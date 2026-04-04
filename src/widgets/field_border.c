#include "field_border.h"

RandyosgWidgetId randyosgui_field_border_create(RandyosgWindow* win,
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
    float bg = w->readonly ? WIN98.surface_r : WIN98.button_highlight_r;
    float gg = w->readonly ? WIN98.surface_g : WIN98.button_highlight_g;
    float bb = w->readonly ? WIN98.surface_b : WIN98.button_highlight_b;
    draw_widget_rect(cmd, w, extent, bg, gg, bb);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    Widget text_area = *w;
    text_area.x += 2;
    text_area.y += 2;
    text_area.w -= 4;
    text_area.h -= 4;
    draw_widget_text(r, cmd, &text_area, extent, 3,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     bg, gg, bb);
}
