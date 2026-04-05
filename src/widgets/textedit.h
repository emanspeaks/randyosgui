#ifndef WIDGET_TEXTEDIT_H
#define WIDGET_TEXTEDIT_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_textedit_create(RandyWindow* win, const char* text, bool readonly);
void             randy_textedit_set_text(RandyWindow* win, RandyWidgetId id,
                                               const char* text);

/* Renderer â€” draw */
void draw_textedit(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_TEXTEDIT_H */
