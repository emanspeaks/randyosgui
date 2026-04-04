#include "sunken_panel.h"

RandyosgWidgetId randyosgui_sunken_panel_create(RandyosgWindow* win,
                                                 const char* text) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_SUNKEN_PANEL, text);
    return w ? w->id : 0;
}

void draw_sunken_panel(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent) {
    draw_widget_rect(cmd, w, extent,
                     WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
    draw_sunken_panel_border_98(cmd, extent, w->x, w->y, w->w, w->h);
    Widget text_area = *w;
    text_area.x += 4;
    text_area.y += 4;
    text_area.w -= 8;
    text_area.h -= 8;
    draw_widget_text(r, cmd, &text_area, extent, 3,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
}
