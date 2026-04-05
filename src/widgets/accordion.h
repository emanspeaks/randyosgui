#ifndef WIDGET_ACCORDION_H
#define WIDGET_ACCORDION_H

#include "../randyosgui_internal.h"

/* Public API â€” collapsible section headers */
RandyWidgetId randy_accordion_create(RandyWindow* win, const char* label, bool expanded);
void             randy_accordion_set_expanded(RandyWindow* win, RandyWidgetId id,
                                                    bool expanded);
bool             randy_accordion_get_expanded(RandyWindow* win, RandyWidgetId id);

/* Renderer â€” draw */
void draw_accordion(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent);

#endif /* WIDGET_ACCORDION_H */
