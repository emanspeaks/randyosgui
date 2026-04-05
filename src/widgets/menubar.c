#include "menubar.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_menubar_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_MENUBAR, NULL);
    if (!w) return 0;
    w->pref_h = 20;
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_FIXED;
    return w->id;
}

RandyWidgetId randy_menu_item_create(RandyWindow* win, const char* label) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_MENU_ITEM, label);
    if (!w) return 0;
    w->pref_h = 20;
    w->size_policy_h = SIZE_FIT;
    w->pref_w = 60;
    return w->id;
}

void randy_menu_item_set_callback(RandyWindow* win, RandyWidgetId id,
                                        RandyClickCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->click_cb = cb;
    w->click_userdata = userdata;
}

void draw_menubar(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    (void)r;
    /* Menubar background strip */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    /* Highlight top, shadow bottom */
    draw_rect(cmd, extent, w->x, w->y, w->w, 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, w->x, w->y + w->h - 1, w->w, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
}

void draw_menu_item(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent) {
    /* Qt Fusion: highlight bg on hover/press, no bevels */
    if (w->pressed || w->hovered) {
        draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
                  g_style.button_hover.r, g_style.button_hover.g, g_style.button_hover.b);
    }

    float bg_r = (w->pressed || w->hovered) ? g_style.button_hover.r : g_style.surface.r;
    float bg_g = (w->pressed || w->hovered) ? g_style.button_hover.g : g_style.surface.g;
    float bg_b = (w->pressed || w->hovered) ? g_style.button_hover.b : g_style.surface.b;
    draw_widget_text(r, cmd, w, extent, 6,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     bg_r, bg_g, bg_b);
}
