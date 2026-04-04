#include "radio.h"

RandyosgWidgetId randyosgui_radio_create(RandyosgWindow* win,
                                          const char* label,
                                          bool selected) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_RADIO, label);
    if (!w) return 0;
    w->checked = selected;
    return w->id;
}

void randyosgui_radio_set_selected(RandyosgWindow* win,
                                    RandyosgWidgetId id,
                                    bool selected) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_RADIO) return;
    w->checked = selected;
}

bool randyosgui_radio_get_selected(RandyosgWindow* win,
                                    RandyosgWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_RADIO) return false;
    return w->checked;
}

void randyosgui_radio_set_callback(RandyosgWindow* win,
                                    RandyosgWidgetId id,
                                    RandyosgToggleCallback cb,
                                    void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_RADIO) return;
    w->toggle_cb = cb;
    w->toggle_userdata = userdata;
}

void draw_radio(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent) {
    int dot = 12;
    int rd_x = w->x;
    int rd_y = w->y + ((w->h - dot) / 2);

    draw_radio_border_98(cmd, extent, rd_x, rd_y);

    if (w->checked) {
        draw_radio_dot_98(cmd, extent, rd_x + 4, rd_y + 4);
    }

    Widget text_area = *w;
    text_area.x = w->x + 19;
    text_area.w = w->w - 19;
    draw_widget_text(r, cmd, &text_area, extent, 4,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
}
