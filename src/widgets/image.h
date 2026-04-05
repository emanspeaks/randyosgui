#ifndef WIDGET_IMAGE_H
#define WIDGET_IMAGE_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_image_create(RandyWindow* win, int width, int height);

/* Renderer â€” draw (placeholder rectangle) */
void draw_image(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent);

#endif /* WIDGET_IMAGE_H */
