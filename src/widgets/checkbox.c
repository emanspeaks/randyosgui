#include "checkbox.h"

RandyWidgetId randy_checkbox_create(RandyWindow* win,
                                             const char* label,
                                             bool checked) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_CHECKBOX, label);
    if (!w) return 0;
    w->checked = checked;
    return w->id;
}

void randy_checkbox_set_checked(RandyWindow* win,
                                      RandyWidgetId id,
                                      bool checked) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_CHECKBOX) return;
    w->checked = checked;
}

bool randy_checkbox_get_checked(RandyWindow* win,
                                      RandyWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_CHECKBOX) return false;
    return w->checked;
}

void randy_checkbox_set_callback(RandyWindow* win,
                                       RandyWidgetId id,
                                       RandyToggleCallback cb,
                                       void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_CHECKBOX) return;
    w->toggle_cb = cb;
    w->toggle_userdata = userdata;
}

void draw_checkbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    int box = 13;
    int cb_x = w->x;
    int cb_y = w->y + ((w->h - box) / 2);

    draw_rect(cmd, extent, cb_x, cb_y, box, box, g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, cb_x, cb_y, box, box, true);

    if (w->hovered) {
        draw_rect(cmd, extent, cb_x - 1, cb_y - 1, box + 2, 1, g_style.input_border_hover.r, g_style.input_border_hover.g, g_style.input_border_hover.b);
        draw_rect(cmd, extent, cb_x - 1, cb_y + box, box + 2, 1, g_style.input_border_hover.r, g_style.input_border_hover.g, g_style.input_border_hover.b);
        draw_rect(cmd, extent, cb_x - 1, cb_y - 1, 1, box + 2, g_style.input_border_hover.r, g_style.input_border_hover.g, g_style.input_border_hover.b);
        draw_rect(cmd, extent, cb_x + box, cb_y - 1, 1, box + 2, g_style.input_border_hover.r, g_style.input_border_hover.g, g_style.input_border_hover.b);
    }

    if (w->checked) {
        float cr = g_style.window_frame.r;
        float cg = g_style.window_frame.g;
        float cb2 = g_style.window_frame.b;
        int sx = cb_x + 3;
        int sy = cb_y + 3;

        draw_rect(cmd, extent, sx + 6, sy + 0, 1, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 5, sy + 1, 2, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 0, sy + 2, 1, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 4, sy + 2, 3, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 0, sy + 3, 2, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 3, sy + 3, 3, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 0, sy + 4, 5, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 1, sy + 5, 3, 1, cr, cg, cb2);
        draw_rect(cmd, extent, sx + 2, sy + 6, 1, 1, cr, cg, cb2);
    }

    Widget text_area = *w;
    text_area.x = w->x + 19;
    text_area.w = w->w - 19;
    draw_widget_text(r, cmd, &text_area, extent, 4,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
