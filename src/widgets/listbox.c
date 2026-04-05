#include "listbox.h"

RandyWidgetId randy_listbox_create(RandyWindow* win,
                                            int num_items,
                                            const char* const* items,
                                            int selected) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_LISTBOX, NULL);
    if (!w) return 0;
    w->pref_h = 80;
    w->size_policy_v = SIZE_GROW;
    w->value = selected;
    w->min_value = 0;
    w->max_value = num_items > 0 ? num_items - 1 : 0;

    /* Store items as cells array (reuse table infrastructure) */
    if (num_items > 0 && items) {
        w->cells = (char**)randy_alloc(sizeof(char*) * (size_t)num_items);
        if (w->cells) {
            w->num_cells = num_items;
            for (int i = 0; i < num_items; i++) {
                w->cells[i] = randy_strdup(items[i]);
            }
        }
    }
    return w->id;
}

void randy_listbox_set_callback(RandyWindow* win, RandyWidgetId id,
                                      RandyValueCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->value_cb = cb;
    w->value_userdata = userdata;
}

void draw_listbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    /* White background with sunken border */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    /* Sunken border */
    draw_rect(cmd, extent, w->x, w->y, w->w, 1,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x, w->y, 1, w->h,
              g_style.button_shadow.r, g_style.button_shadow.g, g_style.button_shadow.b);
    draw_rect(cmd, extent, w->x + w->w - 1, w->y, 1, w->h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_rect(cmd, extent, w->x, w->y + w->h - 1, w->w, 1,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);

    /* Draw items */
    int item_h = 16;
    int y = w->y + 2;
    for (int i = 0; i < w->num_cells && y + item_h <= w->y + w->h; i++) {
        if (i == w->value) {
            /* Selected item: navy background, white text */
            draw_rect(cmd, extent, w->x + 2, y, w->w - 4, item_h,
                      g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
            Widget item_text = *w;
            item_text.text = w->cells[i];
            item_text.x = w->x + 4;
            item_text.y = y;
            item_text.w = w->w - 8;
            item_text.h = item_h;
            draw_widget_text(r, cmd, &item_text, extent, 2,
                             g_style.highlight_text.r, g_style.highlight_text.g, g_style.highlight_text.b,
                             g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
        } else {
            Widget item_text = *w;
            item_text.text = w->cells[i];
            item_text.x = w->x + 4;
            item_text.y = y;
            item_text.w = w->w - 8;
            item_text.h = item_h;
            draw_widget_text(r, cmd, &item_text, extent, 2,
                             g_style.text.r, g_style.text.g, g_style.text.b,
                             g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
        }
        y += item_h;
    }
}
