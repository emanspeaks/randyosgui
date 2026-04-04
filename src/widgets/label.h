#ifndef WIDGET_LABEL_H
#define WIDGET_LABEL_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_label_create(RandyosgWindow* win, const char* text);
void             randyosgui_label_set_text(RandyosgWindow* win, RandyosgWidgetId id,
                                           const char* text);

/* Renderer — draw */
void draw_label(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent);

#endif /* WIDGET_LABEL_H */
