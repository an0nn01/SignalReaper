#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>

// Starea aplicatiei
typedef enum {
    MainMenu,
    SelectMode,
    ReadMode,
    SavedSignalsMenu,
    EmulateMenu
} AppState;

static AppState current_state = MainMenu;
static uint8_t menu_position = 0;
static bool is_running = true;
static FuriString* status_text;
static FuriString* file_list_str;
static uint8_t saved_signal_count = 0;

// Functii de desen
static void draw_main_menu(Canvas* canvas);
static void draw_select_mode_menu(Canvas* canvas);
static void draw_read_menu(Canvas* canvas);
static void draw_saved_signals_menu(Canvas* canvas);
static void draw_emulate_menu(Canvas* canvas);
static void draw_status(Canvas* canvas);

// Functii helper
static void draw_menu(Canvas* canvas, const char* title, const char* items[], uint8_t count);

// Functii de atac (placeholdere - logica reala este complexa)
static void start_rf_capture();
static void start_ir_capture();
static void start_rfid_read();
static void load_saved_signals();
static void emulate_rf_signal();
static void emulate_ir_signal();
static void emulate_rfid_signal();
static void delete_current_signal();

static void input_callback(InputEvent* input_event, void* ctx) {
    UNUSED(ctx);
    if(input_event->type == InputTypeShort) {
        switch(input_event->key) {
            case InputKeyBack:
                if(!furi_string_empty(status_text)) {
                    furi_string_reset(status_text);
                } else if(current_state != MainMenu) {
                    current_state = MainMenu;
                    menu_position = 0;
                } else {
                    is_running = false;
                }
                break;
            case InputKeyUp:
                if(menu_position > 0) menu_position--;
                break;
            case InputKeyDown:
                if(current_state == SelectMode && menu_position < 2) menu_position++;
                else if(current_state == ReadMode && menu_position < 2) menu_position++;
                else if(current_state == SavedSignalsMenu && menu_position < saved_signal_count -1) menu_position++;
                else if(current_state == EmulateMenu && menu_position < 2) menu_position++;
                break;
            case InputKeyOk:
                if(current_state == MainMenu) {
                    current_state = SelectMode;
                    menu_position = 0;
                } else if(current_state == SelectMode) {
                    if(menu_position == 0) { current_state = ReadMode; menu_position = 0; }
                    else if(menu_position == 1) { current_state = SavedSignalsMenu; load_saved_signals(); menu_position = 0; }
                    else if(menu_position == 2) { current_state = EmulateMenu; menu_position = 0; }
                } else if(current_state == ReadMode) {
                    if(menu_position == 0) start_rf_capture();
                    else if(menu_position == 1) start_ir_capture();
                    else if(menu_position == 2) start_rfid_read();
                } else if(current_state == SavedSignalsMenu) {
                    // Placeholder for selecting a file to emulate/delete
                    furi_string_set(status_text, "Selected a signal.");
                } else if(current_state == EmulateMenu) {
                    if(menu_position == 0) emulate_rf_signal();
                    else if(menu_position == 1) emulate_ir_signal();
                    else if(menu_position == 2) emulate_rfid_signal();
                }
                break;
        }
    }
}

// --- Implementari functii de desen ---
static void draw_menu(Canvas* canvas, const char* title, const char* items[], uint8_t count) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, title);
    canvas_set_font(canvas, FontSecondary);
    for(int i = 0; i < count; i++) {
        canvas_draw_str(canvas, 5, 30 + i * 12, items[i]);
        if(i == menu_position) {
            canvas_draw_str(canvas, 2, 30 + i * 12, ">";
        }
    }
}

static void draw_main_menu(Canvas* canvas) {
    draw_menu(canvas, "SignalReaper", (const char*[]){"Start", "About"}, 2);
}

static void draw_select_mode_menu(Canvas* canvas) {
    draw_menu(canvas, "Select Mode", (const char*[]){
        "Read Signal", "Saved Signals", "Emulate Signal"
    }, 3);
}

