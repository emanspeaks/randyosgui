#include "vbox.h"

/* VBOX is a pure layout container — it has no visual representation.
 * Drawing is handled by recursing into its children via the draw dispatch. */

void draw_vbox(RendererContext* r, VkCommandBuffer cmd,
               const Widget* w, VkExtent2D extent,
               void (*draw_children_fn)(RendererContext*, VkCommandBuffer,
                                        Widget*, VkExtent2D)) {
    (void)r; (void)cmd; (void)extent;
    if (!w || !w->visible) return;
    if (draw_children_fn && w->first_child) {
        draw_children_fn(r, cmd, w->first_child, extent);
    }
}
