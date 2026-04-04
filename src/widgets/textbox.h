#ifndef WIDGET_TEXTBOX_H
#define WIDGET_TEXTBOX_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_textbox_create(RandyosgWindow* win, const char* value,
                                            bool readonly);
void             randyosgui_textbox_set_value(RandyosgWindow* win, RandyosgWidgetId id,
                                              const char* value);

/* Renderer — draw */
void draw_textbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_TEXTBOX_H */
