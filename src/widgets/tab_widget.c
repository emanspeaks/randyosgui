#include "tab_widget.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_tab_widget_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TAB_WIDGET, NULL);
    if (!w) return 0;
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_GROW;
    w->pref_h = 120;
    w->value = 0; /* current page index */
    return w->id;
}

RandyWidgetId randy_tab_widget_add_page(RandyWindow* win,
                                                  RandyWidgetId tab_widget_id,
                                                  const char* label) {
    if (!win) return 0;
    Widget* tw = widget_find(win, tab_widget_id);
    if (!tw || tw->kind != WIDGET_TAB_WIDGET) return 0;

    /* Create a VBOX page and add as child */
    Widget* page = (Widget*)randy_zalloc(sizeof(Widget));
    if (!page) return 0;
    page->id = win->next_id++;
    page->kind = WIDGET_VBOX;
    page->text = randy_strdup(label);
    page->visible = (tw->first_child == NULL); /* first page visible */
    page->enabled = true;
    page->size_policy_h = SIZE_GROW;
    page->size_policy_v = SIZE_GROW;
    page->spacing = 6;

    widget_add_child(tw, page);
    win->needs_layout = true;
    return page->id;
}

void randy_tab_widget_set_current(RandyWindow* win,
                                        RandyWidgetId id,
                                        int page_index) {
    if (!win) return;
    Widget* tw = widget_find(win, id);
    if (!tw || tw->kind != WIDGET_TAB_WIDGET) return;

    int i = 0;
    for (Widget* c = tw->first_child; c; c = c->next_sibling) {
        c->visible = (i == page_index);
        i++;
    }
    tw->value = page_index;
    win->needs_layout = true;
    win->needs_render = true;
}

void draw_tab_widget(RendererContext* r, VkCommandBuffer cmd,
                     const Widget* w, VkExtent2D extent) {
    (void)r;

    /* Draw the tab bar at the top */
    int tab_h = 22;
    int tab_x = w->x;
    int tab_idx = 0;

    for (Widget* page = w->first_child; page; page = page->next_sibling) {
        int tw = 80;
        if (page->text) {
            int approx = 16 + ((int)strlen(page->text) * 7);
            if (approx > tw) tw = approx;
        }
        bool active = (tab_idx == w->value);

        if (active) {
            /* Active tab: raised surface bg, connects to content */
            draw_rect(cmd, extent, tab_x, w->y, tw, tab_h + 1,
                      g_style.surface.r, g_style.surface.g, g_style.surface.b);
            /* Highlight on top and left */
            draw_rect(cmd, extent, tab_x, w->y, tw - 1, 1,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            draw_rect(cmd, extent, tab_x, w->y + 1, 1, tab_h,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            /* Shadow on right */
            draw_rect(cmd, extent, tab_x + tw - 1, w->y, 1, tab_h + 1,
                      g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
            /* Accent underline on active tab */
            draw_rect(cmd, extent, tab_x + 1, w->y + 1, tw - 2, 2,
                      g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
        } else {
            /* Inactive tab: slightly lower, raised */
            draw_rect(cmd, extent, tab_x, w->y + 2, tw, tab_h - 2,
                      g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
            draw_rect(cmd, extent, tab_x, w->y + 2, tw - 1, 1,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            draw_rect(cmd, extent, tab_x, w->y + 3, 1, tab_h - 3,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            draw_rect(cmd, extent, tab_x + tw - 1, w->y + 2, 1, tab_h - 2,
                      g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
        }

        /* Tab label */
        if (page->text) {
            Widget label_area;
            memset(&label_area, 0, sizeof(label_area));
            label_area.text = page->text;
            label_area.x = tab_x + 6;
            label_area.y = active ? w->y + 5 : w->y + 5;
            label_area.w = tw - 12;
            label_area.h = tab_h - 8;
            label_area.visible = true;
            label_area.enabled = true;
            float lbg_r = active ? g_style.surface.r : g_style.button_face.r;
            float lbg_g = active ? g_style.surface.g : g_style.button_face.g;
            float lbg_b = active ? g_style.surface.b : g_style.button_face.b;
            draw_widget_text(r, cmd, &label_area, extent, 2,
                             g_style.text.r, g_style.text.g, g_style.text.b,
                             lbg_r, lbg_g, lbg_b);
        }

        tab_x += tw - 1;
        tab_idx++;
    }

    /* Content area with raised bevel below tab bar */
    int content_y = w->y + tab_h;
    int content_h = w->h - tab_h;
    if (content_h > 0) {
        draw_rect(cmd, extent, w->x, content_y, w->w, content_h,
                  g_style.surface.r, g_style.surface.g, g_style.surface.b);
        draw_bevel(cmd, extent, w->x, content_y, w->w, content_h, false);
    }
}
