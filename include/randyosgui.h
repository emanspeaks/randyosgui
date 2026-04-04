#ifndef RANDYOSGUI_H
#define RANDYOSGUI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * randyosgui - cross-platform retained-mode GUI framework
 * C API — opaque handles, stable ABI
 * ========================================================================= */

/* Version */
#define RANDYOSGUI_VERSION_MAJOR 0
#define RANDYOSGUI_VERSION_MINOR 0
#define RANDYOSGUI_VERSION_PATCH 1

/* Export / import macros */
#if defined(_WIN32)
#  if defined(RANDYOSGUI_BUILD_SHARED)
#    define RANDYOSGUI_API __declspec(dllexport)
#  elif defined(RANDYOSGUI_USE_SHARED)
#    define RANDYOSGUI_API __declspec(dllimport)
#  else
#    define RANDYOSGUI_API
#  endif
#else
#  define RANDYOSGUI_API __attribute__((visibility("default")))
#endif

/* -------------------------------------------------------------------------
 * Opaque types
 * ---------------------------------------------------------------------- */

typedef struct RandyosgContext  RandyosgContext;
typedef struct RandyosgWindow   RandyosgWindow;
typedef struct RandyosgWidget   RandyosgWidget;
typedef uint32_t                RandyosgWidgetId;

/* -------------------------------------------------------------------------
 * Errors
 * ---------------------------------------------------------------------- */

typedef enum {
    RANDYOSG_OK                  = 0,
    RANDYOSG_ERR_NOMEM           = 1,
    RANDYOSG_ERR_PLATFORM        = 2,
    RANDYOSG_ERR_RENDERER        = 3,
    RANDYOSG_ERR_INVALID_HANDLE  = 4,
} RandyosgResult;

/* -------------------------------------------------------------------------
 * Context lifecycle
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgContext* randyosgui_init(void);
RANDYOSGUI_API void             randyosgui_shutdown(RandyosgContext* ctx);
RANDYOSGUI_API const char*      randyosgui_result_string(RandyosgResult result);

/* -------------------------------------------------------------------------
 * Window management
 * ---------------------------------------------------------------------- */

typedef struct {
    const char* title;
    int         width;
    int         height;
    bool        resizable;
    bool        decorated;
} RandyosgWindowDesc;

RANDYOSGUI_API RandyosgWindow* randyosgui_window_create(RandyosgContext* ctx,
                                                         const RandyosgWindowDesc* desc);
RANDYOSGUI_API void            randyosgui_window_destroy(RandyosgWindow* win);
RANDYOSGUI_API bool            randyosgui_window_should_close(RandyosgWindow* win);
RANDYOSGUI_API void            randyosgui_window_set_title(RandyosgWindow* win,
                                                            const char* title);
RANDYOSGUI_API void            randyosgui_window_get_size(RandyosgWindow* win,
                                                           int* width, int* height);

/* -------------------------------------------------------------------------
 * Main loop helpers
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API void randyosgui_poll_events(RandyosgContext* ctx);
RANDYOSGUI_API void randyosgui_render(RandyosgWindow* win);

/* -------------------------------------------------------------------------
 * Widgets — label
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_label_create(RandyosgWindow* win,
                                                          const char* text);
RANDYOSGUI_API void             randyosgui_label_set_text(RandyosgWindow* win,
                                                           RandyosgWidgetId id,
                                                           const char* text);

/* -------------------------------------------------------------------------
 * Widgets — button
 * ---------------------------------------------------------------------- */

typedef void (*RandyosgClickCallback)(RandyosgWidgetId id, void* userdata);

RANDYOSGUI_API RandyosgWidgetId randyosgui_button_create(RandyosgWindow* win,
                                                           const char* label);
RANDYOSGUI_API void             randyosgui_button_set_callback(RandyosgWindow* win,
                                                                RandyosgWidgetId id,
                                                                RandyosgClickCallback cb,
                                                                void* userdata);

/* -------------------------------------------------------------------------
 * Widgets — checkbox
 * ---------------------------------------------------------------------- */

typedef void (*RandyosgToggleCallback)(RandyosgWidgetId id, bool checked, void* userdata);
typedef void (*RandyosgValueCallback)(RandyosgWidgetId id, int value, void* userdata);

RANDYOSGUI_API RandyosgWidgetId randyosgui_checkbox_create(RandyosgWindow* win,
                                                            const char* label,
                                                            bool checked);
RANDYOSGUI_API void             randyosgui_checkbox_set_checked(RandyosgWindow* win,
                                                                 RandyosgWidgetId id,
                                                                 bool checked);
RANDYOSGUI_API bool             randyosgui_checkbox_get_checked(RandyosgWindow* win,
                                                                 RandyosgWidgetId id);
