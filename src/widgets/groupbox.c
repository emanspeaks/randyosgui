#include "groupbox.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_groupbox_create(RandyWindow* win,
                                             const char* title) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_GROUPBOX, title);
    return w ? w->id : 0;
}

void draw_groupbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    draw_widget_rect(cmd, w, extent, g_style.surface.r, g_style.surface.g, g_style.surface.b);

    int title_px = approx_text_px(w->text) + 6;
    int title_left = w->x + 8;
    int title_right = title_left + title_px;
    int top_y = w->y + 7;
    int left = w->x;
    int right = w->x + w->w - 1;
    int bottom = w->y + w->h - 1;

    /* Etched border with gap for title */
    float sr = g_style.button_shadow.r;
    float sg = g_style.button_shadow.g;
    float sb = g_style.button_shadow.b;
    float hr = g_style.button_highlight.r;
    float hg = g_style.button_highlight.g;
    float hb = g_style.button_highlight.b;

    /* Shadow lines (outer) */
    draw_rect(cmd, extent, left, top_y, title_left - left - 2, 1, sr, sg, sb);
    draw_rect(cmd, extent, title_right + 2, top_y, right - title_right - 2, 1, sr, sg, sb);
    draw_rect(cmd, extent, left, top_y + 1, 1, bottom - top_y - 1, sr, sg, sb);
    draw_rect(cmd, extent, left, bottom, right - left + 1, 1, sr, sg, sb);
    draw_rect(cmd, extent, right, top_y + 1, 1, bottom - top_y - 1, sr, sg, sb);
    /* Highlight lines (inner, offset +1) */
    draw_rect(cmd, extent, left + 1, top_y + 1, title_left - left - 3, 1, hr, hg, hb);
    draw_rect(cmd, extent, title_right + 2, top_y + 1, right - title_right - 3, 1, hr, hg, hb);
    draw_rect(cmd, extent, left + 1, top_y + 2, 1, bottom - top_y - 3, hr, hg, hb);
    draw_rect(cmd, extent, left + 1, bottom - 1, right - left - 1, 1, hr, hg, hb);
    draw_rect(cmd, extent, right - 1, top_y + 2, 1, bottom - top_y - 3, hr, hg, hb);

    Widget text_area = *w;
    text_area.x = title_left;
    text_area.y = w->y - 3;
    text_area.h = 18;
    text_area.w = title_px;
    draw_widget_text(r, cmd, &text_area, extent, 6,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
