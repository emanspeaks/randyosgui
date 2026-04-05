#include "renderer_private.h"
#include "../style.h"

/*
 * renderer_draw.c â€” Win98 theme palette, text system, and draw primitives.
 *
 * Covers:
 *   - Global draw-capture state (g_capture) for headless testing
 *   - Win98 theme color constants (WIN98)
 *   - FreeType text system init / shutdown
 *   - Low-level pixel drawing (draw_rect, draw_bevel)
 *   - Window chrome helper (draw_window_chrome)
 *   - Widget rect and text drawing (draw_widget_rect, draw_text_span, draw_widget_text)
 *   - Widget-specific draw helpers (radio, tab, status, sunken, dropdown button)
 *   - Tree view geometry helpers (tree_has_next_sibling, etc.)
 */

/* =========================================================================
 * Draw-capture global (used by renderer_test_capture_widget_draw_ops)
 * ========================================================================= */

DrawCapture g_capture = {0};

/* =========================================================================
 * Win98 theme palette â€” now driven by g_style (see style.h / style.c)
 * ========================================================================= */

/* Backward-compat: kept as shorthand macros for draw code below. */
#define S g_style

/* =========================================================================
 * Text system
 * ========================================================================= */

bool init_text_system(RendererContext* r) {
    const char* sans_path = g_style.font_sans_path;
    const char* mono_path = g_style.font_mono_path;
    unsigned int font_px = (unsigned int)g_style.font_size_px;
    if (font_px == 0) font_px = 12;

    if (FT_Init_FreeType(&r->ft_library) != 0) {
        fprintf(stderr, "[randy/renderer] FreeType init failed; text disabled\n");
        r->ft_library = NULL;
        r->font_sans = NULL;
        r->font_mono = NULL;
        return false;
    }

    if (FT_New_Face(r->ft_library, sans_path, 0, &r->font_sans) != 0) {
        fprintf(stderr, "[randy/renderer] failed to load sans font: %s\n", sans_path);
        FT_Done_FreeType(r->ft_library);
        r->ft_library = NULL;
        r->font_sans = NULL;
        r->font_mono = NULL;
        return false;
    }

    if (FT_New_Face(r->ft_library, mono_path, 0, &r->font_mono) != 0) {
        fprintf(stderr, "[randy/renderer] failed to load mono font: %s\n", mono_path);
        FT_Done_Face(r->font_sans);
        FT_Done_FreeType(r->ft_library);
        r->ft_library = NULL;
        r->font_sans = NULL;
        r->font_mono = NULL;
        return false;
    }

    FT_Set_Pixel_Sizes(r->font_sans, 0, font_px);
    FT_Set_Pixel_Sizes(r->font_mono, 0, font_px);

    return true;
}

void shutdown_text_system(RendererContext* r) {
    if (r->font_mono) {
        FT_Done_Face(r->font_mono);
        r->font_mono = NULL;
    }
    if (r->font_sans) {
        FT_Done_Face(r->font_sans);
        r->font_sans = NULL;
    }
    if (r->ft_library) {
        FT_Done_FreeType(r->ft_library);
        r->ft_library = NULL;
    }
}

/* =========================================================================
 * Low-level pixel drawing
 * ========================================================================= */