static void draw_read_menu(Canvas* canvas) {
    draw_menu(canvas, "Read From:", (const char*[]){
        "Sub-GHz (RF)", "IR (Credit)", "RFID (Card)"
    }, 3);
}

static void draw_saved_signals_menu(Canvas* canvas) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, "Saved Signals");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 35, furi_string_get_cstr(file_list_str));
    canvas_draw_str(canvas, 2, 60, "Press Back to return.");
}

static void draw_emulate_menu(Canvas* canvas) {
    draw_menu(canvas, "Emulate As:", (const char*[]){
        "Sub-GHz (RF)", "IR (Credit)", "RFID (Card)"
    }, 3);
}

static void draw_status(Canvas* canvas) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, "SignalReaper Status");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 35, furi_string_get_cstr(status_text));
    canvas_draw_str(canvas, 2, 50, "Press Back to return.");
}

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    switch(current_state) {
        case MainMenu: draw_main_menu(canvas); break;
        case SelectMode: draw_select_mode_menu(canvas); break;
        case ReadMode: draw_read_menu(canvas); break;
        case SavedSignalsMenu: draw_saved_signals_menu(canvas); break;
        case EmulateMenu: draw_emulate_menu(canvas); break;
    }
    if(!furi_string_empty(status_text)) {
        draw_status(canvas);
    }
}


// --- Implementari functii de atac (PLACEHOLDERS) ---
// ACESTEA SUNT DOAR_AFISARI. LOGICA REALA NECESITA BIBLIOTECI FLIPPER
static void start_rf_capture() {
    furi_string_set(status_text, "RF: Listening for signal...");
    // LOGICA REALA: foloseste libraria furi_hal/subghz pentru a incepe captura
    // asteapta semnalul, salveaza fisierul .sub
    furi_string_printf(status_text, "RF: Signal saved!");
}

static void start_ir_capture() {
    furi_string_set(status_text, "IR: Point remote/cell...");
    // LOGICA REALA: Foloseste furi_hal/infrared cu o bucla pentru a captura
    // o secvența lunga de impulsuri in loc de o singura comanda
    furi_string_printf(status_text, "IR: Sequence saved!");
}

static void start_rfid_read() {
    furi_string_set(status_text, "RFID: Hold card to reader...");
    // LOGICA REALA: foloseste libraria lfrfid pentru a citi cardul
    furi_string_printf(status_text, "RFID: Card saved!");
}

static void load_saved_signals() {
    // LOGICA REALA: foloseste storage/filesystem pentru a lista fisierele
    // din /ext/infrared, /ext/subghz, /ext/lfrfid
    // si le salveaza in `file_list_str`. Calculeaza `saved_signal_count`.
    furi_string_set(file_list_str, "signal_01.sub\nsignal_02.ir\nmy_card.rfid");
    saved_signal_count = 3;
}

static void emulate_rf_signal() {
    furi_string_set(status_text, "RF: Emulating...");
    // LOGICA REALA: incarca fisierul .sub si trimite-l
}

static void emulate_ir_signal() {
    furi_string_set(status_text, "IR: Emulating...");
    // LOGICA REALA: incarca fisierul .ir si trimite secventa. Sincronizeaza cu un LED IR extern.
    //Folosind AIO Desk v1.4, poti comanda un LED extern pin.
}

static void emulate_rfid_signal() {
    furi_string_set(status_text, "RFID: Emulating...");
    // LOGICA REALA: foloseste libraria lfrfid pentru a emula cardul
}


int32_t signal_reaper_app(void* p) {
    UNUSED(p);
    is_running = true;
    current_state = MainMenu;
    menu_position = 0;
    status_text = furi_string_alloc();
    file_list_str = furi_string_alloc();

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_display_backlight_on);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, NULL);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(is_running) furi_delay_ms(50);

    furi_string_free(status_text);
    furi_string_free(file_list_str);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    return 0;
}
