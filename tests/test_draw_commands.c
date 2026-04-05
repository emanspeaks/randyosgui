#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "randyosgui_internal.h"

static Widget make_widget(WidgetKind kind) {
    Widget w;
    memset(&w, 0, sizeof(w));
    w.kind = kind;
    w.x = 10;
    w.y = 10;
    w.w = 120;
    w.h = 24;
    w.text = "x";
    w.visible = true;
    w.enabled = true;
    return w;
}

static bool has_op(const RandyDrawOp* ops, size_t n,
                   int x, int y, int w, int h,
                   float r, float g, float b) {
    for (size_t i = 0; i < n; i++) {
        if (ops[i].x == x && ops[i].y == y && ops[i].w == w && ops[i].h == h &&
            ops[i].r == r && ops[i].g == g && ops[i].b == b) {
            return true;
        }
    }
    return false;
}

void drawspec_checkbox_exact(void) {
    Widget w = make_widget(WIDGET_CHECKBOX);
    w.h = 16;
    w.checked = true;
    w.hovered = false;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 256, 256, ops, 512);
    assert(n > 0);

    /* Dark box fill (input_background #313131) */
    assert(has_op(ops, n, 10, 11, 13, 13, 0.192f, 0.192f, 0.192f));
    /* Sunken bevel: outer dark on top (window_frame #2B2B2B, w-1=12) */
    assert(has_op(ops, n, 10, 11, 12, 1, 0.169f, 0.169f, 0.169f));
    /* Sunken bevel: inner shadow on top (button_shadow #010409, w-3=10) */
    assert(has_op(ops, n, 11, 12, 10, 1, 0.004f, 0.016f, 0.035f));
    /* Accent-colored checkmark leftmost stroke */
    assert(has_op(ops, n, 13, 17, 1, 2, 0.000f, 0.471f, 0.831f));
}

void drawspec_radio_exact_contrast(void) {
    Widget w = make_widget(WIDGET_RADIO);
    w.h = 16;
    w.checked = true;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 256, 256, ops, 512);
    assert(n > 0);

    /* Border arc top (button_shadow #010409) */
    assert(has_op(ops, n, 14, 11, 5, 1, 0.004f, 0.016f, 0.035f));
    /* Dark fill center (input_background #313131) */
    assert(has_op(ops, n, 11, 14, 11, 7, 0.192f, 0.192f, 0.192f));
    /* Accent-colored center dot */
    assert(has_op(ops, n, 14, 16, 5, 3, 0.000f, 0.471f, 0.831f));
}

void drawspec_status_field_border(void) {
    Widget w = make_widget(WIDGET_STATUS_FIELD);
    w.h = 20;
    w.w = 160;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 256, 256, ops, 512);
    assert(n > 0);

    /* Surface fill (#181818) */
    assert(has_op(ops, n, 10, 10, 160, 20, 0.094f, 0.094f, 0.094f));
    /* Sunken bevel: outer dark on top (window_frame #2B2B2B, w-1=159) */
    assert(has_op(ops, n, 10, 10, 159, 1, 0.169f, 0.169f, 0.169f));
    /* Sunken bevel: inner shadow on top (button_shadow #010409, w-3=157) */
    assert(has_op(ops, n, 11, 11, 157, 1, 0.004f, 0.016f, 0.035f));
    /* Sunken bevel: highlight on bottom (#3C3C3C) */
    assert(has_op(ops, n, 10, 29, 160, 1, 0.235f, 0.235f, 0.235f));
}

void drawspec_sunken_panel_border(void) {
    Widget w = make_widget(WIDGET_SUNKEN_PANEL);
    w.h = 64;
    w.w = 180;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 320, 320, ops, 512);
    assert(n > 0);

    /* Input_background fill (#313131) */
    assert(has_op(ops, n, 10, 10, 180, 64, 0.192f, 0.192f, 0.192f));
    /* Sunken bevel: outer dark on top (window_frame #2B2B2B, w-1=179) */
    assert(has_op(ops, n, 10, 10, 179, 1, 0.169f, 0.169f, 0.169f));
    /* Sunken bevel: inner shadow on top (button_shadow #010409, w-3=177) */
    assert(has_op(ops, n, 11, 11, 177, 1, 0.004f, 0.016f, 0.035f));
    /* Sunken bevel: highlight on right (#3C3C3C) */
    assert(has_op(ops, n, 189, 10, 1, 64, 0.235f, 0.235f, 0.235f));
}
