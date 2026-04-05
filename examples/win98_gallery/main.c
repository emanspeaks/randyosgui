/*
 * Win98 Widget Gallery â€” comprehensive demo for randy.
 *
 * Showcases every available widget type, organised into tabbed pages
 * inspired by the Qt Widget Gallery example.
 */

#include <stdio.h>
#include "randyosgui.h"

/* --------------------------------------------------------------------- */
/*  Callback state                                                       */
/* --------------------------------------------------------------------- */

typedef struct {
    RandyWindow*  win;
    RandyWidgetId status_label;
    RandyWidgetId progress_id;
    RandyWidgetId dialog_id;
    int              clicks;
} DemoState;

static void on_click(RandyWidgetId id, void* ud) {
    DemoState* d = (DemoState*)ud;
    d->clicks++;
    char buf[128];
    snprintf(buf, sizeof(buf), "Clicked widget #%u  (%d total)", id, d->clicks);
    randy_label_set_text(d->win, d->status_label, buf);
}

static void on_toggle(RandyWidgetId id, bool checked, void* ud) {
    DemoState* d = (DemoState*)ud;
    char buf[128];
    snprintf(buf, sizeof(buf), "Toggle #%u -> %s", id, checked ? "ON" : "OFF");
    randy_label_set_text(d->win, d->status_label, buf);
}

static void on_value(RandyWidgetId id, int value, void* ud) {
    DemoState* d = (DemoState*)ud;
    if (d->progress_id)
        randy_progress_set_value(d->win, d->progress_id, value);
    char buf[128];
    snprintf(buf, sizeof(buf), "Value #%u -> %d", id, value);
    randy_label_set_text(d->win, d->status_label, buf);
}

static void on_show_dialog(RandyWidgetId id, void* ud) {
    DemoState* d = (DemoState*)ud;
    (void)id;
    randy_dialog_show(d->win, d->dialog_id);
    randy_label_set_text(d->win, d->status_label, "Dialog opened");
}

static void on_close_dialog(RandyWidgetId id, void* ud) {
    DemoState* d = (DemoState*)ud;
    (void)id;
    randy_dialog_hide(d->win, d->dialog_id);
    randy_label_set_text(d->win, d->status_label, "Dialog closed");
}

/* --------------------------------------------------------------------- */
/*  main                                                                 */
/* --------------------------------------------------------------------- */

