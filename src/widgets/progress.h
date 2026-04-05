#ifndef WIDGET_PROGRESS_H
#define WIDGET_PROGRESS_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_progress_create(RandyWindow* win, const char* label,
                                             int max_value, int value);
void             randy_progress_set_value(RandyWindow* win, RandyWidgetId id,
                                               int value);

/* Renderer â€” draw */
void draw_progress(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_PROGRESS_H */
