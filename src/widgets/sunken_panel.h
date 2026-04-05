#ifndef WIDGET_SUNKEN_PANEL_H
#define WIDGET_SUNKEN_PANEL_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_sunken_panel_create(RandyWindow* win, const char* text);

/* Renderer â€” draw */
void draw_sunken_panel(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent);

#endif /* WIDGET_SUNKEN_PANEL_H */
