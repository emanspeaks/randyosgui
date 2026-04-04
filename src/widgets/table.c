#include "table.h"

RandyosgWidgetId randyosgui_table_header_create(RandyosgWindow* win,
                                                 int num_cols,
                                                 const char* const* labels,
                                                 const int* col_widths) {
    if (!win || num_cols <= 0 || !labels || !col_widths) return 0;
    Widget* w = widget_alloc(win, WIDGET_TABLE_HEADER, NULL);
    if (!w) return 0;

    w->num_cells  = num_cols;
    w->cells      = (char**)randyosgui_alloc((size_t)num_cols * sizeof(char*));
    w->col_widths = (int*)randyosgui_alloc((size_t)num_cols * sizeof(int));
    if (!w->cells || !w->col_widths) {
        free(w->cells); free(w->col_widths);
        w->cells = NULL; w->col_widths = NULL; w->num_cells = 0;
        return 0;
    }
    for (int i = 0; i < num_cols; i++) {
        w->cells[i]      = randyosgui_strdup(labels[i]);
        w->col_widths[i] = col_widths[i];
    }
    return w->id;
}

RandyosgWidgetId randyosgui_table_row_create(RandyosgWindow* win,
                                              int num_cells,
                                              const char* const* cells,
                                              bool selected) {
    if (!win || num_cells <= 0 || !cells) return 0;
    Widget* w = widget_alloc(win, WIDGET_TABLE_ROW, NULL);
    if (!w) return 0;

    w->num_cells = num_cells;
    w->cells     = (char**)randyosgui_alloc((size_t)num_cells * sizeof(char*));
    if (!w->cells) { w->num_cells = 0; return 0; }
    for (int i = 0; i < num_cells; i++) {
        w->cells[i] = randyosgui_strdup(cells[i]);
    }
    w->checked = selected;
    return w->id;
}

void randyosgui_table_row_set_callback(RandyosgWindow* win,
                                        RandyosgWidgetId id,
                                        RandyosgClickCallback cb,
                                        void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TABLE_ROW) return;
    w->click_cb = cb;
    w->click_userdata = userdata;
}

void draw_table_header(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent) {
    if (w->num_cells <= 0 || !w->cells || !w->col_widths) return;
    int cx = w->x;
    for (int col = 0; col < w->num_cells; col++) {
        int cw = w->col_widths[col];
        draw_rect(cmd, extent, cx, w->y, cw, w->h,
                  WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
        draw_bevel(cmd, extent, cx, w->y, cw, w->h, false);
        if (r->font_sans && w->cells[col] && w->cells[col][0]) {
            Widget clip = *w;
            clip.x = cx + 2; clip.w = cw - 4;
            const char* s = w->cells[col];
            draw_text_span(r, cmd, extent, &clip, r->font_sans,
                           s, s + strlen(s),
                           cx + 6, w->y + (w->h / 2) + 4,
                           WIN98.text_r, WIN98.text_g, WIN98.text_b,
                           WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
        }
        cx += cw;
    }
}

void draw_table_row(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent,
                    const Widget* widgets_head) {
    float bg_r, bg_g, bg_b, fg_r, fg_g, fg_b;
    if (w->checked) {
        bg_r = WIN98.dialog_blue_r; bg_g = WIN98.dialog_blue_g; bg_b = WIN98.dialog_blue_b;
        fg_r = 1.0f; fg_g = 1.0f; fg_b = 1.0f;
    } else {
        bg_r = 1.0f; bg_g = 1.0f; bg_b = 1.0f;
        fg_r = WIN98.text_r; fg_g = WIN98.text_g; fg_b = WIN98.text_b;
    }
    draw_widget_rect(cmd, w, extent, bg_r, bg_g, bg_b);

    if (w->num_cells <= 0 || !w->cells) return;

    const Widget* hdr = NULL;
    for (const Widget* h = widgets_head; h != w; h = h->next) {
        if (h->kind == WIDGET_TABLE_HEADER) hdr = h;
    }
    if (!hdr || !hdr->col_widths || hdr->num_cells != w->num_cells) {
        int col_w = w->w / w->num_cells;
        int cx = w->x;
        for (int col = 0; col < w->num_cells; col++) {
            int cw = (col == w->num_cells - 1) ? (w->x + w->w - cx) : col_w;
            if (r->font_sans && w->cells[col] && w->cells[col][0]) {
                Widget clip = *w; clip.x = cx + 2; clip.w = cw - 4;
                const char* s = w->cells[col];
                draw_text_span(r, cmd, extent, &clip, r->font_sans,
                               s, s + strlen(s),
                               cx + 6, w->y + (w->h / 2) + 4,
                               fg_r, fg_g, fg_b, bg_r, bg_g, bg_b);
            }
            cx += cw;
        }
    } else {
        int cx = w->x;
        for (int col = 0; col < w->num_cells; col++) {
            int cw = hdr->col_widths[col];
            if (r->font_sans && w->cells[col] && w->cells[col][0]) {
                Widget clip = *w; clip.x = cx + 2; clip.w = cw - 4;
                const char* s = w->cells[col];
                draw_text_span(r, cmd, extent, &clip, r->font_sans,
                               s, s + strlen(s),
                               cx + 6, w->y + (w->h / 2) + 4,
                               fg_r, fg_g, fg_b, bg_r, bg_g, bg_b);
            }
            cx += cw;
        }
    }
}
