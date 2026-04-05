#include "randyosgui_internal.h"
#include "style.h"

/* =========================================================================
 * Layout constants (fed from g_style metrics)
 * ========================================================================= */

#define RANDY_LAYOUT_PADDING_X    g_style.content_padding_x
#define RANDY_LAYOUT_PADDING_TOP  g_style.title_bar_height
#define RANDY_LAYOUT_SPACING      g_style.default_spacing
#define RANDY_LAYOUT_BOTTOM_MARGIN g_style.content_bottom_margin

/* Return inter-widget spacing based on widget kinds (compact for trees/tables) */
static int widget_layout_spacing(const Widget* w) {
    if (!w) return RANDY_LAYOUT_SPACING;
    if (w->kind == WIDGET_TREE_ITEM) {
        return (w->next_sibling && w->next_sibling->kind == WIDGET_TREE_ITEM) ? 3 : RANDY_LAYOUT_SPACING;
    }
    if (w->kind == WIDGET_TABLE_HEADER && w->next_sibling && w->next_sibling->kind == WIDGET_TABLE_ROW) {
        return 0;
    }
    if (w->kind == WIDGET_TABLE_ROW) {
        return (w->next_sibling && w->next_sibling->kind == WIDGET_TABLE_ROW) ? 0 : RANDY_LAYOUT_SPACING;
    }
    if (w->kind == WIDGET_STATUS_FIELD) {
        return (w->next_sibling && w->next_sibling->kind == WIDGET_STATUS_FIELD) ? 1 : RANDY_LAYOUT_SPACING;
    }
    return RANDY_LAYOUT_SPACING;
}

/* =========================================================================
 * Recursive layout engine (Clay-inspired sizing model)
 * ========================================================================= */

/* Forward declarations for mutual recursion */
static void layout_vbox(Widget* container, int x, int y, int w, int h);
static void layout_hbox(Widget* container, int x, int y, int w, int h);
static void layout_widget_children(Widget* w);

/* Resolve a widget's height: preferred, or content-based for FIT containers */
static int resolve_height(Widget* w) {
    if (!w || !w->visible) return 0;

    /* Containers: sum children */
    if (w->kind == WIDGET_VBOX) {
        int total = w->padding * 2;
        int count = 0;
        for (Widget* c = w->first_child; c; c = c->next_sibling) {
            if (!c->visible) continue;
            total += resolve_height(c);
            if (count > 0) total += w->spacing > 0 ? w->spacing : widget_layout_spacing(c);
            count++;
        }
        return total > 0 ? total : w->pref_h;
    }

    if (w->kind == WIDGET_HBOX) {
        int max_h = 0;
        for (Widget* c = w->first_child; c; c = c->next_sibling) {
            if (!c->visible) continue;
            int ch = resolve_height(c);
            if (ch > max_h) max_h = ch;
        }
        return max_h + w->padding * 2;
    }

    /* Container widgets that size to fit their children */
    if (w->first_child) {
        int sp = w->spacing > 0 ? w->spacing : RANDY_LAYOUT_SPACING;
        if (w->kind == WIDGET_GROUPBOX) {
            int total = 24; /* 16px top inset + 8px bottom */
            int count = 0;
            for (Widget* c = w->first_child; c; c = c->next_sibling) {
                if (!c->visible) continue;
                total += resolve_height(c);
                if (count > 0) total += sp;
                count++;
            }
            return total;
        }
        if (w->kind == WIDGET_ACCORDION && w->checked) {
            int total = g_style.accordion_header_height + 2;
            int count = 0;
            for (Widget* c = w->first_child; c; c = c->next_sibling) {
                if (!c->visible) continue;
                total += resolve_height(c);
                if (count > 0) total += sp;
                count++;
            }
            return total;
        }
    }

    return w->pref_h;
}

