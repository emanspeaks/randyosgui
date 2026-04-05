#ifndef RANDY_H
#define RANDY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * randy - cross-platform retained-mode GUI framework
 * C API — opaque handles, stable ABI
 * ========================================================================= */

/* Version */
#define RANDY_VERSION_MAJOR 0
#define RANDY_VERSION_MINOR 0
#define RANDY_VERSION_PATCH 1

/* Export / import macros */
#if defined(_WIN32)
#  if defined(RANDY_BUILD_SHARED)
#    define RANDY_API __declspec(dllexport)
#  elif defined(RANDY_USE_SHARED)
#    define RANDY_API __declspec(dllimport)
#  else
#    define RANDY_API
#  endif
#else
#  define RANDY_API __attribute__((visibility("default")))
#endif

/* -------------------------------------------------------------------------
 * Opaque types
 * ---------------------------------------------------------------------- */

typedef struct RandyContext  RandyContext;
typedef struct RandyWindow   RandyWindow;
typedef struct RandyWidget   RandyWidget;
typedef uint32_t                RandyWidgetId;

/* -------------------------------------------------------------------------
 * Errors
 * ---------------------------------------------------------------------- */

typedef enum {
    RANDY_OK                  = 0,
    RANDY_ERR_NOMEM           = 1,
    RANDY_ERR_PLATFORM        = 2,
    RANDY_ERR_RENDERER        = 3,
    RANDY_ERR_INVALID_HANDLE  = 4,
} RandyResult;

/* -------------------------------------------------------------------------
 * Size policy — Clay-inspired layout sizing model
 * ---------------------------------------------------------------------- */

typedef enum {
    RANDY_SIZE_FIXED   = 0,  /**< exact pixel size */
    RANDY_SIZE_FIT     = 1,  /**< shrink to content */
    RANDY_SIZE_GROW    = 2,  /**< expand to fill available space */
    RANDY_SIZE_PERCENT = 3,  /**< percentage of parent container */
} RandySizePolicy;

/* -------------------------------------------------------------------------
 * Context lifecycle
 * ---------------------------------------------------------------------- */

RANDY_API RandyContext* randy_init(void);
RANDY_API void             randy_shutdown(RandyContext* ctx);
RANDY_API const char*      randy_result_string(RandyResult result);

/* -------------------------------------------------------------------------
 * Window management
 * ---------------------------------------------------------------------- */

typedef struct {
    const char* title;
    int         width;
    int         height;
    bool        resizable;
    bool        decorated;
} RandyWindowDesc;

RANDY_API RandyWindow* randy_window_create(RandyContext* ctx,
                                                         const RandyWindowDesc* desc);
RANDY_API void            randy_window_destroy(RandyWindow* win);
RANDY_API bool            randy_window_should_close(RandyWindow* win);
RANDY_API void            randy_window_set_title(RandyWindow* win,
                                                            const char* title);
RANDY_API void            randy_window_get_size(RandyWindow* win,
                                                           int* width, int* height);

/* -------------------------------------------------------------------------
 * Main loop helpers
 * ---------------------------------------------------------------------- */

RANDY_API void randy_poll_events(RandyContext* ctx);
RANDY_API void randy_render(RandyWindow* win);

/* -------------------------------------------------------------------------
 * Widgets — label
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_label_create(RandyWindow* win,
                                                          const char* text);
RANDY_API void             randy_label_set_text(RandyWindow* win,
                                                           RandyWidgetId id,
                                                           const char* text);

/* -------------------------------------------------------------------------
 * Widgets — button
 * ---------------------------------------------------------------------- */

typedef void (*RandyClickCallback)(RandyWidgetId id, void* userdata);

RANDY_API RandyWidgetId randy_button_create(RandyWindow* win,
                                                           const char* label);
RANDY_API void             randy_button_set_callback(RandyWindow* win,
                                                                RandyWidgetId id,
                                                                RandyClickCallback cb,
                                                                void* userdata);

/* -------------------------------------------------------------------------
 * Widgets — checkbox
 * ---------------------------------------------------------------------- */

typedef void (*RandyToggleCallback)(RandyWidgetId id, bool checked, void* userdata);
typedef void (*RandyValueCallback)(RandyWidgetId id, int value, void* userdata);

RANDY_API RandyWidgetId randy_checkbox_create(RandyWindow* win,
                                                            const char* label,
                                                            bool checked);
