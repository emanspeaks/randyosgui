#include "tab.h"

RandyosgWidgetId randyosgui_tab_create(RandyosgWindow* win,
                                        const char* label,
                                        bool active) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TAB, label);
    if (!w) return 0;
    w->checked = active;
    return w->id;
}

void randyosgui_tab_set_active(RandyosgWindow* win,
                                RandyosgWidgetId id,
                                bool active) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TAB) return;
    w->checked = active;
}

bool randyosgui_tab_get_active(RandyosgWindow* win,
                                RandyosgWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TAB) return false;
    return w->checked;
}

void randyosgui_tab_set_callback(RandyosgWindow* win,
                                  RandyosgWidgetId id,
                                  RandyosgClickCallback cb,
                                  void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TAB) return;
    w->click_cb = cb;
    w->click_userdata = userdata;
}

void draw_tab(RendererContext* r, VkCommandBuffer cmd,
              const Widget* w, VkExtent2D extent,
              const Widget* widgets_head, bool* in_tab_strip) {
    if (!*in_tab_strip) {
        const Widget* t = w;
        int tabs_left = w->x;
        int tabs_right = w->x + w->w;
        int active_left = -1;
        int active_right = -1;
        while (t && t->kind == WIDGET_TAB) {
            if (t->x < tabs_left) tabs_left = t->x;
            if (t->x + t->w > tabs_right) tabs_right = t->x + t->w;
            if (t->checked) {
                active_left = t->x + 2;
                active_right = t->x + t->w - 2;
            }
            t = t->next;
        }

        int base_y = w->y + w->h - 1;
        if (active_left < 0 || active_right <= active_left) {
            draw_rect(cmd, extent, tabs_left, base_y, tabs_right - tabs_left, 1,
                      WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
        } else {
            if (active_left > tabs_left) {
                draw_rect(cmd, extent, tabs_left, base_y, active_left - tabs_left, 1,
                          WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
            }
            if (tabs_right > active_right) {
                draw_rect(cmd, extent, active_right, base_y, tabs_right - active_right, 1,
                          WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
            }
        }
        *in_tab_strip = true;
    }

    int tx = w->x;
    int ty = w->checked ? (w->y - 2) : w->y;
    int th = w->checked ? (w->h + 2) : w->h;

    draw_rect(cmd, extent, tx, ty, w->w, th,
              WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
    draw_tab_border_98(cmd, extent, tx, ty, w->w, th);

    if (w->checked) {
        draw_rect(cmd, extent, tx + 2, ty + th - 1, w->w - 4, 2,
                  WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
    }

    Widget text_area = *w;
    int tw = approx_text_px(w->text);
    text_area.x = tx + ((w->w - tw) / 2) - 6;
    text_area.w = tw + 12;
    draw_widget_text(r, cmd, &text_area, extent, 4,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);

    (void)widgets_head;
}
