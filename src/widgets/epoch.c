#include "epoch.h"
#include "../renderer/renderer_private.h"
#include "../style.h"
#include <stdio.h>
#include <math.h>

/* =========================================================================
 * Calendar conversion helpers (Howard Hinnant civil_from_days algorithm)
 * Reference: https://howardhinnant.github.io/date_algorithms.html
 * ========================================================================= */

static void days_to_ymd(int days, int* y, int* m, int* d) {
    days += 719468; /* shift epoch from 1970-01-01 to 0000-03-01 */
    int era = (days >= 0 ? days : days - 146096) / 146097;
    int doe = days - era * 146097;
    int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    *y = yoe + era * 400;
    int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int mp  = (5 * doy + 2) / 153;
    *d = doy - (153 * mp + 2) / 5 + 1;
    *m = mp < 10 ? mp + 3 : mp - 9;
    if (*m <= 2) (*y)++;
}

static int ymd_to_days(int y, int m, int d) {
    if (m <= 2) y--;
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

static bool is_leap_year(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int day_of_year(int y, int m, int d) {
    /* Jan 1 = day 1 */
    int jan1 = ymd_to_days(y, 1, 1);
    int cur  = ymd_to_days(y, m, d);
    return cur - jan1 + 1;
}

/* =========================================================================
 * Decompose epoch_sec into field values for each format
 * ========================================================================= */

/* DHMS: fields[0]=days, [1]=hours, [2]=minutes, [3]=seconds */
static void epoch_to_dhms(double sec, int* fields) {
    int total = (int)sec;
    int neg   = total < 0;
    if (neg) total = -total;
    fields[0] = total / 86400;           total %= 86400;
    fields[1] = total / 3600;            total %= 3600;
    fields[2] = total / 60;
    fields[3] = total % 60;
    if (neg) fields[0] = -fields[0];
}

/* YMDHMS: fields[0]=year, [1]=month, [2]=day, [3]=hour, [4]=minute, [5]=second */
static void epoch_to_ymdhms(double sec, int* fields) {
    int total_sec = (int)sec;
    int days_part = total_sec / 86400;
    int rem       = total_sec - days_part * 86400;
    if (rem < 0) { days_part--; rem += 86400; }
    int y, m, d;
    days_to_ymd(days_part, &y, &m, &d);
    fields[0] = y;
    fields[1] = m;
    fields[2] = d;
    fields[3] = rem / 3600;  rem %= 3600;
    fields[4] = rem / 60;
    fields[5] = rem % 60;
}

/* Y:DOY:HMS: fields[0]=year, [1]=doy, [2]=hour, [3]=minute, [4]=second */
static void epoch_to_y_doy_hms(double sec, int* fields) {
    int total_sec = (int)sec;
    int days_part = total_sec / 86400;
    int rem       = total_sec - days_part * 86400;
    if (rem < 0) { days_part--; rem += 86400; }
    int y, m, d;
    days_to_ymd(days_part, &y, &m, &d);
    fields[0] = y;
    fields[1] = day_of_year(y, m, d);
    fields[2] = rem / 3600;  rem %= 3600;
    fields[3] = rem / 60;
    fields[4] = rem % 60;
}

/* =========================================================================
 * Field metadata per format
 * ========================================================================= */

typedef struct {
    int   count;               /* number of fields */
    int   text_w[6];           /* pixel width of text area */
    const char* unit[6];       /* unit suffix label */
    double increment[6];       /* seconds to add per click */
} EpochFormatInfo;

static const EpochFormatInfo fmt_dhms = {
    .count     = 4,
    .text_w    = { 44, 28, 28, 28 },
    .unit      = { "d", "h", "m", "s" },
    .increment = { 86400, 3600, 60, 1 },
};

static const EpochFormatInfo fmt_ymdhms = {
    .count     = 6,
    .text_w    = { 44, 28, 28, 28, 28, 28 },
    .unit      = { "y", "mo", "d", "h", "m", "s" },
    .increment = { 365.0 * 86400, 30.0 * 86400, 86400, 3600, 60, 1 },
};

static const EpochFormatInfo fmt_y_doy_hms = {
    .count     = 5,
    .text_w    = { 44, 36, 28, 28, 28 },
    .unit      = { "y", "doy", "h", "m", "s" },
    .increment = { 365.0 * 86400, 86400, 3600, 60, 1 },
};

static const EpochFormatInfo* get_format_info(int fmt) {
    switch (fmt) {
        case EPOCH_FMT_YMDHMS:    return &fmt_ymdhms;
        case EPOCH_FMT_Y_DOY_HMS: return &fmt_y_doy_hms;
        default:                  return &fmt_dhms;
    }
}

static void get_fields(int fmt, double epoch_sec, int* fields) {
    switch (fmt) {
        case EPOCH_FMT_YMDHMS:    epoch_to_ymdhms(epoch_sec, fields);    break;
        case EPOCH_FMT_Y_DOY_HMS: epoch_to_y_doy_hms(epoch_sec, fields); break;
        default:                  epoch_to_dhms(epoch_sec, fields);       break;
    }
}

/* =========================================================================
 * Geometry constants
 * ========================================================================= */

#define HEADER_H     20
#define EDITOR_H     21
#define ROW_GAP       4
#define CHK_SIZE     13
#define RADIO_SZ     13
#define BTN_W        16
#define FIELD_GAP     6
#define UNIT_GAP      2
#define RADIO_GAP    12
#define LABEL_GAP     8

/* =========================================================================
 * Public API
 * ========================================================================= */

RandyWidgetId randy_epoch_create(RandyWindow* win, const char* label,
                                          double initial_sec) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_EPOCH, label);
    if (!w) return 0;
    w->epoch_sec = initial_sec;
    w->value     = EPOCH_FMT_DHMS;  /* default format */
    w->checked   = true;            /* enabled by default */
    w->pref_h    = HEADER_H + ROW_GAP + EDITOR_H;
    return w->id;
}

