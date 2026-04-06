#include "style.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

/* =========================================================================
 * Global active style (initialised to Win98 defaults at startup)
 * ========================================================================= */

static RandyStyle s_default_style;
static bool s_defaults_initialised = false;

static void style_set_defaults_impl(RandyStyle* s);

static void ensure_defaults(void) {
    if (!s_defaults_initialised) {
        style_set_defaults_impl(&s_default_style);
        s_defaults_initialised = true;
    }
}

/* This is the one global instance every renderer file reads from. */
RandyStyle g_style;

__attribute__((constructor))
static void style_global_init(void) {
    style_set_defaults_impl(&g_style);
}

/* =========================================================================
 * Dark Modern defaults (VS Code Dark Modern-inspired)
 * ========================================================================= */

static void style_set_defaults_impl(RandyStyle* s) {
    if (!s) return;

    /* Colors — VS Code Dark Modern palette */
    s->text               = (RandyColor){ 0.800f, 0.800f, 0.800f };  /* #CCCCCC  foreground           */
    s->surface            = (RandyColor){ 0.094f, 0.094f, 0.094f };  /* #181818  sideBar.background    */
    s->button_face        = (RandyColor){ 0.192f, 0.192f, 0.192f };  /* #313131  checkbox.background   */
    s->button_highlight   = (RandyColor){ 0.235f, 0.235f, 0.235f };  /* #3C3C3C  checkbox.border       */
    s->button_shadow      = (RandyColor){ 0.004f, 0.016f, 0.035f };  /* #010409  overview ruler border */
    s->window_frame       = (RandyColor){ 0.169f, 0.169f, 0.169f };  /* #2B2B2B  panel.border          */
    s->highlight          = (RandyColor){ 0.000f, 0.471f, 0.831f };  /* #0078D4  focusBorder           */
    s->highlight_text     = (RandyColor){ 1.000f, 1.000f, 1.000f };  /* #FFFFFF  button.foreground     */
    s->button_hover       = (RandyColor){ 0.012f, 0.431f, 0.757f };  /* #026EC1  button.hoverBackground*/
    s->input_background   = (RandyColor){ 0.192f, 0.192f, 0.192f };  /* #313131  input.background      */
    s->input_border_hover = (RandyColor){ 0.000f, 0.471f, 0.831f };  /* #0078D4  focusBorder           */
    s->tooltip_background = (RandyColor){ 0.125f, 0.125f, 0.125f };  /* #202020  editorWidget.bg       */
    s->tooltip_text       = (RandyColor){ 0.800f, 0.800f, 0.800f };  /* #CCCCCC  foreground            */

    /* Font paths */
    snprintf(s->font_sans_path, sizeof(s->font_sans_path), "%s",
             "C:/Windows/Fonts/tahoma.ttf");
    snprintf(s->font_mono_path, sizeof(s->font_mono_path), "%s",
             "third_party/fonts/atkinsonhyperlegiblemono/AtkynsonMonoNerdFontMono-Regular.otf");

    /* Metrics */
    s->font_size_px           = 12;

    s->window_border_width    = 2;
    s->title_bar_height       = 24;
    s->content_padding_x      = 8;
    s->content_padding_top    = 30;
    s->content_bottom_margin  = 8;

    s->default_spacing        = 6;

    s->label_height           = 18;
    s->button_height          = 23;
    s->checkbox_height        = 16;
    s->textbox_height         = 21;
    s->dropdown_height        = 21;
    s->slider_height          = 24;
    s->progress_height        = 20;
    s->groupbox_height        = 54;
    s->tab_height             = 22;
    s->tab_width              = 96;
    s->tree_item_height       = 16;
    s->table_header_height    = 17;
    s->table_row_height       = 14;
    s->field_border_height    = 22;
    s->status_field_height    = 20;
    s->sunken_panel_height    = 64;
    s->separator_height       = 2;
    s->spinbox_height         = 21;
    s->combobox_height        = 21;
    s->menubar_height         = 20;
    s->toolbar_height         = 26;
    s->textedit_height        = 64;
    s->listbox_height         = 80;
    s->image_default_size     = 64;
    s->tooltip_height         = 20;
    s->epoch_height            = 45;
    s->accordion_header_height = 22;
    s->scroll_area_height     = 100;
    s->stacked_height         = 120;
    s->tab_widget_height      = 120;

    s->checkbox_box_size      = 13;
    s->radio_diameter         = 12;
    s->spinbox_button_width   = 16;
    s->combobox_button_width  = 17;
    s->scrollbar_width        = 16;
    s->dialog_default_width   = 300;
    s->dialog_default_height  = 180;
    s->dialog_title_bar_height = 18;
    s->dialog_close_button_size = 14;
    s->dialog_padding         = 8;
}