int main(void) {
    /* --- Context ---------------------------------------------------- */
    RandyContext* ctx = randy_init();
    if (!ctx) { fprintf(stderr, "init failed\n"); return 1; }

    /* Optionally load a JSONC palette (falls back to Win98 defaults) */
    RandyStyle* style = randy_style_get_active();
    randy_style_load(style, "examples/win98_gallery/win98_default.jsonc");

    RandyWindowDesc desc = {
        .title  = "randy - Win98 Widget Gallery",
        .width  = 900,
        .height = 720,
        .resizable = true,
        .decorated = true,
    };
    RandyWindow* win = randy_window_create(ctx, &desc);
    if (!win) {
        fprintf(stderr, "window_create failed\n");
        randy_shutdown(ctx);
        return 1;
    }

    DemoState demo = { .win = win };

    /* === Menu bar =================================================== */
    RandyWidgetId menubar = randy_menubar_create(win);
    RandyWidgetId mi_file = randy_menu_item_create(win, "File");
    RandyWidgetId mi_edit = randy_menu_item_create(win, "Edit");
    RandyWidgetId mi_view = randy_menu_item_create(win, "View");
    RandyWidgetId mi_help = randy_menu_item_create(win, "Help");
    randy_widget_add_child(win, menubar, mi_file);
    randy_widget_add_child(win, menubar, mi_edit);
    randy_widget_add_child(win, menubar, mi_view);
    randy_widget_add_child(win, menubar, mi_help);

    /* === Toolbar ===================================================== */
    RandyWidgetId toolbar  = randy_toolbar_create(win);
    RandyWidgetId tb_new   = randy_button_create(win, "New");
    RandyWidgetId tb_open  = randy_button_create(win, "Open");
    RandyWidgetId tb_save  = randy_button_create(win, "Save");
    randy_widget_add_child(win, toolbar, tb_new);
    randy_widget_add_child(win, toolbar, tb_open);
    randy_widget_add_child(win, toolbar, tb_save);

    /* === Tab widget â€” main content area ============================== */
    RandyWidgetId tabs = randy_tab_widget_create(win);

    /* --- Page 1: Buttons ------------------------------------------- */
    RandyWidgetId pg_btn = randy_tab_widget_add_page(win, tabs, "Buttons");

    RandyWidgetId gb_push = randy_groupbox_create(win, "Push Buttons");
    RandyWidgetId btn_ok     = randy_button_create(win, "OK");
    RandyWidgetId btn_cancel = randy_button_create(win, "Cancel");
    RandyWidgetId btn_apply  = randy_button_create(win, "Apply");
    RandyWidgetId btn_dialog = randy_button_create(win, "Open Dialog...");
    randy_widget_add_child(win, gb_push, btn_ok);
    randy_widget_add_child(win, gb_push, btn_cancel);
    randy_widget_add_child(win, gb_push, btn_apply);
    randy_widget_add_child(win, gb_push, btn_dialog);
    randy_widget_add_child(win, pg_btn, gb_push);

    RandyWidgetId gb_chk = randy_groupbox_create(win, "Checkboxes");
    RandyWidgetId cb1 = randy_checkbox_create(win, "Show grid", true);
    RandyWidgetId cb2 = randy_checkbox_create(win, "Snap to guides", false);
    RandyWidgetId cb3 = randy_checkbox_create(win, "Anti-aliasing", true);
    randy_widget_add_child(win, gb_chk, cb1);
    randy_widget_add_child(win, gb_chk, cb2);
    randy_widget_add_child(win, gb_chk, cb3);
    randy_widget_add_child(win, pg_btn, gb_chk);

    RandyWidgetId gb_rad = randy_groupbox_create(win, "Radio Buttons");
    RandyWidgetId rb1 = randy_radio_create(win, "Small icons", true);
    RandyWidgetId rb2 = randy_radio_create(win, "Large icons", false);
    RandyWidgetId rb3 = randy_radio_create(win, "List view", false);
    randy_widget_add_child(win, gb_rad, rb1);
    randy_widget_add_child(win, gb_rad, rb2);
    randy_widget_add_child(win, gb_rad, rb3);
    randy_widget_add_child(win, pg_btn, gb_rad);

    /* --- Page 2: Input Widgets ------------------------------------- */
    RandyWidgetId pg_inp = randy_tab_widget_add_page(win, tabs, "Input");

    randy_widget_add_child(win, pg_inp,
        randy_label_create(win, "Text field:"));
    RandyWidgetId txt1 = randy_textbox_create(win, "Editable text", false);
    RandyWidgetId txt2 = randy_textbox_create(win, "Read-only field", true);
    randy_widget_add_child(win, pg_inp, txt1);
    randy_widget_add_child(win, pg_inp, txt2);

    randy_widget_add_child(win, pg_inp,
        randy_label_create(win, "Spin box (0-100):"));
    RandyWidgetId spin = randy_spinbox_create(win, 0, 100, 42);
    randy_widget_add_child(win, pg_inp, spin);

    randy_widget_add_child(win, pg_inp,
        randy_label_create(win, "Combo box:"));
    RandyWidgetId combo = randy_combobox_create(win, "Option A");
    randy_widget_add_child(win, pg_inp, combo);

    randy_widget_add_child(win, pg_inp,
        randy_label_create(win, "Dropdown:"));
    RandyWidgetId dd = randy_dropdown_create(win, "Quality: High");
    randy_widget_add_child(win, pg_inp, dd);

    randy_widget_add_child(win, pg_inp,
        randy_label_create(win, "Slider (0-100):"));
    RandyWidgetId slider = randy_slider_create(win, "Volume", 0, 100, 50);
    randy_widget_add_child(win, pg_inp, slider);

    RandyWidgetId sep1 = randy_separator_create(win);
    randy_widget_add_child(win, pg_inp, sep1);

    randy_widget_add_child(win, pg_inp,
        randy_label_create(win, "Multi-line text:"));
    RandyWidgetId textedit = randy_textedit_create(win,
        "Line 1\nLine 2\nLine 3", false);
    randy_widget_add_child(win, pg_inp, textedit);

    /* --- Page 3: Item Views ---------------------------------------- */
    RandyWidgetId pg_items = randy_tab_widget_add_page(win, tabs, "Item Views");

    randy_widget_add_child(win, pg_items,
        randy_label_create(win, "List box:"));
    static const char* const fruits[] = {
        "Apple", "Banana", "Cherry", "Date", "Elderberry"
    };
    RandyWidgetId listbox = randy_listbox_create(win, 5, fruits, 0);
    randy_widget_add_child(win, pg_items, listbox);

    RandyWidgetId sep2 = randy_separator_create(win);
    randy_widget_add_child(win, pg_items, sep2);

    randy_widget_add_child(win, pg_items,
        randy_label_create(win, "Tree view:"));
    RandyWidgetId t1 = randy_tree_item_create(win, "Documents", 0, true, true);
    RandyWidgetId t2 = randy_tree_item_create(win, "report.doc", 1, false, false);
    RandyWidgetId t3 = randy_tree_item_create(win, "budget.xls", 1, false, false);
    RandyWidgetId t4 = randy_tree_item_create(win, "Pictures",   0, true, false);
    RandyWidgetId t5 = randy_tree_item_create(win, "Programs",   0, true, true);
    RandyWidgetId t6 = randy_tree_item_create(win, "notepad.exe",1, false, false);
    randy_widget_add_child(win, pg_items, t1);
    randy_widget_add_child(win, pg_items, t2);
    randy_widget_add_child(win, pg_items, t3);
    randy_widget_add_child(win, pg_items, t4);
    randy_widget_add_child(win, pg_items, t5);
    randy_widget_add_child(win, pg_items, t6);

    RandyWidgetId sep3 = randy_separator_create(win);
    randy_widget_add_child(win, pg_items, sep3);

    randy_widget_add_child(win, pg_items,
        randy_label_create(win, "Table:"));
    static const char* const hdr[]   = { "Name", "Size", "Type" };
    static const int         hdr_w[] = { 140,     60,     80 };
    RandyWidgetId th = randy_table_header_create(win, 3, hdr, hdr_w);
    randy_widget_add_child(win, pg_items, th);
    static const char* const r1[] = { "readme.txt", "4 KB",  "Text" };
    static const char* const r2[] = { "photo.jpg",  "2 MB",  "Image" };
    static const char* const r3[] = { "build.zig",  "22 KB", "Source" };
    RandyWidgetId row1 = randy_table_row_create(win, 3, r1, true);
    RandyWidgetId row2 = randy_table_row_create(win, 3, r2, false);
    RandyWidgetId row3 = randy_table_row_create(win, 3, r3, false);
    randy_widget_add_child(win, pg_items, row1);
    randy_widget_add_child(win, pg_items, row2);
    randy_widget_add_child(win, pg_items, row3);

    /* --- Page 4: Display & Containers ------------------------------ */
    RandyWidgetId pg_disp = randy_tab_widget_add_page(win, tabs, "Display");

    randy_widget_add_child(win, pg_disp,
        randy_label_create(win, "Progress bar (linked to slider on Input page):"));
    demo.progress_id = randy_progress_create(win, "Loading", 100, 50);
    randy_widget_add_child(win, pg_disp, demo.progress_id);

    RandyWidgetId sep4 = randy_separator_create(win);
    randy_widget_add_child(win, pg_disp, sep4);

    RandyWidgetId acc1 = randy_accordion_create(win, "Details (expanded)", true);
    randy_widget_add_child(win, acc1,
        randy_label_create(win, "Accordion content visible by default."));
    randy_widget_add_child(win, pg_disp, acc1);

    RandyWidgetId acc2 = randy_accordion_create(win, "More Info (collapsed)", false);
    randy_widget_add_child(win, acc2,
        randy_label_create(win, "Click the header to expand."));
    randy_widget_add_child(win, pg_disp, acc2);

    RandyWidgetId sep5 = randy_separator_create(win);
    randy_widget_add_child(win, pg_disp, sep5);

    RandyWidgetId fb1 = randy_field_border_create(win, "Editable field", false);
    RandyWidgetId fb2 = randy_field_border_create(win, "Read-only field", true);
    randy_widget_add_child(win, pg_disp, fb1);
    randy_widget_add_child(win, pg_disp, fb2);

    RandyWidgetId sp = randy_sunken_panel_create(win, "Sunken panel area");
    randy_widget_add_child(win, pg_disp, sp);

    RandyWidgetId img = randy_image_create(win, 48, 48);
    randy_widget_add_child(win, pg_disp, img);

    RandyWidgetId tip = randy_tooltip_create(win, "Tooltip: hover info");
    randy_widget_add_child(win, pg_disp, tip);

    RandyWidgetId sa = randy_scroll_area_create(win);
    randy_widget_add_child(win, sa,
        randy_label_create(win, "Content inside a scroll area."));
    randy_widget_add_child(win, pg_disp, sa);

    /* === Dialog (starts hidden) ===================================== */
    demo.dialog_id = randy_dialog_create(win, "Sample Dialog");
    randy_widget_add_child(win, demo.dialog_id,
        randy_label_create(win, "This is a modal dialog overlay."));
    RandyWidgetId dlg_close = randy_button_create(win, "Close");
    randy_widget_add_child(win, demo.dialog_id, dlg_close);

    /* === Status bar ================================================= */
    demo.status_label = randy_label_create(win, "Ready");
    randy_status_field_create(win, "Win98 Widget Gallery v1.0");

    /* === Wire up callbacks ========================================== */
    /* Buttons */
    randy_button_set_callback(win, btn_ok,     on_click, &demo);
    randy_button_set_callback(win, btn_cancel,  on_click, &demo);
    randy_button_set_callback(win, btn_apply,   on_click, &demo);
    randy_button_set_callback(win, btn_dialog,  on_show_dialog, &demo);
    randy_button_set_callback(win, dlg_close,   on_close_dialog, &demo);
    /* Toolbar */
    randy_button_set_callback(win, tb_new,  on_click, &demo);
    randy_button_set_callback(win, tb_open, on_click, &demo);
    randy_button_set_callback(win, tb_save, on_click, &demo);
    /* Menu items */
    randy_menu_item_set_callback(win, mi_file, on_click, &demo);
    randy_menu_item_set_callback(win, mi_edit, on_click, &demo);
    randy_menu_item_set_callback(win, mi_view, on_click, &demo);
    randy_menu_item_set_callback(win, mi_help, on_click, &demo);
    /* Combobox / table rows */
    randy_combobox_set_callback(win, combo, on_click, &demo);
    randy_table_row_set_callback(win, row1, on_click, &demo);
    randy_table_row_set_callback(win, row2, on_click, &demo);
    randy_table_row_set_callback(win, row3, on_click, &demo);
    /* Toggles */
    randy_checkbox_set_callback(win, cb1, on_toggle, &demo);
    randy_checkbox_set_callback(win, cb2, on_toggle, &demo);
    randy_checkbox_set_callback(win, cb3, on_toggle, &demo);
    randy_radio_set_callback(win, rb1, on_toggle, &demo);
    randy_radio_set_callback(win, rb2, on_toggle, &demo);
    randy_radio_set_callback(win, rb3, on_toggle, &demo);
    /* Values */
    randy_slider_set_callback(win, slider, on_value, &demo);
    randy_spinbox_set_callback(win, spin,  on_value, &demo);
    randy_listbox_set_callback(win, listbox, on_value, &demo);

    /* === Event loop ================================================= */
    while (!randy_window_should_close(win)) {
        randy_poll_events(ctx);
        randy_render(win);
    }

    randy_window_destroy(win);
    randy_shutdown(ctx);
    return 0;
}