void randy_epoch_set_value(RandyWindow* win, RandyWidgetId id, double sec) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_EPOCH) return;
    w->epoch_sec = sec;
    win->needs_render = true;
}

double randy_epoch_get_value(RandyWindow* win, RandyWidgetId id) {
    if (!win) return 0.0;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_EPOCH) return 0.0;
    return w->epoch_sec;
}

void randy_epoch_set_format(RandyWindow* win, RandyWidgetId id, int fmt) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_EPOCH) return;
    if (fmt < 0 || fmt > 2) return;
    w->value = fmt;
    win->needs_render = true;
}

int randy_epoch_get_format(RandyWindow* win, RandyWidgetId id) {
    if (!win) return 0;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_EPOCH) return 0;
    return w->value;
}

void randy_epoch_set_callback(RandyWindow* win, RandyWidgetId id,
                                    RandyValueCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_EPOCH) return;
    w->value_cb = cb;
    w->value_userdata = userdata;
}

/* =========================================================================
 * Drawing
 * ========================================================================= */

/* Draw a small radio circle at (cx, cy) with diameter sz */
static void draw_epoch_radio(VkCommandBuffer cmd, VkExtent2D extent,
                              int cx, int cy, int sz, bool selected) {
    /* Outer circle (border) */
    float br = g_style.button_shadow.r;
    float bg = g_style.button_shadow.g;
    float bb = g_style.button_shadow.b;

    /* White fill */
    draw_rect(cmd, extent, cx + 1, cy + 1, sz - 2, sz - 2,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);

    /* Border pixels — approximate circle with rectangles */
    draw_rect(cmd, extent, cx + 2, cy, sz - 4, 1, br, bg, bb);
    draw_rect(cmd, extent, cx + 2, cy + sz - 1, sz - 4, 1, br, bg, bb);
    draw_rect(cmd, extent, cx, cy + 2, 1, sz - 4, br, bg, bb);
    draw_rect(cmd, extent, cx + sz - 1, cy + 2, 1, sz - 4, br, bg, bb);
    draw_rect(cmd, extent, cx + 1, cy + 1, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, cx + sz - 2, cy + 1, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, cx + 1, cy + sz - 2, 1, 1, br, bg, bb);
    draw_rect(cmd, extent, cx + sz - 2, cy + sz - 2, 1, 1, br, bg, bb);

    /* Selected dot */
    if (selected) {
        int dot = 5;
        int dx = cx + (sz - dot) / 2;
        int dy = cy + (sz - dot) / 2;
        draw_rect(cmd, extent, dx + 1, dy, dot - 2, dot,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, dx, dy + 1, dot, dot - 2,
                  g_style.text.r, g_style.text.g, g_style.text.b);
    }
}

