#include <stdio.h>
#include "randyosgui.h"

typedef struct {
    RandyosgWindow* win;
    RandyosgWidgetId status_label;
    RandyosgWidgetId progress_id;
    int click_count;
    int checkbox_changes;
    int slider_changes;
} DemoState;

static void on_action_click(RandyosgWidgetId id, void* userdata) {
    DemoState* state = (DemoState*)userdata;
    state->click_count++;

    char msg[128];
    snprintf(msg, sizeof(msg), "Action button #%u clicked (%d total)", id, state->click_count);
    randyosgui_label_set_text(state->win, state->status_label, msg);
}

static void on_toggle_changed(RandyosgWidgetId id, bool checked, void* userdata) {
    DemoState* state = (DemoState*)userdata;
    state->checkbox_changes++;

    char msg[128];
    snprintf(msg, sizeof(msg), "Toggle #%u -> %s (%d changes)",
             id,
             checked ? "checked" : "unchecked",
             state->checkbox_changes);
    randyosgui_label_set_text(state->win, state->status_label, msg);
}

static void on_slider_changed(RandyosgWidgetId id, int value, void* userdata) {
    DemoState* state = (DemoState*)userdata;
    state->slider_changes++;

    if (state->progress_id != 0) {
        randyosgui_progress_set_value(state->win, state->progress_id, value);
    }

    char msg[128];
    snprintf(msg, sizeof(msg), "Slider #%u -> %d (%d changes)",
             id, value, state->slider_changes);
    randyosgui_label_set_text(state->win, state->status_label, msg);
}

