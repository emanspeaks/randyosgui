#include "tree.h"

RandyosgWidgetId randyosgui_tree_item_create(RandyosgWindow* win,
                                              const char* label,
                                              int depth,
                                              bool expandable,
                                              bool expanded) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TREE_ITEM, label);
    if (!w) return 0;
    if (depth < 0) depth = 0;
    if (depth > 8) depth = 8;
    w->value = depth;
    w->readonly = expandable;
    w->checked = expanded;
    return w->id;
}

void draw_tree_item(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent,
                    const Widget* widgets_head) {
    Widget* prev = NULL;
    for (Widget* p = (Widget*)widgets_head; p && p != w; p = p->next) prev = p;
    bool first_tree = (!prev || prev->kind != WIDGET_TREE_ITEM);

    const Widget* block_start = w;
    while (block_start) {
        Widget* pprev = NULL;
        for (Widget* p = (Widget*)widgets_head; p && p != block_start; p = p->next) pprev = p;
        if (!pprev || pprev->kind != WIDGET_TREE_ITEM) break;
        block_start = pprev;
    }

    if (first_tree) {
        int block_y = w->y - 2;
        int block_h = w->h + 4;
        for (const Widget* t = w->next; t && t->kind == WIDGET_TREE_ITEM; t = t->next) {
            block_h = (t->y + t->h + 2) - block_y;
        }
        draw_rect(cmd, extent, w->x, block_y, w->w, block_h,
                  WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
        draw_rect(cmd, extent, w->x, block_y, w->w, 1,
                  WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
        draw_rect(cmd, extent, w->x, block_y + block_h - 1, w->w, 1,
                  WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
        draw_rect(cmd, extent, w->x, block_y, 1, block_h,
                  WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
        draw_rect(cmd, extent, w->x + w->w - 1, block_y, 1, block_h,
                  WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
    }

    int tree_base_x = w->x + 6;
    int node_x = tree_base_x + (w->value * 16);
    int row_mid_y = w->y + (w->h / 2);

    for (int level = 0; level < w->value; level++) {
        const Widget* ancestor = tree_find_ancestor(block_start, w, level);
        if (!ancestor) continue;
        if (!tree_has_next_sibling(ancestor)) continue;

        int vx = tree_base_x + (level * 16) + 4;
        for (int py = w->y - 1; py < w->y + w->h; py += 2) {
            draw_rect(cmd, extent, vx, py, 1, 1, 0.502f, 0.502f, 0.502f);
        }
    }

    if (w->value > 0) {
        int vx = tree_base_x + ((w->value - 1) * 16) + 4;
        bool has_prev = tree_has_prev_sibling(block_start, w);
        bool has_next = tree_has_next_sibling(w);

        int y_top = has_prev ? (w->y - 1) : row_mid_y;
        int y_bottom = has_next ? (w->y + w->h) : (row_mid_y + 1);
        for (int py = y_top; py < y_bottom; py += 2) {
            draw_rect(cmd, extent, vx, py, 1, 1, 0.502f, 0.502f, 0.502f);
        }

        int target_x = node_x + 4;
        for (int px = vx; px <= target_x; px += 2) {
            draw_rect(cmd, extent, px, row_mid_y, 1, 1, 0.502f, 0.502f, 0.502f);
        }
    }

    if (w->readonly) {
        int bx = node_x;
        int by = w->y + 3;
        draw_rect(cmd, extent, bx, by, 9, 9, 0.98f, 0.98f, 0.98f);
        draw_rect(cmd, extent, bx, by, 9, 1,
                  WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
        draw_rect(cmd, extent, bx, by + 8, 9, 1,
                  WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
        draw_rect(cmd, extent, bx, by, 1, 9,
                  WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
        draw_rect(cmd, extent, bx + 8, by, 1, 9,
                  WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);

        draw_rect(cmd, extent, bx + 2, by + 4, 5, 1,
                  WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
        if (!w->checked) {
            draw_rect(cmd, extent, bx + 4, by + 2, 1, 5,
                      WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
        }
    }

    Widget text_area = *w;
    text_area.x = node_x + (w->readonly ? 14 : 0);
    text_area.w = w->w - (text_area.x - w->x);
    draw_widget_text(r, cmd, &text_area, extent, 4,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);
}
