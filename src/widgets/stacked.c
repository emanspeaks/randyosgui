#include "stacked.h"

RandyWidgetId randy_stacked_create(RandyWindow* win) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_STACKED, NULL);
    if (!w) return 0;
    w->value = 0; /* current page index */
    w->size_policy_h = SIZE_GROW;
    w->size_policy_v = SIZE_GROW;
    w->pref_h = 100;
    return w->id;
}

void randy_stacked_set_current(RandyWindow* win,
                                     RandyWidgetId id,
                                     int index) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_STACKED) return;

    /* Count children, clamp index */
    int count = 0;
    for (Widget* c = w->first_child; c; c = c->next_sibling) count++;
    if (index < 0) index = 0;
    if (index >= count) index = count > 0 ? count - 1 : 0;

    w->value = index;

    /* Show only the current page */
    int i = 0;
    for (Widget* c = w->first_child; c; c = c->next_sibling) {
        c->visible = (i == index);
        i++;
    }
    win->needs_layout = true;
    win->needs_render = true;
}

int randy_stacked_get_current(RandyWindow* win,
                                    RandyWidgetId id) {
    if (!win) return 0;
    Widget* w = widget_find(win, id);
    return (w && w->kind == WIDGET_STACKED) ? w->value : 0;
}

void draw_stacked(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    (void)r; (void)cmd; (void)extent;
    /* Stacked widget is a pure layout container — only the visible child is drawn.
     * The draw dispatch in renderer_widgets.c handles recursing into visible children. */
    (void)w;
}