// =========================================================================
// Minimal JSONC parser
//
// Supports:
//   - // line comments
//   - block comments (no nesting)
//   - Quoted string keys
//   - Number values (int and float)
//   - String values ("#RRGGBB" hex colors)
//   - Nested objects (one level for "colors" / "metrics")
//   - Trailing commas
//
// Does NOT attempt to be a general JSON parser.  It targets our specific
// style schema only.
// =========================================================================

/* Read entire file into a malloc'd buffer. Returns NULL on failure. */
static char* read_file_to_string(const char* path, size_t* out_len) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }

    char* buf = (char*)malloc((size_t)len + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t read_count = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[read_count] = '\0';
    if (out_len) *out_len = read_count;
    return buf;
}

/* Skip whitespace and comments */
static const char* skip_ws(const char* p) {
    while (*p) {
        if (isspace((unsigned char)*p)) { p++; continue; }
        if (p[0] == '/' && p[1] == '/') {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }
        if (p[0] == '/' && p[1] == '*') {
            p += 2;
            while (*p && !(p[0] == '*' && p[1] == '/')) p++;
            if (*p) p += 2;
            continue;
        }
        break;
    }
    return p;
}

/* Parse a quoted string.  Returns pointer past closing quote, or NULL
 * on error.  Writes key into `buf` (up to buf_size-1 chars). */
static const char* parse_string(const char* p, char* buf, size_t buf_size) {
    if (*p != '"') return NULL;
    p++;
    size_t i = 0;
    while (*p && *p != '"') {
        if (*p == '\\' && p[1]) {
            p++;
        }
        if (i < buf_size - 1) buf[i++] = *p;
        p++;
    }
    buf[i] = '\0';
    if (*p == '"') p++;
    return p;
}

/* Parse a hex color string "#RRGGBB" into an RandyColor */
static bool parse_hex_color(const char* hex, RandyColor* out) {
    if (!hex || hex[0] != '#' || strlen(hex) != 7) return false;
    unsigned int r, g, b;
    if (sscanf(hex + 1, "%02x%02x%02x", &r, &g, &b) != 3) return false;
    out->r = (float)r / 255.0f;
    out->g = (float)g / 255.0f;
    out->b = (float)b / 255.0f;
    return true;
}

/* Parse a numeric value (int or float).  Returns pointer past the number */
static const char* parse_number(const char* p, double* out) {
    char* end = NULL;
    *out = strtod(p, &end);
    return (end && end > p) ? end : NULL;
}

/* Color name offset mapping */
typedef struct { const char* name; size_t offset; } ColorEntry;
typedef struct { const char* name; size_t offset; } MetricEntry;

#define COLOR_ENTRY(field) { #field, offsetof(RandyStyle, field) }
#define METRIC_ENTRY(field) { #field, offsetof(RandyStyle, field) }

static const ColorEntry s_colors[] = {
    COLOR_ENTRY(text),
    COLOR_ENTRY(surface),
    COLOR_ENTRY(button_face),
    COLOR_ENTRY(button_highlight),
    COLOR_ENTRY(button_shadow),
    COLOR_ENTRY(window_frame),
    COLOR_ENTRY(highlight),
    COLOR_ENTRY(highlight_text),
    COLOR_ENTRY(button_hover),
    COLOR_ENTRY(input_background),
    COLOR_ENTRY(input_border_hover),
    COLOR_ENTRY(tooltip_background),
    COLOR_ENTRY(tooltip_text),
};
static const size_t s_num_colors = sizeof(s_colors) / sizeof(s_colors[0]);