void draw_rect(VkCommandBuffer cmd,
               VkExtent2D extent,
               int x,
               int y,
               int rw,
               int rh,
               float r,
               float g,
               float b) {

    if (x < 0) { rw += x; x = 0; }
    if (y < 0) { rh += y; y = 0; }
    if (x >= (int)extent.width || y >= (int)extent.height) return;
    if (x + rw > (int)extent.width)  rw = (int)extent.width  - x;
    if (y + rh > (int)extent.height) rh = (int)extent.height - y;
    if (rw <= 0 || rh <= 0) return;

    if (g_capture.enabled) {
        if (g_capture.count < g_capture.max_ops) {
            g_capture.ops[g_capture.count] = (RandyDrawOp){
                .x = x, .y = y, .w = rw, .h = rh,
                .r = r, .g = g, .b = b,
            };
        }
        g_capture.count++;
        return;
    }

    VkClearAttachment att = {
        .aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT,
        .colorAttachment = 0,
        .clearValue      = { .color = { .float32 = { r, g, b, 1.0f } } },
    };

    VkClearRect rect = {
        .rect           = { .offset = { .x = x, .y = y },
                            .extent = { .width = (uint32_t)rw, .height = (uint32_t)rh } },
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    vkCmdClearAttachments(cmd, 1, &att, 1, &rect);
}

void draw_bevel(VkCommandBuffer cmd, VkExtent2D extent,
                int x, int y, int w, int h, bool sunken) {
    if (w < 5 || h < 5) return;

    /* 2px 3D bevel — graduated highlight/shadow like classic Win32 controls */
    float hr = g_style.button_highlight.r;
    float hg = g_style.button_highlight.g;
    float hb = g_style.button_highlight.b;
    float sr = g_style.button_shadow.r;
    float sg = g_style.button_shadow.g;
    float sb = g_style.button_shadow.b;
    float dr = g_style.window_frame.r;
    float dg = g_style.window_frame.g;
    float db = g_style.window_frame.b;

    if (!sunken) {
        /* Outer highlight (top/left) */
        draw_rect(cmd, extent, x, y, w - 1, 1, hr, hg, hb);
        draw_rect(cmd, extent, x, y + 1, 1, h - 1, hr, hg, hb);
        /* Outer dark (bottom/right) */
        draw_rect(cmd, extent, x, y + h - 1, w, 1, dr, dg, db);
        draw_rect(cmd, extent, x + w - 1, y, 1, h, dr, dg, db);
        /* Inner shadow (bottom/right, inset 1) */
        draw_rect(cmd, extent, x + 1, y + h - 2, w - 2, 1, sr, sg, sb);
        draw_rect(cmd, extent, x + w - 2, y + 1, 1, h - 2, sr, sg, sb);
    } else {
        /* Outer dark (top/left) */
        draw_rect(cmd, extent, x, y, w - 1, 1, dr, dg, db);
        draw_rect(cmd, extent, x, y + 1, 1, h - 1, dr, dg, db);
        /* Inner shadow (top/left, inset 1) */
        draw_rect(cmd, extent, x + 1, y + 1, w - 3, 1, sr, sg, sb);
        draw_rect(cmd, extent, x + 1, y + 2, 1, h - 3, sr, sg, sb);
        /* Outer highlight (bottom/right) */
        draw_rect(cmd, extent, x, y + h - 1, w, 1, hr, hg, hb);
        draw_rect(cmd, extent, x + w - 1, y, 1, h, hr, hg, hb);
    }
}

void draw_window_chrome(VkCommandBuffer cmd, VkExtent2D extent) {
    draw_rect(cmd, extent, 0, 0, (int)extent.width, (int)extent.height,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);

    /* Raised bevel on frame */
    int w = (int)extent.width;
    int h = (int)extent.height;
    draw_bevel(cmd, extent, 0, 0, w, h, false);

    /* Title bar */
    int bw = g_style.window_border_width;
    int title_x = bw;
    int title_y = bw;
    int title_w = w - 2 * bw;
    int title_h = g_style.title_bar_height;
    draw_rect(cmd, extent, title_x, title_y, title_w, title_h,
              g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
}

void draw_widget_rect(VkCommandBuffer cmd,
                      const Widget* w,
                      VkExtent2D extent,
                      float r,
                      float g,
                      float b) {
    if (!w || w->w <= 0 || w->h <= 0) return;
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h, r, g, b);
}

/* =========================================================================
 * Text rendering
 * ========================================================================= */

void draw_text_span(RendererContext* r,
                    VkCommandBuffer cmd,
                    VkExtent2D extent,
                    const Widget* w,
                    FT_Face face,
                    const char* begin,
                    const char* end,
                    int pen_x,
                    int baseline_y,
                    float tr,
                    float tg,
                    float tb,
                    float br,
                    float bg,
                    float bb) {
    (void)r;
    if (!face || !begin || begin == end) return;

    for (const char* p = begin; p < end && *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (FT_Load_Char(face, ch, FT_LOAD_RENDER) != 0) {
            continue;
        }

        FT_GlyphSlot slot = face->glyph;
        int glyph_x = pen_x + slot->bitmap_left;
        int glyph_y = baseline_y - slot->bitmap_top;

        for (int row = 0; row < (int)slot->bitmap.rows; row++) {
            for (int col = 0; col < (int)slot->bitmap.width; col++) {
                unsigned char cov = slot->bitmap.buffer[(row * (int)slot->bitmap.pitch) + col];
                if (cov == 0) continue;

                int px = glyph_x + col;
                int py = glyph_y + row;
                if (px < w->x || py < w->y || px >= (w->x + w->w) || py >= (w->y + w->h)) {
                    continue;
                }

                float a = ((float)cov) / 255.0f;
                float rr = (br * (1.0f - a)) + (tr * a);
                float gg = (bg * (1.0f - a)) + (tg * a);
                float bbm = (bb * (1.0f - a)) + (tb * a);

                draw_rect(cmd, extent, px, py, 1, 1, rr, gg, bbm);
            }
        }

        pen_x += (int)(slot->advance.x >> 6);
        if (pen_x >= (w->x + w->w - 6)) break;
    }
}

void draw_widget_text(RendererContext* r, VkCommandBuffer cmd, const Widget* w,
                      VkExtent2D extent, int text_offset_y,
                      float tr, float tg, float tb,
                      float br, float bg, float bb) {
    if (!r->font_sans || !w->text || !w->text[0]) return;

    int baseline_y = w->y + (w->h / 2) + text_offset_y;
    const char* split = strstr(w->text, "||");
    if (split && r->font_mono) {
        const char* left_begin = w->text;
        const char* left_end = split;
        while (left_end > left_begin && left_end[-1] == ' ') left_end--;

        const char* right_begin = split + 2;
        while (*right_begin == ' ') right_begin++;

        int mid = w->x + (w->w / 2);
        draw_text_span(r, cmd, extent, w, r->font_sans,
                       left_begin, left_end,
                       w->x + 10, baseline_y,
                       tr, tg, tb, br, bg, bb);
        draw_text_span(r, cmd, extent, w, r->font_mono,
                       right_begin, right_begin + strlen(right_begin),
                       mid + 8, baseline_y,
                       tr, tg, tb, br, bg, bb);
        return;
    }

    draw_text_span(r, cmd, extent, w, r->font_sans,
                   w->text, w->text + strlen(w->text),
                   w->x + 6, baseline_y,
                   tr, tg, tb, br, bg, bb);
}

int approx_text_px(const char* s) {
    if (!s) return 0;
    return (int)strlen(s) * 7;
}

/* =========================================================================
 * Widget-specific draw helpers (98.css faithful transcriptions)
 * ========================================================================= */

/* =========================================================================
 * Tree view geometry helpers
 * ========================================================================= */

bool tree_has_next_sibling(const Widget* row) {
    if (!row || row->kind != WIDGET_TREE_ITEM) return false;
    int level = row->value;
    for (const Widget* t = row->next_sibling; t && t->kind == WIDGET_TREE_ITEM; t = t->next_sibling) {
        if (t->value < level) break;
        if (t->value == level) return true;
    }
    return false;
}

bool tree_has_prev_sibling(const Widget* block_start, const Widget* row) {
    if (!block_start || !row || row->kind != WIDGET_TREE_ITEM) return false;
    int level = row->value;
    const Widget* prev = NULL;
    for (const Widget* t = block_start; t && t != row; t = t->next_sibling) {
        if (t->kind != WIDGET_TREE_ITEM) break;
        prev = t;
    }
    for (const Widget* t = prev; t && t != block_start; ) {
        if (t->kind != WIDGET_TREE_ITEM) break;
        if (t->value < level) break;
        if (t->value == level) return true;

        const Widget* p = block_start;
        const Widget* pprev = NULL;
        while (p && p != t) {
            pprev = p;
            p = p->next_sibling;
        }
        t = pprev;
    }

    if (block_start == row) return false;
    if (block_start->kind == WIDGET_TREE_ITEM && block_start->value == level) return true;
    return false;
}

const Widget* tree_find_ancestor(const Widget* block_start, const Widget* row,
                                  int ancestor_level) {
    const Widget* ancestor = NULL;
    for (const Widget* t = block_start; t && t != row; t = t->next_sibling) {
        if (t->kind != WIDGET_TREE_ITEM) break;
        if (t->value == ancestor_level) ancestor = t;
    }
    return ancestor;
}
