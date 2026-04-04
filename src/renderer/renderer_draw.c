#include "renderer_private.h"

/*
 * renderer_draw.c — Win98 theme palette, text system, and draw primitives.
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
 * Win98 theme palette
 * ========================================================================= */

const Win98Theme WIN98 = {
    .text_r = 0.133f, .text_g = 0.133f, .text_b = 0.133f,
    .surface_r = 0.753f, .surface_g = 0.753f, .surface_b = 0.753f,
    .button_face_r = 0.875f, .button_face_g = 0.875f, .button_face_b = 0.875f,
    .button_highlight_r = 1.000f, .button_highlight_g = 1.000f, .button_highlight_b = 1.000f,
    .button_shadow_r = 0.502f, .button_shadow_g = 0.502f, .button_shadow_b = 0.502f,
    .window_frame_r = 0.039f, .window_frame_g = 0.039f, .window_frame_b = 0.039f,
    .dialog_blue_r = 0.000f, .dialog_blue_g = 0.000f, .dialog_blue_b = 0.502f,
};

/* =========================================================================
 * Text system
 * ========================================================================= */

bool init_text_system(RendererContext* r) {
    static const unsigned int UI_FONT_PX = 12;
    static const char SANS_PATH[] =
        "third_party/fonts/arimo/ArimoNerdFont-Regular.ttf";
    static const char MONO_PATH[] =
        "third_party/fonts/atkinsonhyperlegiblemono/AtkynsonMonoNerdFontMono-Regular.otf";

    if (FT_Init_FreeType(&r->ft_library) != 0) {
        fprintf(stderr, "[randyosgui/renderer] FreeType init failed; text disabled\n");
        r->ft_library = NULL;
        r->font_sans = NULL;
        r->font_mono = NULL;
        return false;
    }

    if (FT_New_Face(r->ft_library, SANS_PATH, 0, &r->font_sans) != 0) {
        fprintf(stderr, "[randyosgui/renderer] failed to load sans font: %s\n", SANS_PATH);
        FT_Done_FreeType(r->ft_library);
        r->ft_library = NULL;
        r->font_sans = NULL;
        r->font_mono = NULL;
        return false;
    }

    if (FT_New_Face(r->ft_library, MONO_PATH, 0, &r->font_mono) != 0) {
        fprintf(stderr, "[randyosgui/renderer] failed to load mono font: %s\n", MONO_PATH);
        FT_Done_Face(r->font_sans);
        FT_Done_FreeType(r->ft_library);
        r->ft_library = NULL;
        r->font_sans = NULL;
        r->font_mono = NULL;
        return false;
    }

    FT_Set_Pixel_Sizes(r->font_sans, 0, UI_FONT_PX);
    FT_Set_Pixel_Sizes(r->font_mono, 0, UI_FONT_PX);

    fprintf(stderr, "[randyosgui/renderer] text fonts: Arimo(sans) + AtkynsonMono(mono) (%upx)\n", UI_FONT_PX);
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
            g_capture.ops[g_capture.count] = (RandyosgDrawOp){
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
    if (w < 3 || h < 3) return;

    float tl_r = sunken ? WIN98.window_frame_r : WIN98.button_highlight_r;
    float tl_g = sunken ? WIN98.window_frame_g : WIN98.button_highlight_g;
    float tl_b = sunken ? WIN98.window_frame_b : WIN98.button_highlight_b;
    float br_r = sunken ? WIN98.button_highlight_r : WIN98.window_frame_r;
    float br_g = sunken ? WIN98.button_highlight_g : WIN98.window_frame_g;
    float br_b = sunken ? WIN98.button_highlight_b : WIN98.window_frame_b;

    float inner_tl_r = sunken ? WIN98.button_shadow_r : WIN98.button_face_r;
    float inner_tl_g = sunken ? WIN98.button_shadow_g : WIN98.button_face_g;
    float inner_tl_b = sunken ? WIN98.button_shadow_b : WIN98.button_face_b;
    float inner_br_r = sunken ? WIN98.button_face_r : WIN98.button_shadow_r;
    float inner_br_g = sunken ? WIN98.button_face_g : WIN98.button_shadow_g;
    float inner_br_b = sunken ? WIN98.button_face_b : WIN98.button_shadow_b;

    draw_rect(cmd, extent, x, y, w, 1, tl_r, tl_g, tl_b);
    draw_rect(cmd, extent, x, y, 1, h, tl_r, tl_g, tl_b);
    draw_rect(cmd, extent, x + w - 1, y, 1, h, br_r, br_g, br_b);
    draw_rect(cmd, extent, x, y + h - 1, w, 1, br_r, br_g, br_b);

    draw_rect(cmd, extent, x + 1, y + 1, w - 2, 1, inner_tl_r, inner_tl_g, inner_tl_b);
    draw_rect(cmd, extent, x + 1, y + 1, 1, h - 2, inner_tl_r, inner_tl_g, inner_tl_b);
    draw_rect(cmd, extent, x + w - 2, y + 1, 1, h - 2, inner_br_r, inner_br_g, inner_br_b);
    draw_rect(cmd, extent, x + 1, y + h - 2, w - 2, 1, inner_br_r, inner_br_g, inner_br_b);
}

void draw_window_chrome(VkCommandBuffer cmd, VkExtent2D extent) {
    draw_rect(cmd, extent, 0, 0, (int)extent.width, (int)extent.height,
              WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);

    int outer_x = 6;
    int outer_y = 6;
    int outer_w = (int)extent.width - 12;
    int outer_h = (int)extent.height - 12;
    draw_bevel(cmd, extent, outer_x, outer_y, outer_w, outer_h, false);

    int title_x = outer_x + 3;
    int title_y = outer_y + 3;
    int title_w = outer_w - 6;
    int title_h = 20;
    draw_rect(cmd, extent, title_x, title_y, title_w, title_h,
              WIN98.dialog_blue_r, WIN98.dialog_blue_g, WIN98.dialog_blue_b);
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

void draw_radio_border_98(VkCommandBuffer cmd, VkExtent2D extent, int x, int y) {
    const float g = 0.502f;
    const float lg = 0.875f;

    draw_rect(cmd, extent, x + 4, y + 0, 4, 1, g, g, g);
    draw_rect(cmd, extent, x + 2, y + 1, 2, 1, g, g, g);
    draw_rect(cmd, extent, x + 8, y + 1, 2, 1, g, g, g);
    draw_rect(cmd, extent, x + 1, y + 2, 1, 2, g, g, g);
    draw_rect(cmd, extent, x + 0, y + 4, 1, 4, g, g, g);
    draw_rect(cmd, extent, x + 1, y + 8, 1, 2, g, g, g);

    draw_rect(cmd, extent, x + 4, y + 1, 4, 1, 0.0f, 0.0f, 0.0f);
    draw_rect(cmd, extent, x + 2, y + 2, 2, 1, 0.0f, 0.0f, 0.0f);
    draw_rect(cmd, extent, x + 8, y + 2, 2, 1, 0.0f, 0.0f, 0.0f);
    draw_rect(cmd, extent, x + 1, y + 3, 1, 5, 0.0f, 0.0f, 0.0f);
    draw_rect(cmd, extent, x + 2, y + 3, 1, 2, 0.0f, 0.0f, 0.0f);

    draw_rect(cmd, extent, x + 10, y + 3, 1, 1, lg, lg, lg);
    draw_rect(cmd, extent, x + 10, y + 4, 1, 4, lg, lg, lg);
    draw_rect(cmd, extent, x + 8,  y + 10, 2, 1, lg, lg, lg);
    draw_rect(cmd, extent, x + 4,  y + 10, 4, 1, lg, lg, lg);
    draw_rect(cmd, extent, x + 2,  y + 10, 2, 1, lg, lg, lg);

    draw_rect(cmd, extent, x + 10, y + 2, 1, 2, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 11, y + 4, 1, 4, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 10, y + 8, 1, 2, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 8,  y + 11, 2, 1, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 4,  y + 11, 4, 1, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 2,  y + 11, 2, 1, 1.0f, 1.0f, 1.0f);

    draw_rect(cmd, extent, x + 3, y + 2, 6, 1, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 2, y + 3, 8, 1, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 2, y + 4, 8, 4, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 2, y + 8, 8, 1, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x + 3, y + 9, 6, 1, 1.0f, 1.0f, 1.0f);
}

void draw_radio_dot_98(VkCommandBuffer cmd, VkExtent2D extent, int x, int y) {
    draw_rect(cmd, extent, x + 1, y + 0, 2, 1,
              WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
    draw_rect(cmd, extent, x + 0, y + 1, 4, 2,
              WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
    draw_rect(cmd, extent, x + 1, y + 3, 2, 1,
              WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
}

void draw_tab_border_98(VkCommandBuffer cmd, VkExtent2D extent, int x, int y, int w, int h) {
    if (w < 6 || h < 6) return;

    draw_rect(cmd, extent, x + 2, y, w - 3, 1,
              WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
    draw_rect(cmd, extent, x + 2, y + 1, w - 4, 1,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);

    draw_rect(cmd, extent, x, y + 2, 1, h - 2,
              WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
    draw_rect(cmd, extent, x + 1, y + 2, 1, h - 3,
              WIN98.button_highlight_r, WIN98.button_highlight_g, WIN98.button_highlight_b);

    draw_rect(cmd, extent, x + w - 1, y + 2, 1, h - 2,
              WIN98.window_frame_r, WIN98.window_frame_g, WIN98.window_frame_b);
    draw_rect(cmd, extent, x + w - 2, y + 2, 1, h - 3,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
}

void draw_status_field_border_98(VkCommandBuffer cmd, VkExtent2D extent,
                                  int x, int y, int w, int h) {
    if (w < 3 || h < 3) return;
    draw_rect(cmd, extent, x, y, w, 1,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, x, y, 1, h,
              WIN98.button_shadow_r, WIN98.button_shadow_g, WIN98.button_shadow_b);
    draw_rect(cmd, extent, x + w - 1, y, 1, h,
              WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
    draw_rect(cmd, extent, x, y + h - 1, w, 1,
              WIN98.button_face_r, WIN98.button_face_g, WIN98.button_face_b);
}

void draw_sunken_panel_border_98(VkCommandBuffer cmd, VkExtent2D extent,
                                  int x, int y, int w, int h) {
    if (w < 6 || h < 6) return;
    draw_rect(cmd, extent, x, y, w - 1, 1, 0.502f, 0.502f, 0.502f);
    draw_rect(cmd, extent, x, y, 1, h - 1, 0.502f, 0.502f, 0.502f);

    draw_rect(cmd, extent, x + 1, y + 1, w - 3, 1, 0.039f, 0.039f, 0.039f);
    draw_rect(cmd, extent, x + 1, y + 1, 1, h - 3, 0.039f, 0.039f, 0.039f);

    draw_rect(cmd, extent, x + w - 1, y, 1, h, 1.0f, 1.0f, 1.0f);
    draw_rect(cmd, extent, x, y + h - 1, w, 1, 1.0f, 1.0f, 1.0f);

    draw_rect(cmd, extent, x + w - 2, y + 1, 1, h - 3, 0.875f, 0.875f, 0.875f);
    draw_rect(cmd, extent, x + 1, y + h - 2, w - 3, 1, 0.875f, 0.875f, 0.875f);
}

void draw_select_button_98(VkCommandBuffer cmd, VkExtent2D extent, int x, int y) {
    const float black = 0.039f;
    const float white = 1.0f;
    const float gray  = 0.502f;
    const float light = 0.875f;
    const float silver = 0.753f;

    draw_rect(cmd, extent, x + 0,  y + 0,  15, 1,  light,  light,  light);
    draw_rect(cmd, extent, x + 0,  y + 0,  1,  16, light,  light,  light);
    draw_rect(cmd, extent, x + 1,  y + 1,  13, 1,  white,  white,  white);
    draw_rect(cmd, extent, x + 1,  y + 2,  1,  13, white,  white,  white);

    draw_rect(cmd, extent, x + 15, y + 0,  1,  17, black,  black,  black);
    draw_rect(cmd, extent, x + 0,  y + 16, 16, 1,  black,  black,  black);
    draw_rect(cmd, extent, x + 14, y + 1,  1,  14, gray,   gray,   gray);
    draw_rect(cmd, extent, x + 1,  y + 15, 14, 1,  gray,   gray,   gray);

    draw_rect(cmd, extent, x + 2,  y + 2,  12, 13, silver, silver, silver);

    draw_rect(cmd, extent, x + 5,  y + 7,  7,  1,  black,  black,  black);
    draw_rect(cmd, extent, x + 6,  y + 8,  5,  1,  black,  black,  black);
    draw_rect(cmd, extent, x + 7,  y + 9,  3,  1,  black,  black,  black);
    draw_rect(cmd, extent, x + 8,  y + 10, 1,  1,  black,  black,  black);
}

/* =========================================================================
 * Tree view geometry helpers
 * ========================================================================= */

bool tree_has_next_sibling(const Widget* row) {
    if (!row || row->kind != WIDGET_TREE_ITEM) return false;
    int level = row->value;
    for (const Widget* t = row->next; t && t->kind == WIDGET_TREE_ITEM; t = t->next) {
        if (t->value < level) break;
        if (t->value == level) return true;
    }
    return false;
}

bool tree_has_prev_sibling(const Widget* block_start, const Widget* row) {
    if (!block_start || !row || row->kind != WIDGET_TREE_ITEM) return false;
    int level = row->value;
    const Widget* prev = NULL;
    for (const Widget* t = block_start; t && t != row; t = t->next) {
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
            p = p->next;
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
    for (const Widget* t = block_start; t && t != row; t = t->next) {
        if (t->kind != WIDGET_TREE_ITEM) break;
        if (t->value == ancestor_level) ancestor = t;
    }
    return ancestor;
}
