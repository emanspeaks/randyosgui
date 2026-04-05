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

    /* checkbox box fill */
    assert(has_op(ops, n, 10, 11, 13, 13, 0.98f, 0.98f, 0.98f));
    /* checkmark distinctive top-right pixel from 98.css checkmark.svg */
    assert(has_op(ops, n, 19, 14, 1, 1, 0.039f, 0.039f, 0.039f));
}

void drawspec_radio_exact_contrast(void) {
    Widget w = make_widget(WIDGET_RADIO);
    w.h = 16;
    w.checked = true;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 256, 256, ops, 512);
    assert(n > 0);

    /* expected dark arc from radio-border.svg (#000) */
    assert(has_op(ops, n, 14, 13, 4, 1, 0.0f, 0.0f, 0.0f));
    /* expected white rim */
    assert(has_op(ops, n, 20, 14, 1, 2, 1.0f, 1.0f, 1.0f));
    /* center dot */
    assert(has_op(ops, n, 14, 17, 4, 2, 0.039f, 0.039f, 0.039f));
}

void drawspec_status_field_border(void) {
    Widget w = make_widget(WIDGET_STATUS_FIELD);
    w.h = 20;
    w.w = 160;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 256, 256, ops, 512);
    assert(n > 0);

    /* top/left button-shadow */
    assert(has_op(ops, n, 10, 10, 160, 1, 0.502f, 0.502f, 0.502f));
    assert(has_op(ops, n, 10, 10, 1, 20, 0.502f, 0.502f, 0.502f));
    /* bottom/right button-face */
    assert(has_op(ops, n, 169, 10, 1, 20, 0.875f, 0.875f, 0.875f));
    assert(has_op(ops, n, 10, 29, 160, 1, 0.875f, 0.875f, 0.875f));
}

void drawspec_sunken_panel_border(void) {
    Widget w = make_widget(WIDGET_SUNKEN_PANEL);
    w.h = 64;
    w.w = 180;

    RandyDrawOp ops[512];
    size_t n = renderer_test_capture_widget_draw_ops(&w, 320, 320, ops, 512);
    assert(n > 0);

    /* sunken-panel-border.svg outer gray and white opposite edges */
    assert(has_op(ops, n, 10, 10, 179, 1, 0.502f, 0.502f, 0.502f));
    assert(has_op(ops, n, 189, 10, 1, 64, 1.0f, 1.0f, 1.0f));
}
