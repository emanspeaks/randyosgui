#include "radio.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

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
    int sz = 13;
    int rd_x = w->x;
    int rd_y = w->y + ((w->h - sz) / 2);

    /* Qt Fusion radio: white filled circle with 1px gray border */
    float br = g_style.button_shadow.r;
    float bg = g_style.button_shadow.g;
    float bb = g_style.button_shadow.b;
    float fr = g_style.input_background.r;
    float fg = g_style.input_background.g;
    float fb = g_style.input_background.b;

    /* Fill: approximate circle with rects */
    draw_rect(cmd, extent, rd_x + 4, rd_y + 1, 5, 1, fr, fg, fb);
    draw_rect(cmd, extent, rd_x + 2, rd_y + 2, 9, 1, fr, fg, fb);
    draw_rect(cmd, extent, rd_x + 1, rd_y + 3, 11, 7, fr, fg, fb);
    draw_rect(cmd, extent, rd_x + 2, rd_y + 10, 9, 1, fr, fg, fb);
    draw_rect(cmd, extent, rd_x + 4, rd_y + 11, 5, 1, fr, fg, fb);

    /* Border: approximate circle outline */
    draw_rect(cmd, extent, rd_x + 4, rd_y + 0, 5, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 2, rd_y + 1, 2, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 9, rd_y + 1, 2, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 1, rd_y + 2, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 11, rd_y + 2, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 0, rd_y + 3, 1, 7, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 12, rd_y + 3, 1, 7, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 1, rd_y + 10, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 11, rd_y + 10, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 2, rd_y + 11, 2, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 9, rd_y + 11, 2, 1, br, bg, bb);
    draw_rect(cmd, extent, rd_x + 4, rd_y + 12, 5, 1, br, bg, bb);

    if (w->hovered) {
        /* Hover: use accent color for border */
        float hr = g_style.input_border_hover.r;
        float hg = g_style.input_border_hover.g;
        float hb = g_style.input_border_hover.b;
        draw_rect(cmd, extent, rd_x + 4, rd_y + 0, 5, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 2, rd_y + 1, 2, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 9, rd_y + 1, 2, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 1, rd_y + 2, 1, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 11, rd_y + 2, 1, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 0, rd_y + 3, 1, 7, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 12, rd_y + 3, 1, 7, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 1, rd_y + 10, 1, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 11, rd_y + 10, 1, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 2, rd_y + 11, 2, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 9, rd_y + 11, 2, 1, hr, hg, hb);
        draw_rect(cmd, extent, rd_x + 4, rd_y + 12, 5, 1, hr, hg, hb);
    }

    if (w->checked) {
        /* Filled dot: accent color circle in center */
        float dr = g_style.highlight.r;
        float dg = g_style.highlight.g;
        float db = g_style.highlight.b;
        draw_rect(cmd, extent, rd_x + 5, rd_y + 4, 3, 1, dr, dg, db);
        draw_rect(cmd, extent, rd_x + 4, rd_y + 5, 5, 3, dr, dg, db);
        draw_rect(cmd, extent, rd_x + 5, rd_y + 8, 3, 1, dr, dg, db);
    }

    Widget text_area = *w;
    text_area.x = w->x + 19;
    text_area.w = w->w - 19;
    draw_widget_text(r, cmd, &text_area, extent, 4,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
