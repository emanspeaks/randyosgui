#include "button.h"

RandyWidgetId randy_button_create(RandyWindow* win, const char* label) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_BUTTON, label);
    return w ? w->id : 0;
}

void randy_button_set_callback(RandyWindow* win, RandyWidgetId id,
                                     RandyClickCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->click_cb       = cb;
    w->click_userdata = userdata;
}

void draw_button(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent) {
    if (w->pressed) {
        draw_widget_rect(cmd, w, extent, g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
        draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
        draw_widget_text(r, cmd, w, extent, 4,
                         g_style.text.r, g_style.text.g, g_style.text.b,
                         g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    } else if (w->hovered) {
        draw_widget_rect(cmd, w, extent, g_style.button_hover.r, g_style.button_hover.g, g_style.button_hover.b);
        draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, false);
        draw_widget_text(r, cmd, w, extent, 4,
                         g_style.text.r, g_style.text.g, g_style.text.b,
                         g_style.button_hover.r, g_style.button_hover.g, g_style.button_hover.b);
    } else {
        draw_widget_rect(cmd, w, extent, g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
        draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, false);
        draw_widget_text(r, cmd, w, extent, 4,
                         g_style.text.r, g_style.text.g, g_style.text.b,
                         g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    }
}
