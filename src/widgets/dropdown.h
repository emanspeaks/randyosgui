#ifndef WIDGET_DROPDOWN_H
#define WIDGET_DROPDOWN_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_dropdown_create(RandyosgWindow* win, const char* value);
void             randyosgui_dropdown_set_value(RandyosgWindow* win, RandyosgWidgetId id,
                                               const char* value);

/* Renderer — draw */
void draw_dropdown(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_DROPDOWN_H */
