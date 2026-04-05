#ifndef WIDGET_LISTBOX_H
#define WIDGET_LISTBOX_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_listbox_create(RandyWindow* win, int num_items,
                                            const char* const* items, int selected);
void             randy_listbox_set_callback(RandyWindow* win, RandyWidgetId id,
                                                  RandyValueCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_listbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_LISTBOX_H */
