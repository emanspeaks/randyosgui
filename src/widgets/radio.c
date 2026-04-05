#include "radio.h"

RandyWidgetId randy_radio_create(RandyWindow* win,
                                          const char* label,
                                          bool selected) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_RADIO, label);
    if (!w) return 0;
    w->checked = selected;
    return w->id;
}

void randy_radio_set_selected(RandyWindow* win,
                                    RandyWidgetId id,
                                    bool selected) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_RADIO) return;
    w->checked = selected;
}

bool randy_radio_get_selected(RandyWindow* win,
                                    RandyWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_RADIO) return false;
    return w->checked;
}

void randy_radio_set_callback(RandyWindow* win,
                                    RandyWidgetId id,
                                    RandyToggleCallback cb,
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
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
