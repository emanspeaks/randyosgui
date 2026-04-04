#include "checkbox.h"

RandyosgWidgetId randyosgui_checkbox_create(RandyosgWindow* win,
                                             const char* label,
                                             bool checked) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_CHECKBOX, label);
    if (!w) return 0;
    w->checked = checked;
    return w->id;
}

void randyosgui_checkbox_set_checked(RandyosgWindow* win,
                                      RandyosgWidgetId id,
                                      bool checked) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_CHECKBOX) return;
    w->checked = checked;
}

bool randyosgui_checkbox_get_checked(RandyosgWindow* win,
                                      RandyosgWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_CHECKBOX) return false;
    return w->checked;
}

void randyosgui_checkbox_set_callback(RandyosgWindow* win,
                                       RandyosgWidgetId id,
                                       RandyosgToggleCallback cb,
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

    draw_rect(cmd, extent, cb_x, cb_y, box, box, 0.98f, 0.98f, 0.98f);
    draw_bevel(cmd, extent, cb_x, cb_y, box, box, true);

    if (w->hovered) {
        draw_rect(cmd, extent, cb_x - 1, cb_y - 1, box + 2, 1, 0.70f, 0.70f, 0.70f);
        draw_rect(cmd, extent, cb_x - 1, cb_y + box, box + 2, 1, 0.70f, 0.70f, 0.70f);
        draw_rect(cmd, extent, cb_x - 1, cb_y - 1, 1, box + 2, 0.70f, 0.70f, 0.70f);
        draw_rect(cmd, extent, cb_x + box, cb_y - 1, 1, box + 2, 0.70f, 0.70f, 0.70f);
    }

    if (w->checked) {
        float cr = WIN98.window_frame_r;
        float cg = WIN98.window_frame_g;
        float cb2 = WIN98.window_frame_b;
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
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
}