RANDYOSGUI_API void             randyosgui_checkbox_set_callback(RandyosgWindow* win,
                                                                  RandyosgWidgetId id,
                                                                  RandyosgToggleCallback cb,
                                                                  void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - radio
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_radio_create(RandyosgWindow* win,
                                                         const char* label,
                                                         bool selected);
RANDYOSGUI_API void             randyosgui_radio_set_selected(RandyosgWindow* win,
                                                               RandyosgWidgetId id,
                                                               bool selected);
RANDYOSGUI_API bool             randyosgui_radio_get_selected(RandyosgWindow* win,
                                                               RandyosgWidgetId id);
RANDYOSGUI_API void             randyosgui_radio_set_callback(RandyosgWindow* win,
                                                               RandyosgWidgetId id,
                                                               RandyosgToggleCallback cb,
                                                               void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - text box
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_textbox_create(RandyosgWindow* win,
                                                           const char* value,
                                                           bool readonly);
RANDYOSGUI_API void             randyosgui_textbox_set_value(RandyosgWindow* win,
                                                              RandyosgWidgetId id,
                                                              const char* value);

/* -------------------------------------------------------------------------
 * Widgets - dropdown
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_dropdown_create(RandyosgWindow* win,
                                                            const char* value);
RANDYOSGUI_API void             randyosgui_dropdown_set_value(RandyosgWindow* win,
                                                              RandyosgWidgetId id,
                                                              const char* value);

/* -------------------------------------------------------------------------
 * Widgets - slider
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_slider_create(RandyosgWindow* win,
                                                          const char* label,
                                                          int min_value,
                                                          int max_value,
                                                          int value);
RANDYOSGUI_API void             randyosgui_slider_set_value(RandyosgWindow* win,
                                                             RandyosgWidgetId id,
                                                             int value);
RANDYOSGUI_API int              randyosgui_slider_get_value(RandyosgWindow* win,
                                                             RandyosgWidgetId id);
RANDYOSGUI_API void             randyosgui_slider_set_callback(RandyosgWindow* win,
                                                                RandyosgWidgetId id,
                                                                RandyosgValueCallback cb,
                                                                void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - progress bar
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_progress_create(RandyosgWindow* win,
                                                            const char* label,
                                                            int max_value,
                                                            int value);
RANDYOSGUI_API void             randyosgui_progress_set_value(RandyosgWindow* win,
                                                               RandyosgWidgetId id,
                                                               int value);

/* -------------------------------------------------------------------------
 * Widgets - group box
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_groupbox_create(RandyosgWindow* win,
                                                            const char* title);

/* -------------------------------------------------------------------------
 * Widgets - tabs
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_tab_create(RandyosgWindow* win,
                                                       const char* label,
                                                       bool active);
RANDYOSGUI_API void             randyosgui_tab_set_active(RandyosgWindow* win,
                                                           RandyosgWidgetId id,
                                                           bool active);
RANDYOSGUI_API bool             randyosgui_tab_get_active(RandyosgWindow* win,
                                                           RandyosgWidgetId id);
RANDYOSGUI_API void             randyosgui_tab_set_callback(RandyosgWindow* win,
                                                             RandyosgWidgetId id,
                                                             RandyosgClickCallback cb,
                                                             void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - tree view (item rows)
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_tree_item_create(RandyosgWindow* win,
                                                             const char* label,
                                                             int depth,
                                                             bool expandable,
                                                             bool expanded);

/* -------------------------------------------------------------------------
 * Widgets - table view (header/rows)
 *
 * A table is a grid of cells.  Create one header row to define the column
 * labels and pixel widths, then append data rows with one string per cell.
 * Column count is fixed per header; subsequent rows must supply the same
 * number of cells.
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_table_header_create(RandyosgWindow* win,
                                                                int num_cols,
                                                                const char* const* labels,
                                                                const int* col_widths);
RANDYOSGUI_API RandyosgWidgetId randyosgui_table_row_create(RandyosgWindow* win,
                                                             int num_cells,
                                                             const char* const* cells,
                                                             bool selected);
RANDYOSGUI_API void             randyosgui_table_row_set_callback(RandyosgWindow* win,
                                                                   RandyosgWidgetId id,
                                                                   RandyosgClickCallback cb,
                                                                   void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - field borders / status fields / sunken panel
 * ---------------------------------------------------------------------- */

RANDYOSGUI_API RandyosgWidgetId randyosgui_field_border_create(RandyosgWindow* win,
                                                                const char* text,
                                                                bool disabled_style);
RANDYOSGUI_API RandyosgWidgetId randyosgui_status_field_create(RandyosgWindow* win,
                                                                const char* text);
RANDYOSGUI_API RandyosgWidgetId randyosgui_sunken_panel_create(RandyosgWindow* win,
                                                                const char* text);

#ifdef __cplusplus
}
#endif

#endif /* RANDYOSGUI_H */
