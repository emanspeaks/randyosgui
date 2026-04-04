#include "groupbox.h"

RandyosgWidgetId randyosgui_groupbox_create(RandyosgWindow* win,
                                             const char* title) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_GROUPBOX, title);
    return w ? w->id : 0;
}

void draw_groupbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    draw_widget_rect(cmd, w, extent, WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);

    int title_px = approx_text_px(w->text) + 6;
    int title_left = w->x + 8;
    int title_right = title_left + title_px;
    int top_y = w->y + 7;
    int left = w->x;
    int right = w->x + w->w - 1;
    int bottom = w->y + w->h - 1;

    draw_rect(cmd, extent, left, top_y, title_left - left - 2, 1,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, title_right + 2, top_y, right - title_right - 2, 1,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, left, top_y + 1, title_left - left - 2, 1,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
    draw_rect(cmd, extent, title_right + 2, top_y + 1, right - title_right - 2, 1,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);

    draw_rect(cmd, extent, left, top_y + 1, 1, bottom - top_y,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, left + 1, top_y + 2, 1, bottom - top_y - 1,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);

    draw_rect(cmd, extent, right - 1, top_y + 1, 1, bottom - top_y,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, right, top_y + 2, 1, bottom - top_y - 1,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);

    draw_rect(cmd, extent, left, bottom - 1, right - left, 1,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, left + 1, bottom, right - left, 1,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);

    Widget text_area = *w;
    text_area.x = title_left;
    text_area.y = w->y - 3;
    text_area.h = 18;
    text_area.w = title_px;
    draw_widget_text(r, cmd, &text_area, extent, 6,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
}
