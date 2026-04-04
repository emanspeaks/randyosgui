#include "dropdown.h"

RandyosgWidgetId randyosgui_dropdown_create(RandyosgWindow* win,
                                             const char* value) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_DROPDOWN, value);
    return w ? w->id : 0;
}

void randyosgui_dropdown_set_value(RandyosgWindow* win,
                                    RandyosgWidgetId id,
                                    const char* value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_DROPDOWN) return;
    free(w->text);
    w->text = randyosgui_strdup(value);
}

void draw_dropdown(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    draw_widget_rect(cmd, w, extent,
                     WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
    draw_sunken_panel_border_98(cmd, extent, w->x, w->y, w->w, w->h);

    int bx = w->x + w->w - 18;
    int by = w->y + 2;
    if (w->h >= 19) {
        draw_select_button_98(cmd, extent, bx, by);
    }

    Widget text_area = *w;
    text_area.x = w->x + 4;
    text_area.w = w->w - 26;
    if (text_area.w < 1) text_area.w = 1;
    draw_widget_text(r, cmd, &text_area, extent, 3,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
}