RANDY_API void             randy_checkbox_set_checked(RandyWindow* win,
                                                                 RandyWidgetId id,
                                                                 bool checked);
RANDY_API bool             randy_checkbox_get_checked(RandyWindow* win,
                                                                 RandyWidgetId id);
RANDY_API void             randy_checkbox_set_callback(RandyWindow* win,
                                                                  RandyWidgetId id,
                                                                  RandyToggleCallback cb,
                                                                  void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - radio
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_radio_create(RandyWindow* win,
                                                         const char* label,
                                                         bool selected);
RANDY_API void             randy_radio_set_selected(RandyWindow* win,
                                                               RandyWidgetId id,
                                                               bool selected);
RANDY_API bool             randy_radio_get_selected(RandyWindow* win,
                                                               RandyWidgetId id);
RANDY_API void             randy_radio_set_callback(RandyWindow* win,
                                                               RandyWidgetId id,
                                                               RandyToggleCallback cb,
                                                               void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - text box
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_textbox_create(RandyWindow* win,
                                                           const char* value,
                                                           bool readonly);
RANDY_API void             randy_textbox_set_value(RandyWindow* win,
                                                              RandyWidgetId id,
                                                              const char* value);

/* -------------------------------------------------------------------------
 * Widgets - dropdown
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_dropdown_create(RandyWindow* win,
                                                            const char* value);
RANDY_API void             randy_dropdown_set_value(RandyWindow* win,
                                                              RandyWidgetId id,
                                                              const char* value);

/* -------------------------------------------------------------------------
 * Widgets - slider
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_slider_create(RandyWindow* win,
                                                          const char* label,
                                                          int min_value,
                                                          int max_value,
                                                          int value);
RANDY_API void             randy_slider_set_value(RandyWindow* win,
                                                             RandyWidgetId id,
                                                             int value);
RANDY_API int              randy_slider_get_value(RandyWindow* win,
                                                             RandyWidgetId id);
RANDY_API void             randy_slider_set_callback(RandyWindow* win,
                                                                RandyWidgetId id,
                                                                RandyValueCallback cb,
                                                                void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - progress bar
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_progress_create(RandyWindow* win,
                                                            const char* label,
                                                            int max_value,
                                                            int value);
RANDY_API void             randy_progress_set_value(RandyWindow* win,
                                                               RandyWidgetId id,
                                                               int value);

/* -------------------------------------------------------------------------
 * Widgets - group box
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_groupbox_create(RandyWindow* win,
                                                            const char* title);

/* -------------------------------------------------------------------------
 * Widgets - tabs
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_tab_create(RandyWindow* win,
                                                       const char* label,
                                                       bool active);
RANDY_API void             randy_tab_set_active(RandyWindow* win,
                                                           RandyWidgetId id,
                                                           bool active);
RANDY_API bool             randy_tab_get_active(RandyWindow* win,
                                                           RandyWidgetId id);
RANDY_API void             randy_tab_set_callback(RandyWindow* win,
                                                             RandyWidgetId id,
                                                             RandyClickCallback cb,
                                                             void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - tree view (item rows)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_tree_item_create(RandyWindow* win,
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

RANDY_API RandyWidgetId randy_table_header_create(RandyWindow* win,
                                                                int num_cols,
                                                                const char* const* labels,
                                                                const int* col_widths);
RANDY_API RandyWidgetId randy_table_row_create(RandyWindow* win,
                                                             int num_cells,
                                                             const char* const* cells,
                                                             bool selected);
RANDY_API void             randy_table_row_set_callback(RandyWindow* win,
                                                                   RandyWidgetId id,
                                                                   RandyClickCallback cb,
                                                                   void* userdata);

/* -------------------------------------------------------------------------
 * Widgets - field borders / status fields / sunken panel
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_field_border_create(RandyWindow* win,
                                                                const char* text,
                                                                bool disabled_style);
RANDY_API RandyWidgetId randy_status_field_create(RandyWindow* win,
                                                                const char* text);
RANDY_API RandyWidgetId randy_sunken_panel_create(RandyWindow* win,
                                                                const char* text);

/* -------------------------------------------------------------------------
 * Layout containers — VBOX / HBOX
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_vbox_create(RandyWindow* win);
RANDY_API RandyWidgetId randy_hbox_create(RandyWindow* win);

RANDY_API void             randy_container_set_spacing(RandyWindow* win,
                                                                  RandyWidgetId id,
                                                                  int spacing);
RANDY_API void             randy_container_set_padding(RandyWindow* win,
                                                                  RandyWidgetId id,
                                                                  int padding);

/* -------------------------------------------------------------------------
 * Widget tree management
 * ---------------------------------------------------------------------- */

RANDY_API void             randy_widget_add_child(RandyWindow* win,
                                                             RandyWidgetId parent_id,
                                                             RandyWidgetId child_id);

/* -------------------------------------------------------------------------
 * Widget common properties — size policy, visibility, enabled
 * ---------------------------------------------------------------------- */

RANDY_API void             randy_widget_set_size_policy(RandyWindow* win,
                                                                   RandyWidgetId id,
                                                                   RandySizePolicy h,
                                                                   RandySizePolicy v);
RANDY_API void             randy_widget_set_visible(RandyWindow* win,
                                                               RandyWidgetId id,
                                                               bool visible);
RANDY_API bool             randy_widget_get_visible(RandyWindow* win,
                                                               RandyWidgetId id);
RANDY_API void             randy_widget_set_enabled(RandyWindow* win,
                                                               RandyWidgetId id,
                                                               bool enabled);
RANDY_API bool             randy_widget_get_enabled(RandyWindow* win,
                                                               RandyWidgetId id);

/* -------------------------------------------------------------------------
 * Widgets — separator
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_separator_create(RandyWindow* win);

/* -------------------------------------------------------------------------
 * Widgets — spinbox (numeric up/down)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_spinbox_create(RandyWindow* win,
                                                           int min_val, int max_val,
                                                           int initial);
RANDY_API void             randy_spinbox_set_value(RandyWindow* win,
                                                              RandyWidgetId id,
                                                              int value);
RANDY_API int              randy_spinbox_get_value(RandyWindow* win,
                                                              RandyWidgetId id);
RANDY_API void             randy_spinbox_set_callback(RandyWindow* win,
                                                                 RandyWidgetId id,
                                                                 RandyValueCallback cb,
                                                                 void* userdata);

/* -------------------------------------------------------------------------
 * Widgets — combobox
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_combobox_create(RandyWindow* win,
                                                            const char* selected_text);
RANDY_API void             randy_combobox_set_callback(RandyWindow* win,
                                                                  RandyWidgetId id,
                                                                  RandyClickCallback cb,
                                                                  void* userdata);

/* -------------------------------------------------------------------------
 * Widgets — multi-line text edit
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_textedit_create(RandyWindow* win,
                                                            const char* text,
                                                            bool readonly);
RANDY_API void             randy_textedit_set_text(RandyWindow* win,
                                                              RandyWidgetId id,
                                                              const char* text);

/* -------------------------------------------------------------------------
 * Widgets — listbox
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_listbox_create(RandyWindow* win,
                                                           int num_items,
                                                           const char* const* items,
                                                           int selected);
RANDY_API void             randy_listbox_set_callback(RandyWindow* win,
                                                                 RandyWidgetId id,
                                                                 RandyValueCallback cb,
                                                                 void* userdata);

/* -------------------------------------------------------------------------
 * Widgets — menubar & menu items
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_menubar_create(RandyWindow* win);
RANDY_API RandyWidgetId randy_menu_item_create(RandyWindow* win,
                                                             const char* label);
RANDY_API void             randy_menu_item_set_callback(RandyWindow* win,
                                                                   RandyWidgetId id,
                                                                   RandyClickCallback cb,
                                                                   void* userdata);

/* -------------------------------------------------------------------------
 * Widgets — toolbar
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_toolbar_create(RandyWindow* win);

/* -------------------------------------------------------------------------
 * Widgets — image placeholder
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_image_create(RandyWindow* win,
                                                         int width, int height);

/* -------------------------------------------------------------------------
 * Widgets — scroll area
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_scroll_area_create(RandyWindow* win);

/* -------------------------------------------------------------------------
 * Widgets — stacked container (shows one child at a time)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_stacked_create(RandyWindow* win);
RANDY_API void             randy_stacked_set_current(RandyWindow* win,
                                                                RandyWidgetId id,
                                                                int index);
RANDY_API int              randy_stacked_get_current(RandyWindow* win,
                                                                RandyWidgetId id);

/* -------------------------------------------------------------------------
 * Widgets — compound tab widget (tab bar + stacked pages)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_tab_widget_create(RandyWindow* win);
RANDY_API RandyWidgetId randy_tab_widget_add_page(RandyWindow* win,
                                                                RandyWidgetId tab_widget_id,
                                                                const char* label);
RANDY_API void             randy_tab_widget_set_current(RandyWindow* win,
                                                                   RandyWidgetId id,
                                                                   int page_index);

/* -------------------------------------------------------------------------
 * Widgets — accordion (collapsible sections)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_accordion_create(RandyWindow* win,
                                                             const char* label,
                                                             bool expanded);
RANDY_API void             randy_accordion_set_expanded(RandyWindow* win,
                                                                   RandyWidgetId id,
                                                                   bool expanded);
RANDY_API bool             randy_accordion_get_expanded(RandyWindow* win,
                                                                   RandyWidgetId id);

/* -------------------------------------------------------------------------
 * Widgets — tooltip (starts hidden)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_tooltip_create(RandyWindow* win,
                                                           const char* text);

/* -------------------------------------------------------------------------
 * Widgets — dialog (modal overlay, starts hidden)
 * ---------------------------------------------------------------------- */

RANDY_API RandyWidgetId randy_dialog_create(RandyWindow* win,
                                                          const char* title);
RANDY_API void             randy_dialog_show(RandyWindow* win,
                                                        RandyWidgetId id);
RANDY_API void             randy_dialog_hide(RandyWindow* win,
                                                        RandyWidgetId id);

/* -------------------------------------------------------------------------
 * Style / Palette API — JSONC theme system
 *
 * The library ships with Win98-classic defaults.  Apps can optionally load
 * a JSONC style file or mutate the global style struct at runtime.
 * ---------------------------------------------------------------------- */

typedef struct {
    float r, g, b;
} RandyColor;

typedef struct {
    /* Named colour roles */
    RandyColor text;
    RandyColor surface;
    RandyColor button_face;
    RandyColor button_highlight;
    RandyColor button_shadow;
    RandyColor window_frame;
    RandyColor highlight;
    RandyColor highlight_text;
    RandyColor button_hover;
    RandyColor input_background;
    RandyColor input_border_hover;
    RandyColor tooltip_background;
    RandyColor tooltip_text;

    /* Font paths */
    char font_sans_path[256];
    char font_mono_path[256];

    /* Metrics (pixels) */
    int font_size_px;

    /* Window chrome */
    int window_border_width;
    int title_bar_height;
    int content_padding_x;
    int content_padding_top;
    int content_bottom_margin;

    /* Layout */
    int default_spacing;

    /* Widget heights */
    int label_height;
    int button_height;
    int checkbox_height;
    int textbox_height;
    int dropdown_height;
    int slider_height;
    int progress_height;
    int groupbox_height;
    int tab_height;
    int tab_width;
    int tree_item_height;
    int table_header_height;
    int table_row_height;
    int field_border_height;
    int status_field_height;
    int sunken_panel_height;
    int separator_height;
    int spinbox_height;
    int combobox_height;
    int textedit_height;
    int listbox_height;
    int menubar_height;
    int toolbar_height;
    int image_default_size;
    int scroll_area_height;
    int stacked_height;
    int tab_widget_height;
    int accordion_header_height;
    int tooltip_height;

    /* Control sub-element sizes */
    int checkbox_box_size;
    int radio_diameter;
    int spinbox_button_width;
    int combobox_button_width;
    int scrollbar_width;

    /* Dialog sub-metrics */
    int dialog_default_width;
    int dialog_default_height;
    int dialog_title_bar_height;
    int dialog_close_button_size;
    int dialog_padding;
} RandyStyle;

/** Reset a style struct to the built-in Win98 defaults. */
RANDY_API void             randy_style_set_defaults(RandyStyle* style);

/** Load colours/metrics from a JSONC file into the given style.
 *  Fields not present in the file keep their current values. */
RANDY_API int              randy_style_load(RandyStyle* style,
                                                       const char* path);

/** Write the given style out as a JSONC file. */
RANDY_API int              randy_style_save(const RandyStyle* style,
                                                       const char* path);

/** Get a pointer to the active global style (read/write). */
RANDY_API RandyStyle*   randy_style_get_active(void);

#ifdef __cplusplus
}
#endif

#endif /* RANDY_H */