/* Layout children of a VBOX: top-to-bottom, respecting size policies */
static void layout_vbox(Widget* container, int x, int y, int w, int h) {
    if (!container) return;
    container->x = x;
    container->y = y;
    container->w = w;
    container->h = h;

    int pad = container->padding;
    int cx = x + pad;
    int cy = y + pad;
    int cw = w - pad * 2;
    int avail_h = h - pad * 2;

    if (cw < 1) cw = 1;
    if (avail_h < 1) avail_h = 1;

    /* First pass: measure fixed/fit children, count GROW weight */
    int fixed_total = 0;
    int grow_total_weight = 0;
    int visible_count = 0;
    int spacing_total = 0;

    for (Widget* c = container->first_child; c; c = c->next_sibling) {
        if (!c->visible) continue;
        if (visible_count > 0) {
            spacing_total += container->spacing > 0 ? container->spacing : widget_layout_spacing(c);
        }
        visible_count++;

        if (c->size_policy_v == SIZE_GROW) {
            grow_total_weight += c->grow_weight > 0 ? c->grow_weight : 1;
        } else {
            int ch = resolve_height(c);
            if (c->min_h > 0 && ch < c->min_h) ch = c->min_h;
            if (c->max_h > 0 && ch > c->max_h) ch = c->max_h;
            fixed_total += ch;
        }
    }

    int remaining = avail_h - fixed_total - spacing_total;
    if (remaining < 0) remaining = 0;

    /* Second pass: position children */
    int count = 0;
    for (Widget* c = container->first_child; c; c = c->next_sibling) {
        if (!c->visible) continue;

        if (count > 0) {
            cy += container->spacing > 0 ? container->spacing : widget_layout_spacing(c);
        }
        count++;

        int ch;
        if (c->size_policy_v == SIZE_GROW) {
            int weight = c->grow_weight > 0 ? c->grow_weight : 1;
            ch = (grow_total_weight > 0) ? (remaining * weight) / grow_total_weight : 0;
        } else if (c->size_policy_v == SIZE_PERCENT) {
            ch = (avail_h * c->size_percent) / 100;
        } else {
            ch = resolve_height(c);
        }
        if (c->min_h > 0 && ch < c->min_h) ch = c->min_h;
        if (c->max_h > 0 && ch > c->max_h) ch = c->max_h;

        int child_w = cw;
        if (c->size_policy_h == SIZE_FIXED && c->pref_w > 0) {
            child_w = c->pref_w;
        } else if (c->size_policy_h == SIZE_FIT && c->pref_w > 0) {
            child_w = c->pref_w;
            if (c->min_w > 0 && child_w < c->min_w) child_w = c->min_w;
        } else if (c->size_policy_h == SIZE_PERCENT) {
            child_w = (cw * c->size_percent) / 100;
        }
        if (c->max_w > 0 && child_w > c->max_w) child_w = c->max_w;

        /* Recurse into container children */
        if (c->kind == WIDGET_VBOX || c->kind == WIDGET_HBOX) {
            if (c->kind == WIDGET_VBOX) layout_vbox(c, cx, cy, child_w, ch);
            else layout_hbox(c, cx, cy, child_w, ch);
        } else {
            c->x = cx;
            c->y = cy;
            c->w = child_w;
            c->h = ch;
            layout_widget_children(c);
        }

        cy += ch;
    }
}

