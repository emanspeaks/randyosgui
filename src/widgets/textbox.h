#ifndef WIDGET_TEXTBOX_H
#define WIDGET_TEXTBOX_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_textbox_create(RandyWindow* win, const char* value,
                                            bool readonly);
void             randy_textbox_set_value(RandyWindow* win, RandyWidgetId id,
                                              const char* value);

/* Renderer â€” draw */
void draw_textbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_TEXTBOX_H */
