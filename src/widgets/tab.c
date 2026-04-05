#include "tab.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_tab_create(RandyWindow* win,
                                        const char* label,
                                        bool active) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TAB, label);
    if (!w) return 0;
    w->checked = active;
    return w->id;
}

void randy_tab_set_active(RandyWindow* win,
                                RandyWidgetId id,
                                bool active) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TAB) return;
    w->checked = active;
}

bool randy_tab_get_active(RandyWindow* win,
                                RandyWidgetId id) {
    if (!win) return false;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TAB) return false;
    return w->checked;
}

void randy_tab_set_callback(RandyWindow* win,
                                  RandyWidgetId id,
                                  RandyClickCallback cb,
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
            t = t->next_sibling;
        }

        int base_y = w->y + w->h - 1;
        if (active_left < 0 || active_right <= active_left) {
            draw_rect(cmd, extent, tabs_left, base_y, tabs_right - tabs_left, 1,
                      g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
        } else {
            if (active_left > tabs_left) {
                draw_rect(cmd, extent, tabs_left, base_y, active_left - tabs_left, 1,
                          g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
            }
            if (tabs_right > active_right) {
                draw_rect(cmd, extent, active_right, base_y, tabs_right - active_right, 1,
                          g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
            }
        }
        *in_tab_strip = true;
    }

    int tx = w->x;
    int ty = w->checked ? (w->y - 2) : w->y;
    int th = w->checked ? (w->h + 2) : w->h;

    /* Raised tab with 3D bevel on top and sides */
    float tab_bg_r = w->checked ? g_style.surface.r : g_style.button_face.r;
    float tab_bg_g = w->checked ? g_style.surface.g : g_style.button_face.g;
    float tab_bg_b = w->checked ? g_style.surface.b : g_style.button_face.b;
    draw_rect(cmd, extent, tx, ty, w->w, th, tab_bg_r, tab_bg_g, tab_bg_b);

    /* Highlight on top and left */
    draw_rect(cmd, extent, tx, ty, w->w - 1, 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, tx, ty + 1, 1, th - 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    /* Shadow on right */
    draw_rect(cmd, extent, tx + w->w - 1, ty, 1, th,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);

    if (w->checked) {
        /* Active: accent underline and erase bottom border */
        draw_rect(cmd, extent, tx + 1, ty + 1, w->w - 2, 2,
                  g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
        draw_rect(cmd, extent, tx + 1, ty + th - 1, w->w - 2, 1, tab_bg_r, tab_bg_g, tab_bg_b);
    }

    Widget text_area = *w;
    int tw = approx_text_px(w->text);
    text_area.x = tx + ((w->w - tw) / 2) - 6;
    text_area.w = tw + 12;
    draw_widget_text(r, cmd, &text_area, extent, 4,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);

    (void)widgets_head;
}
