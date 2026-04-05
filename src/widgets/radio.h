#ifndef WIDGET_RADIO_H
#define WIDGET_RADIO_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_radio_create(RandyWindow* win, const char* label,
                                          bool selected);
void             randy_radio_set_selected(RandyWindow* win, RandyWidgetId id,
                                               bool selected);
bool             randy_radio_get_selected(RandyWindow* win, RandyWidgetId id);
void             randy_radio_set_callback(RandyWindow* win, RandyWidgetId id,
                                               RandyToggleCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_radio(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent);

#endif /* WIDGET_RADIO_H */