/* Layout children of an HBOX: left-to-right, respecting size policies */
static void layout_hbox(Widget* container, int x, int y, int w, int h) {
    if (!container) return;
    container->x = x;
    container->y = y;
    container->w = w;
    container->h = h;

    int pad = container->padding;
    int cx = x + pad;
    int cy = y + pad;
    int cw = w - pad * 2;
    int ch = h - pad * 2;

    if (cw < 1) cw = 1;
    if (ch < 1) ch = 1;

    /* First pass: measure fixed/fit widths, count GROW weight */
    int fixed_total = 0;
    int grow_total_weight = 0;
    int visible_count = 0;
    int spacing_total = 0;

    for (Widget* c = container->first_child; c; c = c->next_sibling) {
        if (!c->visible) continue;
        if (visible_count > 0) {
            spacing_total += container->spacing > 0 ? container->spacing : RANDY_LAYOUT_SPACING;
        }
        visible_count++;

        if (c->size_policy_h == SIZE_GROW) {
            grow_total_weight += c->grow_weight > 0 ? c->grow_weight : 1;
        } else {
            int pw = c->pref_w > 0 ? c->pref_w : 80; /* default for FIT */
            if (c->min_w > 0 && pw < c->min_w) pw = c->min_w;
            if (c->max_w > 0 && pw > c->max_w) pw = c->max_w;
            fixed_total += pw;
        }
    }

    int remaining = cw - fixed_total - spacing_total;
    if (remaining < 0) remaining = 0;

    /* Second pass: position children */
    int count = 0;
    for (Widget* c = container->first_child; c; c = c->next_sibling) {
        if (!c->visible) continue;

        if (count > 0) {
            cx += container->spacing > 0 ? container->spacing : RANDY_LAYOUT_SPACING;
        }
        count++;

        int child_w;
        if (c->size_policy_h == SIZE_GROW) {
            int weight = c->grow_weight > 0 ? c->grow_weight : 1;
            child_w = (grow_total_weight > 0) ? (remaining * weight) / grow_total_weight : 0;
        } else if (c->size_policy_h == SIZE_PERCENT) {
            child_w = (cw * c->size_percent) / 100;
        } else {
            child_w = c->pref_w > 0 ? c->pref_w : 80;
        }
        if (c->min_w > 0 && child_w < c->min_w) child_w = c->min_w;
        if (c->max_w > 0 && child_w > c->max_w) child_w = c->max_w;

        int child_h = ch;
        if (c->size_policy_v == SIZE_FIXED && c->pref_h > 0) {
            child_h = c->pref_h;
        } else if (c->size_policy_v == SIZE_FIT) {
            child_h = resolve_height(c);
        }
        if (c->min_h > 0 && child_h < c->min_h) child_h = c->min_h;
        if (c->max_h > 0 && child_h > c->max_h) child_h = c->max_h;

        /* Recurse into container children */
        if (c->kind == WIDGET_VBOX || c->kind == WIDGET_HBOX) {
            if (c->kind == WIDGET_VBOX) layout_vbox(c, cx, cy, child_w, child_h);
            else layout_hbox(c, cx, cy, child_w, child_h);
        } else {
            c->x = cx;
            c->y = cy;
            c->w = child_w;
            c->h = child_h;
            layout_widget_children(c);
        }

        cx += child_w;
    }
}

/* =========================================================================
 * Container child layout dispatch
 * ========================================================================= */

