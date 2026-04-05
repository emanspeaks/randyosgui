#include "tab_widget.h"

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
            /* Active tab: raised, connects to content area */
            draw_rect(cmd, extent, tab_x, w->y, tw, tab_h + 1,
                      g_style.surface.r, g_style.surface.g, g_style.surface.b);
            draw_rect(cmd, extent, tab_x, w->y, 1, tab_h + 1,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            draw_rect(cmd, extent, tab_x, w->y, tw, 1,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            draw_rect(cmd, extent, tab_x + tw - 1, w->y, 1, tab_h + 1,
                      g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
        } else {
            /* Inactive tab: slightly lower */
            draw_rect(cmd, extent, tab_x, w->y + 2, tw, tab_h - 2,
                      g_style.surface.r, g_style.surface.g, g_style.surface.b);
            draw_rect(cmd, extent, tab_x, w->y + 2, 1, tab_h - 2,
                      g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
            draw_rect(cmd, extent, tab_x, w->y + 2, tw, 1,
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
            label_area.y = active ? w->y + 3 : w->y + 5;
            label_area.w = tw - 12;
            label_area.h = tab_h - 6;
            label_area.visible = true;
            label_area.enabled = true;
            draw_widget_text(r, cmd, &label_area, extent, 2,
                             g_style.text.r, g_style.text.g, g_style.text.b,
                             g_style.surface.r, g_style.surface.g, g_style.surface.b);
        }

        tab_x += tw - 1;
        tab_idx++;
    }

    /* Content area border below tab bar */
    int content_y = w->y + tab_h;
    int content_h = w->h - tab_h;
    if (content_h > 0) {
        draw_rect(cmd, extent, w->x, content_y, w->w, content_h,
                  g_style.surface.r, g_style.surface.g, g_style.surface.b);
        /* Raised border */
        draw_rect(cmd, extent, w->x, content_y, w->w, 1,
                  g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
        draw_rect(cmd, extent, w->x, content_y, 1, content_h,
                  g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
        draw_rect(cmd, extent, w->x + w->w - 1, content_y, 1, content_h,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
        draw_rect(cmd, extent, w->x, content_y + content_h - 1, w->w, 1,
                  g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    }
}