static const MetricEntry s_metrics[] = {
    METRIC_ENTRY(font_size_px),
    METRIC_ENTRY(window_border_width),
    METRIC_ENTRY(title_bar_height),
    METRIC_ENTRY(content_padding_x),
    METRIC_ENTRY(content_padding_top),
    METRIC_ENTRY(content_bottom_margin),
    METRIC_ENTRY(default_spacing),
    METRIC_ENTRY(label_height),
    METRIC_ENTRY(button_height),
    METRIC_ENTRY(checkbox_height),
    METRIC_ENTRY(textbox_height),
    METRIC_ENTRY(dropdown_height),
    METRIC_ENTRY(slider_height),
    METRIC_ENTRY(progress_height),
    METRIC_ENTRY(groupbox_height),
    METRIC_ENTRY(tab_height),
    METRIC_ENTRY(tab_width),
    METRIC_ENTRY(tree_item_height),
    METRIC_ENTRY(table_header_height),
    METRIC_ENTRY(table_row_height),
    METRIC_ENTRY(field_border_height),
    METRIC_ENTRY(status_field_height),
    METRIC_ENTRY(sunken_panel_height),
    METRIC_ENTRY(separator_height),
    METRIC_ENTRY(spinbox_height),
    METRIC_ENTRY(combobox_height),
    METRIC_ENTRY(menubar_height),
    METRIC_ENTRY(toolbar_height),
    METRIC_ENTRY(textedit_height),
    METRIC_ENTRY(listbox_height),
    METRIC_ENTRY(image_default_size),
    METRIC_ENTRY(tooltip_height),
    METRIC_ENTRY(accordion_header_height),
    METRIC_ENTRY(scroll_area_height),
    METRIC_ENTRY(stacked_height),
    METRIC_ENTRY(tab_widget_height),
    METRIC_ENTRY(checkbox_box_size),
    METRIC_ENTRY(radio_diameter),
    METRIC_ENTRY(spinbox_button_width),
    METRIC_ENTRY(combobox_button_width),
    METRIC_ENTRY(scrollbar_width),
    METRIC_ENTRY(dialog_default_width),
    METRIC_ENTRY(dialog_default_height),
    METRIC_ENTRY(dialog_title_bar_height),
    METRIC_ENTRY(dialog_close_button_size),
    METRIC_ENTRY(dialog_padding),
};
static const size_t s_num_metrics = sizeof(s_metrics) / sizeof(s_metrics[0]);

/* Font path string mapping */
typedef struct { const char* name; size_t offset; size_t max_len; } FontEntry;
#define FONT_ENTRY(field) { #field, offsetof(RandyStyle, field), sizeof(((RandyStyle*)0)->field) }

static const FontEntry s_fonts[] = {
    FONT_ENTRY(font_sans_path),
    FONT_ENTRY(font_mono_path),
};
static const size_t s_num_fonts = sizeof(s_fonts) / sizeof(s_fonts[0]);

/* Parse a "fonts" object section.  p points just after '{'. */
static const char* parse_fonts_section(const char* p, RandyStyle* style) {
    while (*p) {
        p = skip_ws(p);
        if (*p == '}') { p++; return p; }
        if (*p == ',') { p++; continue; }

        char key[128];
        p = parse_string(p, key, sizeof(key));
        if (!p) return NULL;

        p = skip_ws(p);
        if (*p != ':') return NULL;
        p++;
        p = skip_ws(p);

        char val[256];
        p = parse_string(p, val, sizeof(val));
        if (!p) return NULL;

        for (size_t i = 0; i < s_num_fonts; i++) {
            if (strcmp(key, s_fonts[i].name) == 0) {
                char* dst = (char*)style + s_fonts[i].offset;
                snprintf(dst, s_fonts[i].max_len, "%s", val);
                break;
            }
        }
    }
    return p;
}