int main(void) {
    RandyosgContext* ctx = randyosgui_init();
    if (!ctx) {
        fprintf(stderr, "randyosgui_init failed\n");
        return 1;
    }

    RandyosgWindowDesc desc = {
        .title = "randyosgui - Win98 Controls Gallery",
        .width = 860,
        .height = 640,
        .resizable = true,
        .decorated = true,
    };

    RandyosgWindow* win = randyosgui_window_create(ctx, &desc);
    if (!win) {
        fprintf(stderr, "randyosgui_window_create failed\n");
        randyosgui_shutdown(ctx);
        return 1;
    }

    DemoState demo = {
        .win = win,
        .status_label = 0,
        .progress_id = 0,
        .click_count = 0,
        .checkbox_changes = 0,
        .slider_changes = 0,
    };

    randyosgui_label_create(win, "Windows 98 style gallery (draft all-controls pass)");
    randyosgui_label_create(win, "Draft controls: Label, Button, Checkbox, Radio, TextBox, Dropdown, Slider, Progress, GroupBox, Tabs, Tree, Table, Field/Status/Sunken");
    demo.status_label = randyosgui_label_create(win, "Ready for visual review");

    RandyosgWidgetId tab_general = randyosgui_tab_create(win, "General", true);
    RandyosgWidgetId tab_advanced = randyosgui_tab_create(win, "Advanced", false);
    RandyosgWidgetId tab_about = randyosgui_tab_create(win, "About", false);

    randyosgui_groupbox_create(win, "View options");

    RandyosgWidgetId cb_grid = randyosgui_checkbox_create(win, "Show grid", true);
    RandyosgWidgetId cb_snap = randyosgui_checkbox_create(win, "Snap to controls", false);
    RandyosgWidgetId cb_anim = randyosgui_checkbox_create(win, "Animate transitions", true);

    randyosgui_label_create(win, "Radio buttons");
    RandyosgWidgetId rb_light = randyosgui_radio_create(win, "Light theme", true);
    RandyosgWidgetId rb_classic = randyosgui_radio_create(win, "Classic theme", false);

    randyosgui_textbox_create(win, "C:/projects/randyosgui", false);
    randyosgui_textbox_create(win, "Read-only status: connected", true);

    RandyosgWidgetId quality_dd = randyosgui_dropdown_create(win, "Quality: Balanced");

    RandyosgWidgetId zoom_slider = randyosgui_slider_create(win, "Zoom level", 0, 100, 40);
    randyosgui_label_create(win, "Preview loading");
    demo.progress_id = randyosgui_progress_create(win, "", 100, 40);

    randyosgui_label_create(win, "Project tree");
    randyosgui_tree_item_create(win, "src", 0, true, true);
    randyosgui_tree_item_create(win, "renderer", 1, true, true);
    randyosgui_tree_item_create(win, "renderer.c", 2, false, false);
    randyosgui_tree_item_create(win, "platform", 1, true, false);
    randyosgui_tree_item_create(win, "include", 0, true, true);
    randyosgui_tree_item_create(win, "randyosgui.h", 1, false, false);

    randyosgui_label_create(win, "Recent files");
    static const char* const tbl_labels[] = { "Name", "Size", "Modified" };
    static const int         tbl_widths[] = { 120,    50,    80 };
    randyosgui_table_header_create(win, 3, tbl_labels, tbl_widths);
    static const char* const row1_cells[] = { "renderer.c", "58K",  "Today" };
    static const char* const row2_cells[] = { "platform.c", "14K",  "Yesterday" };
    static const char* const row3_cells[] = { "build.zig",  "22K",  "Today" };
    RandyosgWidgetId row1 = randyosgui_table_row_create(win, 3, row1_cells, true);
    RandyosgWidgetId row2 = randyosgui_table_row_create(win, 3, row2_cells, false);
    RandyosgWidgetId row3 = randyosgui_table_row_create(win, 3, row3_cells, false);

    randyosgui_label_create(win, "Field borders");
    randyosgui_field_border_create(win, "Work area", false);
    randyosgui_field_border_create(win, "Read-only area", true);

    randyosgui_label_create(win, "Sunken panel");
    randyosgui_sunken_panel_create(win, "Panel content preview");

    randyosgui_label_create(win, "Status bar");
    randyosgui_status_field_create(win, "Press F1 for help");
    randyosgui_status_field_create(win, "CPU Usage: 14%");

    RandyosgWidgetId open_btn = randyosgui_button_create(win, "Open");
    RandyosgWidgetId save_btn = randyosgui_button_create(win, "Save");
    RandyosgWidgetId apply_btn = randyosgui_button_create(win, "Apply");
    RandyosgWidgetId cancel_btn = randyosgui_button_create(win, "Cancel");
    RandyosgWidgetId help_btn = randyosgui_button_create(win, "Help");

    randyosgui_button_set_callback(win, open_btn, on_action_click, &demo);
    randyosgui_button_set_callback(win, save_btn, on_action_click, &demo);
    randyosgui_button_set_callback(win, apply_btn, on_action_click, &demo);
    randyosgui_button_set_callback(win, cancel_btn, on_action_click, &demo);
    randyosgui_button_set_callback(win, help_btn, on_action_click, &demo);
    randyosgui_button_set_callback(win, quality_dd, on_action_click, &demo);
    randyosgui_tab_set_callback(win, tab_general, on_action_click, &demo);
    randyosgui_tab_set_callback(win, tab_advanced, on_action_click, &demo);
    randyosgui_tab_set_callback(win, tab_about, on_action_click, &demo);
    randyosgui_table_row_set_callback(win, row1, on_action_click, &demo);
    randyosgui_table_row_set_callback(win, row2, on_action_click, &demo);
    randyosgui_table_row_set_callback(win, row3, on_action_click, &demo);

    randyosgui_checkbox_set_callback(win, cb_grid, on_toggle_changed, &demo);
    randyosgui_checkbox_set_callback(win, cb_snap, on_toggle_changed, &demo);
    randyosgui_checkbox_set_callback(win, cb_anim, on_toggle_changed, &demo);
    randyosgui_radio_set_callback(win, rb_light, on_toggle_changed, &demo);
    randyosgui_radio_set_callback(win, rb_classic, on_toggle_changed, &demo);
    randyosgui_slider_set_callback(win, zoom_slider, on_slider_changed, &demo);

    while (!randyosgui_window_should_close(win)) {
        randyosgui_poll_events(ctx);
        randyosgui_render(win);
    }

    randyosgui_window_destroy(win);
    randyosgui_shutdown(ctx);
    return 0;
}
