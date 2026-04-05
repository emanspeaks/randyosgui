#ifndef WIDGET_LABEL_H
#define WIDGET_LABEL_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_label_create(RandyWindow* win, const char* text);
void             randy_label_set_text(RandyWindow* win, RandyWidgetId id,
                                           const char* text);

/* Renderer â€” draw */
void draw_label(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent);

#endif /* WIDGET_LABEL_H */