/* Parse an object section: either "colors" or "metrics".
 * p points just after the '{'.  Returns pointer past '}'. */
static const char* parse_section(const char* p, RandyStyle* style,
                                 bool is_colors) {
    while (*p) {
        p = skip_ws(p);
        if (*p == '}') { p++; return p; }
        if (*p == ',') { p++; continue; }

        /* Parse key */
        char key[128];
        p = parse_string(p, key, sizeof(key));
        if (!p) return NULL;

        p = skip_ws(p);
        if (*p != ':') return NULL;
        p++;
        p = skip_ws(p);

        if (is_colors) {
            /* Value is a hex color string */
            char val[32];
            p = parse_string(p, val, sizeof(val));
            if (!p) return NULL;

            for (size_t i = 0; i < s_num_colors; i++) {
                if (strcmp(key, s_colors[i].name) == 0) {
                    RandyColor* c = (RandyColor*)((char*)style + s_colors[i].offset);
                    parse_hex_color(val, c);
                    break;
                }
            }
        } else {
            /* Value is a number */
            double val;
            p = parse_number(p, &val);
            if (!p) return NULL;

            for (size_t i = 0; i < s_num_metrics; i++) {
                if (strcmp(key, s_metrics[i].name) == 0) {
                    int* m = (int*)((char*)style + s_metrics[i].offset);
                    *m = (int)val;
                    break;
                }
            }
        }
    }
    return p;
}

static bool style_load_impl(RandyStyle* out, const char* path) {
    if (!out || !path) return false;

    size_t len = 0;
    char* buf = read_file_to_string(path, &len);
    if (!buf) {
        fprintf(stderr, "[randy/style] failed to open: %s\n", path);
        return false;
    }

    const char* p = skip_ws(buf);
    if (*p != '{') {
        fprintf(stderr, "[randy/style] expected '{' at start of %s\n", path);
        free(buf);
        return false;
    }
    p++;

    while (*p) {
        p = skip_ws(p);
        if (*p == '}') break;
        if (*p == ',') { p++; continue; }

        char key[128];
        p = parse_string(p, key, sizeof(key));
        if (!p) break;

        p = skip_ws(p);
        if (*p != ':') break;
        p++;
        p = skip_ws(p);

        if (*p == '{') {
            p++;
            if (strcmp(key, "colors") == 0) {
                p = parse_section(p, out, true);
            } else if (strcmp(key, "fonts") == 0) {
                p = parse_fonts_section(p, out);
            } else {
                p = parse_section(p, out, false);
            }
            if (!p) break;
        } else {
            /* Top-level scalar â€” skip */
            char val[256];
            if (*p == '"') {
                p = parse_string(p, val, sizeof(val));
            } else {
                double d;
                p = parse_number(p, &d);
            }
            if (!p) break;
        }
    }

    free(buf);
    return true;
}

/* =========================================================================
 * JSONC writer
 * ========================================================================= */

static void color_to_hex(const RandyColor* c, char* buf, size_t buf_size) {
    int r = (int)(c->r * 255.0f + 0.5f);
    int g = (int)(c->g * 255.0f + 0.5f);
    int b = (int)(c->b * 255.0f + 0.5f);
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    snprintf(buf, buf_size, "#%02X%02X%02X", r, g, b);
}

/* Color role descriptions for JSONC comments */
static const char* s_color_comments[] = {
    "main text foreground",
    "window / panel background",
    "raised control surface",
    "bevel light edge",
    "bevel dark edge",
    "outer border / dark frame",
    "selection / active title bar",
    "text on highlighted background",
    "control surface on hover",
    "text field / checkbox inner fill",
    "hover ring around input controls",
    "tooltip info-window fill",
    "tooltip text",
};

