#include "combobox.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_combobox_create(RandyWindow* win,
                                             const char* selected_text) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_COMBOBOX, selected_text);
    if (!w) return 0;
    w->pref_h = 21;
    return w->id;
}

void randy_combobox_set_callback(RandyWindow* win, RandyWidgetId id,
                                       RandyClickCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->click_cb = cb;
    w->click_userdata = userdata;
}

void draw_combobox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    /* Input field with sunken text area + raised dropdown button */
    int btn_w = 17;
    int text_w = w->w - btn_w;
    if (text_w < 8) text_w = 8;

    /* Sunken text field area */
    draw_rect(cmd, extent, w->x, w->y, text_w, w->h,
              g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, w->x, w->y, text_w, w->h, true);

    /* Text */
    Widget text_area = *w;
    text_area.w = text_w - 4;
    text_area.x = w->x + 4;
    draw_widget_text(r, cmd, &text_area, extent, 2,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);

    /* Raised dropdown button */
    int bx = w->x + text_w;
    draw_rect(cmd, extent, bx, w->y, btn_w, w->h,
              g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    draw_bevel(cmd, extent, bx, w->y, btn_w, w->h, false);

    /* Down arrow */
    int ax = bx + btn_w / 2;
    int ay = w->y + w->h / 2 - 1;
    draw_rect(cmd, extent, ax - 2, ay, 5, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax - 1, ay + 1, 3, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax, ay + 2, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
}