/* Draw a mini checkbox at (bx, by) */
static void draw_epoch_checkbox(VkCommandBuffer cmd, VkExtent2D extent,
                                 int bx, int by, int sz, bool checked) {
    draw_rect(cmd, extent, bx, by, sz, sz,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_bevel(cmd, extent, bx, by, sz, sz, true);

    if (checked) {
        /* Checkmark with small rectangles */
        int cx = bx + 3, cy = by + 5;
        draw_rect(cmd, extent, cx, cy, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, cx + 1, cy + 1, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, cx + 2, cy + 2, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, cx + 3, cy + 1, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, cx + 4, cy, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, cx + 5, cy - 1, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
        draw_rect(cmd, extent, cx + 6, cy - 2, 1, 3,
                  g_style.text.r, g_style.text.g, g_style.text.b);
    }
}

/* Draw a mini spinbox field: sunken text area + up/down buttons */
static void draw_epoch_spinbox(RendererContext* r, VkCommandBuffer cmd,
                                VkExtent2D extent,
                                int fx, int fy, int text_w, int h,
                                int field_val, bool enabled) {
    float tr, tg, tb;
    if (enabled) {
        tr = g_style.text.r; tg = g_style.text.g; tb = g_style.text.b;
    } else {
        tr = g_style.button_shadow.r; tg = g_style.button_shadow.g; tb = g_style.button_shadow.b;
    }

    /* Sunken text area */
    draw_rect(cmd, extent, fx, fy, text_w, h,
              g_style.button_highlight.r, g_style.button_highlight.g, g_style.button_highlight.b);
    draw_bevel(cmd, extent, fx, fy, text_w, h, true);

    /* Value text */
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", field_val);
    Widget tmp = {0};
    tmp.x = fx + 2;
    tmp.y = fy;
    tmp.w = text_w - 4;
    tmp.h = h;
    tmp.text = buf;
    tmp.visible = true;
    tmp.enabled = enabled;
    draw_widget_text(r, cmd, &tmp, extent, 2, tr, tg, tb,
                     g_style.button_highlight.r, g_style.button_highlight.g,
                     g_style.button_highlight.b);

    /* Up/down buttons */
    int bx = fx + text_w;
    int half_h = h / 2;

    /* Up button */
    draw_rect(cmd, extent, bx, fy, BTN_W, half_h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, bx, fy, BTN_W, half_h, false);
    int ax = bx + BTN_W / 2;
    int ay = fy + half_h / 2 - 1;
    draw_rect(cmd, extent, ax, ay, 1, 1, tr, tg, tb);
    draw_rect(cmd, extent, ax - 1, ay + 1, 3, 1, tr, tg, tb);
    draw_rect(cmd, extent, ax - 2, ay + 2, 5, 1, tr, tg, tb);

    /* Down button */
    int by = fy + half_h;
    int bh = h - half_h;
    draw_rect(cmd, extent, bx, by, BTN_W, bh,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, bx, by, BTN_W, bh, false);
    int dy = by + bh / 2 - 1;
    draw_rect(cmd, extent, ax - 2, dy, 5, 1, tr, tg, tb);
    draw_rect(cmd, extent, ax - 1, dy + 1, 3, 1, tr, tg, tb);
    draw_rect(cmd, extent, ax, dy + 2, 1, 1, tr, tg, tb);
}

void draw_epoch(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent) {
    if (!w || !w->visible) return;

    bool enabled = w->checked;
    int  fmt     = w->value;
    const EpochFormatInfo* fi = get_format_info(fmt);

    float tr, tg, tb;
    if (enabled) {
        tr = g_style.text.r; tg = g_style.text.g; tb = g_style.text.b;
    } else {
        tr = g_style.button_shadow.r; tg = g_style.button_shadow.g; tb = g_style.button_shadow.b;
    }

    /* --- Header row --- */
    int hx = w->x;
    int hy = w->y;

    /* Enable checkbox */
    int chk_y = hy + (HEADER_H - CHK_SIZE) / 2;
    draw_epoch_checkbox(cmd, extent, hx, chk_y, CHK_SIZE, enabled);
    hx += CHK_SIZE + 4;

    /* Label text */
    int label_w = w->text ? approx_text_px(w->text) : 0;
    if (w->text && label_w > 0) {
        Widget lbl = {0};
        lbl.x = hx; lbl.y = hy; lbl.w = label_w; lbl.h = HEADER_H;
        lbl.text = w->text; lbl.visible = true; lbl.enabled = true;
        draw_widget_text(r, cmd, &lbl, extent, 2,
                         tr, tg, tb,
                         g_style.surface.r, g_style.surface.g, g_style.surface.b);
        hx += label_w + LABEL_GAP;
    }

    /* Radio buttons for format selection */
    static const char* radio_labels[3] = { "DHMS", "YMDHMS", "Y:DOY:HMS" };
    for (int i = 0; i < 3; i++) {
        int ry = hy + (HEADER_H - RADIO_SZ) / 2;
        draw_epoch_radio(cmd, extent, hx, ry, RADIO_SZ, fmt == i);
        hx += RADIO_SZ + 3;

        int rlabel_w = approx_text_px(radio_labels[i]);
        Widget rlbl = {0};
        rlbl.x = hx; rlbl.y = hy; rlbl.w = rlabel_w; rlbl.h = HEADER_H;
        rlbl.text = (char*)radio_labels[i]; rlbl.visible = true; rlbl.enabled = true;
        draw_widget_text(r, cmd, &rlbl, extent, 2,
                         tr, tg, tb,
                         g_style.surface.r, g_style.surface.g, g_style.surface.b);
        hx += rlabel_w + RADIO_GAP;
    }

    /* --- Editor row --- */
    int fields[6];
    get_fields(fmt, w->epoch_sec, fields);

    int ey = w->y + HEADER_H + ROW_GAP;
    int ex = w->x;

    for (int i = 0; i < fi->count; i++) {
        int tw = fi->text_w[i];
        draw_epoch_spinbox(r, cmd, extent, ex, ey, tw, EDITOR_H,
                           fields[i], enabled);

        /* Unit label */
        int unit_x = ex + tw + BTN_W + UNIT_GAP;
        int unit_w = approx_text_px(fi->unit[i]);
        Widget ulbl = {0};
        ulbl.x = unit_x; ulbl.y = ey; ulbl.w = unit_w; ulbl.h = EDITOR_H;
        ulbl.text = (char*)fi->unit[i]; ulbl.visible = true; ulbl.enabled = true;
        draw_widget_text(r, cmd, &ulbl, extent, 2,
                         tr, tg, tb,
                         g_style.surface.r, g_style.surface.g, g_style.surface.b);

        ex = unit_x + unit_w + FIELD_GAP;
    }
}

/* =========================================================================
 * Input — sub-element hit testing
 * ========================================================================= */

void epoch_handle_click(Widget* w, double mx, double my) {
    if (!w || w->kind != WIDGET_EPOCH) return;

    int hy = w->y;

    /* --- Checkbox hit --- */
    int chk_y = hy + (HEADER_H - CHK_SIZE) / 2;
    if (mx >= w->x && mx < w->x + CHK_SIZE &&
        my >= chk_y && my < chk_y + CHK_SIZE) {
        w->checked = !w->checked;
        return;
    }

    if (!w->checked) return; /* rest of widget disabled */

    /* --- Radio button hits --- */
    int hx = w->x + CHK_SIZE + 4;
    int label_w = w->text ? approx_text_px(w->text) : 0;
    hx += label_w + LABEL_GAP;

    static const char* radio_labels[3] = { "DHMS", "YMDHMS", "Y:DOY:HMS" };
    for (int i = 0; i < 3; i++) {
        int ry = hy + (HEADER_H - RADIO_SZ) / 2;
        int rlabel_w = approx_text_px(radio_labels[i]);
        int total_w = RADIO_SZ + 3 + rlabel_w;
        if (mx >= hx && mx < hx + total_w &&
            my >= ry && my < ry + HEADER_H) {
            w->value = i;
            return;
        }
        hx += total_w + RADIO_GAP;
    }

    /* --- Spinbox field up/down button hits --- */
    int fmt = w->value;
    const EpochFormatInfo* fi = get_format_info(fmt);

    int ey = w->y + HEADER_H + ROW_GAP;
    int ex = w->x;

    for (int i = 0; i < fi->count; i++) {
        int tw = fi->text_w[i];
        int bx = ex + tw;  /* up/down button x */
        int half_h = EDITOR_H / 2;

        if (mx >= bx && mx < bx + BTN_W && my >= ey && my < ey + EDITOR_H) {
            if (my < ey + half_h) {
                /* Up button */
                w->epoch_sec += fi->increment[i];
            } else {
                /* Down button */
                w->epoch_sec -= fi->increment[i];
            }
            if (w->value_cb) {
                w->value_cb(w->id, (int)w->epoch_sec, w->value_userdata);
            }
            return;
        }

        int unit_w = approx_text_px(fi->unit[i]);
        ex = bx + BTN_W + UNIT_GAP + unit_w + FIELD_GAP;
    }
}