/* Metric descriptions for JSONC comments */
static const char* s_metric_comments[] = {
    "UI font pixel size",
    "outer frame inset per side",
    "window title bar height",
    "horizontal content inset from chrome",
    "top content inset below title bar",
    "bottom margin inside chrome",
    "default space between widgets",
    "label widget height",
    "button widget height",
    "checkbox / radio widget height",
    "text box widget height",
    "dropdown widget height",
    "slider widget height",
    "progress bar widget height",
    "group box widget height",
    "tab button height",
    "tab button default width",
    "tree item row height",
    "table header row height",
    "table data row height",
    "field border widget height",
    "status field widget height",
    "sunken panel widget height",
    "separator widget height",
    "spinbox widget height",
    "combobox widget height",
    "menu bar widget height",
    "toolbar widget height",
    "text edit widget height",
    "listbox widget height",
    "image placeholder default size",
    "tooltip widget height",
    "accordion header height",
    "scroll area default height",
    "stacked container default height",
    "tab widget default height",
    "checkbox indicator square size",
    "radio button circle diameter",
    "spinbox up/down arrow width",
    "combobox dropdown arrow width",
    "scrollbar track width",
    "dialog default width",
    "dialog default height",
    "dialog title bar height",
    "dialog close button size",
    "dialog internal padding",
};

static bool style_save_impl(const RandyStyle* style, const char* path) {
    if (!style || !path) return false;

    FILE* f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "[randy/style] failed to create: %s\n", path);
        return false;
    }

    fprintf(f, "// randy style/palette file (JSONC)\n");
    fprintf(f, "// Edit colors as \"#RRGGBB\" hex strings, metrics as integers.\n");
    fprintf(f, "{\n");

    /* Colors section */
    fprintf(f, "  // --- Color palette ---\n");
    fprintf(f, "  \"colors\": {\n");
    for (size_t i = 0; i < s_num_colors; i++) {
        const RandyColor* c = (const RandyColor*)((const char*)style + s_colors[i].offset);
        char hex[8];
        color_to_hex(c, hex, sizeof(hex));
        fprintf(f, "    \"%s\": \"%s\"", s_colors[i].name, hex);
        if (i + 1 < s_num_colors) fprintf(f, ",");
        fprintf(f, "  // %s\n", s_color_comments[i]);
    }
    fprintf(f, "  },\n\n");

    /* Fonts section */
    fprintf(f, "  // --- Font paths ---\n");
    fprintf(f, "  \"fonts\": {\n");
    for (size_t i = 0; i < s_num_fonts; i++) {
        const char* val = (const char*)style + s_fonts[i].offset;
        fprintf(f, "    \"%s\": \"%s\"", s_fonts[i].name, val);
        if (i + 1 < s_num_fonts) fprintf(f, ",");
        fprintf(f, "\n");
    }
    fprintf(f, "  },\n\n");

    /* Metrics section */
    fprintf(f, "  // --- Size metrics (pixels) ---\n");
    fprintf(f, "  \"metrics\": {\n");
    for (size_t i = 0; i < s_num_metrics; i++) {
        const int* m = (const int*)((const char*)style + s_metrics[i].offset);
        fprintf(f, "    \"%s\": %d", s_metrics[i].name, *m);
        if (i + 1 < s_num_metrics) fprintf(f, ",");
        fprintf(f, "  // %s\n", s_metric_comments[i]);
    }
    fprintf(f, "  }\n");

    fprintf(f, "}\n");
    fclose(f);
    return true;
}

/* =========================================================================
 * Public API wrappers (declared in randyosgui.h with RANDY_API)
 * ========================================================================= */

void randy_style_set_defaults(RandyStyle* style) {
    style_set_defaults_impl(style);
}

int randy_style_load(RandyStyle* style, const char* path) {
    return style_load_impl(style, path) ? 0 : -1;
}

int randy_style_save(const RandyStyle* style, const char* path) {
    return style_save_impl(style, path) ? 0 : -1;
}

RandyStyle* randy_style_get_active(void) {
    return &g_style;
}
