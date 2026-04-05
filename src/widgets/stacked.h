#ifndef WIDGET_STACKED_H
#define WIDGET_STACKED_H

#include "../randyosgui_internal.h"

/* Public API â€” stacked container shows one child at a time */
RandyWidgetId randy_stacked_create(RandyWindow* win);
void             randy_stacked_set_current(RandyWindow* win,
                                                 RandyWidgetId id,
                                                 int index);
int              randy_stacked_get_current(RandyWindow* win,
                                                 RandyWidgetId id);

/* Renderer â€” draw */
void draw_stacked(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_STACKED_H */