static void layout_widget_children(Widget* w) {
    if (!w || !w->first_child) return;

    int cx, cy, cw, ch;
    bool horizontal = false;

    switch (w->kind) {
        case WIDGET_GROUPBOX:
            cx = w->x + 8;  cy = w->y + 16;
            cw = w->w - 16; ch = w->h - 24;
            break;
        case WIDGET_TAB_WIDGET:
            cx = w->x + 2;  cy = w->y + 24;
            cw = w->w - 4;  ch = w->h - 26;
            for (Widget* page = w->first_child; page; page = page->next_sibling) {
                if (!page->visible) continue;
                if (page->kind == WIDGET_VBOX)
                    layout_vbox(page, cx, cy, cw, ch);
                else if (page->kind == WIDGET_HBOX)
                    layout_hbox(page, cx, cy, cw, ch);
                else {
                    page->x = cx; page->y = cy;
                    page->w = cw; page->h = ch;
                    layout_widget_children(page);
                }
            }
            return;
        case WIDGET_TOOLBAR:
        case WIDGET_MENUBAR:
            horizontal = true;
            cx = w->x; cy = w->y;
            cw = w->w; ch = w->h;
            break;
        case WIDGET_DIALOG: {
            int title_h = g_style.dialog_title_bar_height;
            int pad = w->padding > 0 ? w->padding : 8;
            cx = w->x + pad; cy = w->y + title_h + pad;
            cw = w->w - pad * 2; ch = w->h - title_h - pad * 2;
            break;
        }
        case WIDGET_SCROLL_AREA:
            cx = w->x + 1;  cy = w->y + 1;
            cw = w->w - g_style.scrollbar_width - 2; ch = w->h - 2;
            break;
        case WIDGET_ACCORDION:
            if (!w->checked) return;
            cx = w->x + 1;  cy = w->y + g_style.accordion_header_height;
            cw = w->w - 2;  ch = w->h - g_style.accordion_header_height - 1;
            break;
        case WIDGET_STACKED:
            for (Widget* c = w->first_child; c; c = c->next_sibling) {
                if (!c->visible) continue;
                if (c->kind == WIDGET_VBOX)
                    layout_vbox(c, w->x, w->y, w->w, w->h);
                else if (c->kind == WIDGET_HBOX)
                    layout_hbox(c, w->x, w->y, w->w, w->h);
                else {
                    c->x = w->x; c->y = w->y;
                    c->w = w->w; c->h = w->h;
                    layout_widget_children(c);
                }
            }
            return;
        default:
            return;
    }

    if (cw < 1 || ch < 1) return;

    if (horizontal) {
        int sx = cx;
        int sp = w->spacing > 0 ? w->spacing : RANDY_LAYOUT_SPACING;
        int count = 0;
        for (Widget* c = w->first_child; c; c = c->next_sibling) {
            if (!c->visible) continue;
            if (count > 0) sx += sp;
            count++;
            int child_w = c->pref_w > 0 ? c->pref_w : 60;
            if (c->kind == WIDGET_VBOX)
                layout_vbox(c, sx, cy, child_w, ch);
            else if (c->kind == WIDGET_HBOX)
                layout_hbox(c, sx, cy, child_w, ch);
            else {
                c->x = sx; c->y = cy;
                c->w = child_w; c->h = ch;
                layout_widget_children(c);
            }
            sx += child_w;
        }
    } else {
        int sy = cy;
        int sp = w->spacing > 0 ? w->spacing : RANDY_LAYOUT_SPACING;
        int count = 0;
        for (Widget* c = w->first_child; c; c = c->next_sibling) {
            if (!c->visible) continue;
            if (count > 0) sy += sp;
            count++;
            int child_h = resolve_height(c);
            int child_w = cw;
            if (c->size_policy_h == SIZE_FIT && c->pref_w > 0)
                child_w = c->pref_w;
            if (c->kind == WIDGET_VBOX)
                layout_vbox(c, cx, sy, child_w, child_h);
            else if (c->kind == WIDGET_HBOX)
                layout_hbox(c, cx, sy, child_w, child_h);
            else {
                c->x = cx; c->y = sy;
                c->w = child_w; c->h = child_h;
                layout_widget_children(c);
            }
            sy += child_h;
        }
    }
}

void layout_widgets(RandyWindow* win) {
    if (!win || !win->needs_layout || !win->root) return;

    int fb_w = 0, fb_h = 0;
    platform_window_get_size(win->platform, &fb_w, &fb_h);
    if (fb_w <= 0 || fb_h <= 0) return;

    /* Root VBOX fills the window below the title bar */
    int content_x = RANDY_LAYOUT_PADDING_X;
    int content_y = RANDY_LAYOUT_PADDING_TOP;
    int content_w = fb_w - (RANDY_LAYOUT_PADDING_X * 2);
    int content_h = fb_h - RANDY_LAYOUT_PADDING_TOP - RANDY_LAYOUT_BOTTOM_MARGIN;
    if (content_w < 64) content_w = 64;
    if (content_h < 16) content_h = 16;

    layout_vbox(win->root, content_x, content_y, content_w, content_h);

    win->needs_layout = false;
}
