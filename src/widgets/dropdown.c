#include "dropdown.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_dropdown_create(RandyWindow* win,
                                             const char* value) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_DROPDOWN, value);
    return w ? w->id : 0;
}

void randy_dropdown_set_value(RandyWindow* win,
                                    RandyWidgetId id,
                                    const char* value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_DROPDOWN) return;
    free(w->text);
    w->text = randy_strdup(value);
}

void draw_dropdown(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    /* Sunken bevel: white bg with directional 3D border */
    draw_widget_rect(cmd, w, extent,
                     g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);

    /* Drop-down arrow area */
    int btn_w = 18;
    int bx = w->x + w->w - btn_w;
    draw_rect(cmd, extent, bx, w->y + 1, 1, w->h - 2,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    /* Down arrow */
    int ax = bx + btn_w / 2;
    int ay = w->y + w->h / 2 - 1;
    draw_rect(cmd, extent, ax - 3, ay, 7, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax - 2, ay + 1, 5, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax - 1, ay + 2, 3, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, ax, ay + 3, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);

    Widget text_area = *w;
    text_area.x = w->x + 4;
    text_area.w = w->w - btn_w - 4;
    if (text_area.w < 1) text_area.w = 1;
    draw_widget_text(r, cmd, &text_area, extent, 3,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
}
