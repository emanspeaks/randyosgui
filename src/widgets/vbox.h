#ifndef RANDY_WIDGET_VBOX_H
#define RANDY_WIDGET_VBOX_H

#include "../randyosgui_internal.h"

/* Public API is in randyosgui.h (randy_vbox_create, etc.) */

/* VBOX is a layout-only container Ã¢â‚¬â€ no draw function needed.
 * draw_vbox recurses into children. */
void draw_vbox(RendererContext* r, VkCommandBuffer cmd,
               const Widget* w, VkExtent2D extent,
               void (*draw_children_fn)(RendererContext*, VkCommandBuffer,
                                        Widget*, VkExtent2D));

#endif /* RANDY_WIDGET_VBOX_H */
