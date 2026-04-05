#ifndef RANDY_WIDGET_HBOX_H
#define RANDY_WIDGET_HBOX_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* HBOX is a layout-only container â€” no draw function needed.
 * draw_hbox recurses into children. */
void draw_hbox(RendererContext* r, VkCommandBuffer cmd,
               const Widget* w, VkExtent2D extent,
               void (*draw_children_fn)(RendererContext*, VkCommandBuffer,
                                        Widget*, VkExtent2D));

#endif /* RANDY_WIDGET_HBOX_H */
