#ifndef WIDGET_EPOCH_H
#define WIDGET_EPOCH_H

#include "../randyosgui_internal.h"

/* Epoch time-format constants (stored in Widget.value) */
#define EPOCH_FMT_DHMS      0
#define EPOCH_FMT_YMDHMS    1
#define EPOCH_FMT_Y_DOY_HMS 2

/* Public API */
RandyWidgetId randy_epoch_create(RandyWindow* win, const char* label,
                                          double initial_sec);
void             randy_epoch_set_value(RandyWindow* win, RandyWidgetId id,
                                             double sec);
double           randy_epoch_get_value(RandyWindow* win, RandyWidgetId id);
void             randy_epoch_set_format(RandyWindow* win, RandyWidgetId id,
                                              int fmt);
int              randy_epoch_get_format(RandyWindow* win, RandyWidgetId id);
void             randy_epoch_set_callback(RandyWindow* win, RandyWidgetId id,
                                                RandyValueCallback cb,
                                                void* userdata);

/* Renderer — draw */
void draw_epoch(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent);

/* Input — sub-element click handling (called from input.c) */
void epoch_handle_click(Widget* w, double mx, double my);

#endif /* WIDGET_EPOCH_H */
