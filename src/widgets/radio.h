#ifndef WIDGET_RADIO_H
#define WIDGET_RADIO_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_radio_create(RandyosgWindow* win, const char* label,
                                          bool selected);
void             randyosgui_radio_set_selected(RandyosgWindow* win, RandyosgWidgetId id,
                                               bool selected);
bool             randyosgui_radio_get_selected(RandyosgWindow* win, RandyosgWidgetId id);
void             randyosgui_radio_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                               RandyosgToggleCallback cb, void* userdata);

/* Renderer — draw */
void draw_radio(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent);

#endif /* WIDGET_RADIO_H */
