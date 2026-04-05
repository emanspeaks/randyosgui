#ifndef WIDGET_DIALOG_H
#define WIDGET_DIALOG_H

#include "../randyosgui_internal.h"

/* Public API â€” modal dialog overlay */
RandyWidgetId randy_dialog_create(RandyWindow* win, const char* title);
void             randy_dialog_show(RandyWindow* win, RandyWidgetId id);
void             randy_dialog_hide(RandyWindow* win, RandyWidgetId id);

/* Renderer â€” draw */
void draw_dialog(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent);

#endif /* WIDGET_DIALOG_H */
