#ifndef WIDGET_PROGRESS_H
#define WIDGET_PROGRESS_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_progress_create(RandyosgWindow* win, const char* label,
                                             int max_value, int value);
void             randyosgui_progress_set_value(RandyosgWindow* win, RandyosgWidgetId id,
                                               int value);

/* Renderer — draw */
void draw_progress(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_PROGRESS_H */
