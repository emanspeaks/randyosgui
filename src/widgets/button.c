#include "button.h"

RandyosgWidgetId randyosgui_button_create(RandyosgWindow* win, const char* label) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_BUTTON, label);
    return w ? w->id : 0;
}

void randyosgui_button_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                     RandyosgClickCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->click_cb       = cb;
    w->click_userdata = userdata;
}

void draw_button(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent) {
    if (w->pressed) {
        draw_widget_rect(cmd, w, extent, WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
        draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
        draw_widget_text(r, cmd, w, extent, 4,
                         WIN98.text_r, WIN98.text_g, WIN98.text_b,
                         WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
    } else if (w->hovered) {
        draw_widget_rect(cmd, w, extent, 0.94f, 0.94f, 0.94f);
        draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, false);
        draw_widget_text(r, cmd, w, extent, 4,
                         WIN98.text_r, WIN98.text_g, WIN98.text_b,
                         0.94f, 0.94f, 0.94f);
    } else {
        draw_widget_rect(cmd, w, extent, WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
        draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, false);
        draw_widget_text(r, cmd, w, extent, 4,
                         WIN98.text_r, WIN98.text_g, WIN98.text_b,
                         WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
    }
}
