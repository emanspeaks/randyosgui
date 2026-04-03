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

#ifdef __cplusplus
}
#endif

#endif /* RANDYOSGUI_H */
