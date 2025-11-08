/* ######### INCLUDES ######### */ 

#include QMK_KEYBOARD_H
#include "swedish_keys.h"
#include "version.h"
#include <math.h>

/* ######### DEFINES ######### */

#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif 

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  SYM,
  NUM,
  NAV,  
  MOUSE,
  SYS  
};

// For mac/win compatability
enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  U_BACK_TAB,
  U_SE_LESS, 
  U_SE_GRTR,
  U_SE_PIPE,
  U_SE_LCBR,
  U_SE_RCBR,
  U_SE_BSLS,
  U_FIND_PREV,
  U_FIND_NEXT,
  U_SEARCH,
  U_REPLACE,
  U_CUT,
  U_UNDO,
  U_REDO,
  U_COPY,
  U_PASTE,
  U_SAVE,
  U_MARK_ALL,
  U_DOC_LEFT,
  U_DOC_DOWN,
  U_DOC_UP,
  U_DOC_RIGHT,   
  U_WORD_LEFT,
  U_5_ROWS_DOWN,
  U_5_ROWS_UP,
  U_WORD_RIGHT,
  U_RGB_TOG,
  U_SCREENSHOT,
  U_OS_SEARCH,
  U_EMOJIS,
  U_TOGGLE_OS,
  U_LOCK_SCREEN,
  U_SHOW_APPS,
  U_SHOW_DESKTOP,
  U_PREV_APP_WINDOW,
  U_NEXT_APP_WINDOW,
  U_NEW_APP_WINDOW,
  U_CLOSE_APP_WINDOW,  
  U_PREV_TAB,
  U_NEXT_TAB,
  U_NEW_TAB,
  U_CLOSE_TAB,
  U_APP_SWITCHER,  
  U_PREV_APP,
  U_NEXT_APP
};

typedef enum {
  OS_WINDOWS,
  OS_MAC,
} os_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os = OS_WINDOWS; // Used for storing info about the os
uint16_t os_effect_timer = 0; // Used for visualizing os switch
bool capslock_active = false; // Used for setting color for caps key leda
extern rgb_config_t rgb_matrix_config; // Global variable provided by QMK that stores the current RGB matrix settings

static bool fast_cursor_up_active = false; // Used to know if fast cursor is held
static uint16_t fast_cursor_up_timer = 0; // Used to know the time between the last cursor movement
static bool fast_cursor_down_active = false; // Used to know if fast cursor is held
static uint16_t fast_cursor_down_timer = 0; // Used to know the time between the last cursor movement

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [ALPHA] = LAYOUT_voyager(
    XXXXXXX,       XXXXXXX,            XXXXXXX,            XXXXXXX,            XXXXXXX,            XXXXXXX,      /*|*/XXXXXXX,         XXXXXXX,            XXXXXXX,            XXXXXXX,            XXXXXXX,               XXXXXXX,          
    KC_CAPS,       KC_Q,               KC_W,               KC_E,               KC_R,               KC_T,         /*|*/KC_Y,            KC_U,               KC_I,               KC_O,               KC_P,                  KC_ESCAPE,      
    CW_TOGG,       MT(MOD_LALT, KC_A), MT(MOD_LGUI, KC_S), MT(MOD_LCTL, KC_D), MT(MOD_LSFT, KC_F), KC_G,         /*|*/KC_H,            MT(MOD_RSFT, KC_J), MT(MOD_RCTL, KC_K), MT(MOD_RGUI, KC_L), MT(MOD_RALT, SE_OSLH), SE_ADIA,        
    TG(5),         KC_Z,               KC_X,               KC_C,               KC_V,               KC_B,         /*|*/KC_N,            KC_M,               KC_COMMA,           KC_DOT,             SE_AA,          KC_DELETE,      
                                                                               LT(1, KC_ENTER),    LT(3, KC_TAB),/*|*/LT(3, KC_BSPC), LT(2, KC_SPACE)
  ),                                  
  [SYM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX,   XXXXXXX, XXXXXXX,  /*|*/XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, SE_QUES, U_SE_LESS, U_SE_GRTR, SE_MINS, U_SE_PIPE,/*|*/SE_CIRC, U_SE_LCBR, U_SE_RCBR,  SE_DLR,  SE_ACUT, SE_GRV,         
    KC_PERC, KC_EXLM, SE_ASTR,   SE_SLSH,   SE_EQL,  SE_AMPR,  /*|*/KC_HASH, SE_LPRN,   SE_RPRN,    SE_SCLN, SE_DQUO, U_SE_BSLS,        
    XXXXXXX, SE_TILD, SE_PLUS,   SE_LBRC,   SE_RBRC, SE_AT,    /*|*/SE_UNDS, SE_COLN,   KC_COMMA,   KC_DOT,  SE_APOS, XXXXXXX,          
                                            XXXXXXX, XXXXXXX,  /*|*/XXXXXXX, XXXXXXX
  ),
  [NUM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, KC_F10,  KC_F3,   KC_F2,   KC_F1,   XXXXXXX,/*|*/XXXXXXX, KC_1,    KC_2,    KC_3,    XXXXXXX, XXXXXXX,          
    XXXXXXX, KC_F11,  KC_F6,   KC_F5,   KC_F4,   XXXXXXX,/*|*/XXXXXXX, KC_4,    KC_5,    KC_6,    KC_0,    XXXXXXX,           
    XXXXXXX, KC_F12,  KC_F9,   KC_F8,   KC_F7,   XXXXXXX,/*|*/XXXXXXX, KC_7,    KC_8,    KC_9,    XXXXXXX, XXXXXXX,          
                                        XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX
  ),  
  [NAV] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,     XXXXXXX,     XXXXXXX,    XXXXXXX,   XXXXXXX,      /*|*/XXXXXXX,       XXXXXXX,     XXXXXXX,       XXXXXXX,     XXXXXXX,      XXXXXXX,
    XXXXXXX, U_FIND_PREV, U_FIND_NEXT, U_SEARCH,   U_REPLACE, XXXXXXX,      /*|*/XXXXXXX,       U_DOC_LEFT,  U_DOC_DOWN,    U_DOC_UP,    U_DOC_RIGHT,  XXXXXXX,          
    XXXXXXX, U_SAVE,      U_CUT,       U_COPY,     U_PASTE,   SELWORD,      /*|*/XXXXXXX,       KC_LEFT,     KC_DOWN,       KC_UP,       KC_RIGHT,     XXXXXXX,          
    XXXXXXX, U_UNDO,      U_REDO,      U_MARK_ALL, SELLINE,   SELWBAK,      /*|*/XXXXXXX,       U_WORD_LEFT, U_5_ROWS_DOWN, U_5_ROWS_UP, U_WORD_RIGHT, XXXXXXX,          
                                                   XXXXXXX,   LT(4, KC_TAB),/*|*/LT(4,KC_BSPC), XXXXXXX     
  ), 
  [MOUSE] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          
    XXXXXXX, XXXXXXX, MS_BTN2, MS_BTN3, MS_BTN1, XXXXXXX,/*|*/XXXXXXX, MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, XXXXXXX,          
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, XXXXXXX,          
                                        XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX
  ),
  [SYS] = LAYOUT_voyager(
    XXXXXXX,       XXXXXXX,             XXXXXXX,           XXXXXXX,             XXXXXXX,             XXXXXXX,     /*|*/XXXXXXX,        XXXXXXX,           XXXXXXX,            XXXXXXX,          XXXXXXX,           XXXXXXX,
    U_TOGGLE_OS,   XXXXXXX,             KC_AUDIO_VOL_DOWN, KC_AUDIO_VOL_UP,     KC_AUDIO_MUTE,       XXXXXXX,     /*|*/XXXXXXX,        U_PREV_TAB,        U_CLOSE_TAB,        U_NEW_TAB,        U_NEXT_TAB,        KC_ESCAPE,
    U_LOCK_SCREEN, XXXXXXX,             RM_VALD,           RM_VALU,             U_RGB_TOG,           XXXXXXX,     /*|*/XXXXXXX,        U_PREV_APP,        U_SHOW_DESKTOP,     U_SHOW_APPS,      U_NEXT_APP,        XXXXXXX,
    TG(5),         KC_MEDIA_PREV_TRACK, KC_MEDIA_STOP,     KC_MEDIA_PLAY_PAUSE, KC_MEDIA_NEXT_TRACK, XXXXXXX,     /*|*/XXXXXXX,        U_PREV_APP_WINDOW, U_CLOSE_APP_WINDOW, U_NEW_APP_WINDOW, U_NEXT_APP_WINDOW, XXXXXXX,
                                                                                U_EMOJIS,            U_SCREENSHOT,/*|*/U_APP_SWITCHER, U_OS_SEARCH
  )  
};

/* ######### LEDMAPS ######### */
const HSV PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {10,245,232}, {10,245,232},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {10,245,232}, {10,245,232}
  },

  [SYM] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {135,114,167}, {206,255,153}, {206,255,153}, {91,255,160}, {135,114,167},
    {135,114,167}, {206,255,153}, {91,255,160}, {91,255,160}, {206,255,153}, {135,114,167},
    {0,0,0}, {135,114,167}, {91,255,160}, {30,241,223}, {30,241,223}, {135,114,167},
    {0,0,0}, {0,0,0},
    // Right side,
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {91,255,160}, {30,241,223}, {30,241,223}, {135,114,167}, {135,114,167}, {135,114,167},
    {135,114,167}, {30,241,223}, {30,241,223}, {135,114,167}, {135,114,167}, {135,114,167},
    {135,114,167}, {135,114,167}, {135,114,167}, {135,114,167}, {135,114,167}, {0,0,0},
    {0,0,0}, {0,0,0}
  },

  [NUM] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {183,238,205}, {183,238,205}, {183,238,205}, {183,238,205}, {0,0,0},
    {0,0,0}, {183,238,205}, {183,238,205}, {183,238,205}, {183,238,205}, {0,0,0},
    {0,0,0}, {183,238,205}, {183,238,205}, {183,238,205}, {183,238,205}, {0,0,0},
    {0,0,0}, {0,0,0},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {120,223,209}, {120,223,209}, {120,223,209}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {120,223,209}, {120,223,209}, {120,223,209}, {120,223,209}, {0,0,0}, 
    {0,0,0}, {120,223,209}, {120,223,209}, {120,223,209}, {0,0,0}, {0,0,0},
    {0,0,0}, {0,0,0}
  },
  
  [NAV] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {46,248,241}, {46,248,241}, {46,248,241}, {46,248,241}, {0,0,0}, 
    {0,0,0}, {220,238,216}, {220,238,216}, {220,238,216}, {220,238,216}, {5,201,240}, 
    {0,0,0}, {100,255,255}, {100,255,255}, {5,201,240}, {5,201,240}, {5,201,240}, 
    {0,0,0}, {0,0,180},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {169,243,216}, {169,243,216}, {169,243,216}, {169,243,216}, {0,0,0},
    {0,0,0}, {83,234,140}, {83,234,140}, {83,234,140}, {83,234,140}, {0,0,0}, 
    {0,0,0}, {18,250,229}, {18,250,229}, {18,250,229}, {18,250,229}, {0,0,0}, 
    {0,0,180}, {0,0,0}
  },

  [MOUSE] = {
  // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {137,241,207}, {137,241,207}, {137,241,207}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, 
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {71,232,162}, {71,232,162}, {71,232,162}, {71,232,162}, {0,0,0}, 
    {0,0,0}, {26,242,207}, {26,242,207}, {26,242,207}, {26,242,207}, {0,0,0}, 
    {0,0,0}, {0,0,0},
  }, 

  [SYS] = {
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {85,255,100}, {0,0,0}, {101,238,158}, {101,238,158}, {101,238,158}, {0,0,0}, 
    {0,255,100}, {0,0,0}, {30,239,216}, {30,239,216}, {30,239,216}, {0,0,0},
    {0,0,100}, {180,255,255}, {180,255,255}, {180,255,255}, {180,255,255}, {0,0,0}, 
    {240,218,204}, {240,218,204},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {120,218,204}, {120,218,204}, {120,218,204}, {120,218,204}, {0,0,100},
    {0,0,0}, {70,218,255}, {70,218,255}, {70,218,255}, {70,218,255}, {0,0,0}, 
    {0,0,0}, {14,235,217}, {14,235,217}, {14,235,217}, {14,235,217}, {0,0,0}, 
    {70,218,255}, {70,218,255}
  }
};

const HSV PROGMEM ledmap_alt[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {83,245,131}, {83,245,131},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {83,245,131}, {83,245,131}
  }
};

/* ######### LED CONTROL FUNCTIONS ######### */

uint16_t hsv_value_for_animation(uint8_t index, uint8_t max_v) {
  uint16_t period = 500;
  uint16_t period_offset = 100;
  uint16_t phase_offset = index * period_offset;
  uint16_t t = (timer_read() + phase_offset) % period;
  uint16_t value = (t < period/2) 
    ? (t * max_v) / (period/2) 
    : max_v - ((t - period/2) * max_v) / (period/2);

  return value;
}

RGB hsv_to_rgb_with_value(HSV hsv) {
  RGB rgb = hsv_to_rgb( hsv );
  float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
  return (RGB){ f * rgb.r, f * rgb.g, f * rgb.b };
}

static HSV pgm_read_hsv(const HSV *addr) {
  // Reads RGB struct from PROGMEM safely
  HSV hsv;
  hsv.h = pgm_read_byte(&addr->h);
  hsv.s = pgm_read_byte(&addr->s);
  hsv.v = pgm_read_byte(&addr->v);
  return hsv;
}

HSV pgm_read_hsv_for_layer(uint8_t layer, uint8_t index) {
  HSV hsv;
  if (layer == ALPHA && capslock_active && index == 6) {
    hsv = (HSV) { 0, 0, 180 };
  } else if (layer == ALPHA && current_os == OS_MAC) {
    hsv = pgm_read_hsv(&ledmap_alt[layer][index]);
  } else {
    hsv = pgm_read_hsv(&ledmap[layer][index]);
  } 
  return hsv;
}

bool try_set_leds_for_layer_with_animation(uint8_t layer) {
  if (!os_effect_timer) {
    return false;
  } 

  if (timer_elapsed(os_effect_timer) > 5000) {
    os_effect_timer = 0; 
  }

  // Set the runtime buffer from PROGMEM for a given layer, and sets pulsating value
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {    
    HSV hsv = pgm_read_hsv_for_layer(layer, i);
    hsv.v = hsv.v != 0 ? hsv_value_for_animation(i, hsv.v) : 0;
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
  }

  return true;
}

void set_leds_for_layer(uint8_t layer) {
  // Set the runtime buffer from PROGMEM for a given layer
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = pgm_read_hsv_for_layer(layer, i);
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
  }
}

bool rgb_matrix_indicators_user(void) {
  uint8_t active_layer = biton32(layer_state);
  if (try_set_leds_for_layer_with_animation(active_layer)) {
    return false;
  } else {
    set_leds_for_layer(active_layer);
  }
  return true;
}

bool led_update_user(led_t led_state) {
  // Called when the caps/num/scroll lock state changes
  capslock_active = led_state.caps_lock; 
  return true;
}

/* ######### EEPROM STATE ######### */

void load_os(void) {
  // Load OS from EEPROM
  if (!eeconfig_is_enabled()) {
    eeconfig_init();
    current_os = OS_WINDOWS;
    eeconfig_update_user(current_os);
  } else {
    current_os = (uint8_t)eeconfig_read_user();
  }
}

/* ######### INITIALIZATION HOOK ######### */

void keyboard_post_init_user(void) {
  // Called after keyboard finishes intitialization
  rgb_matrix_enable(); // enable rgb matrix in qmk
  load_os(); // Read OS
}

/* ######### TAPPING TERM ######### */

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  return record->event.key.row == 4
    ? TAPPING_TERM_THUMBS
    : TAPPING_TERM;
}

/* ######### OS FUNCTIONALITY ######### */

#define PERFORM_BY_OS(win_action, mac_action) \
  do {                                        \
    if (current_os == OS_WINDOWS) {           \
      win_action;                             \
    } else {                                  \
      mac_action;                             \
    }                                         \
  } while (0)

void fast_cursor_move_up(uint16_t lines) {
  for (int i = 0; i < lines; i++) {
    tap_code(KC_UP);
  }
}

void fast_cursor_move_down(uint16_t lines) {
  for (int i = 0; i < lines; i++) {
    tap_code(KC_DOWN);
  }
}

bool select_word_host_is_mac(void) {
  // Getreur function for word selection
  return current_os == OS_MAC;
}

void flip_os(void) {
  os_effect_timer = timer_read();
  current_os = current_os == OS_MAC ? OS_WINDOWS : OS_MAC;
  eeconfig_update_user(current_os);
}

void switch_app_win(void) {
  register_code(KC_LALT); 
  tap_code16(KC_TAB);
}

void switch_app_mac(void) {
  register_code(KC_LGUI); 
  tap_code16(KC_TAB); 
}

bool process_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case RGB_SLD:            rgblight_mode(1);                                                    break;
    case U_RGB_TOG:          rgb_matrix_toggle();                                                 return false;
    case U_TOGGLE_OS:        (flip_os(), layer_off(5));                                           return false;
    case U_SE_LESS:          PERFORM_BY_OS(tap_code16(SE_LESS_WIN),  tap_code16(SE_LESS_MAC));    break;
    case U_SE_GRTR:          PERFORM_BY_OS(tap_code16(SE_GRTR_WIN),  tap_code16(SE_GRTR_MAC));    break;
    case U_SE_PIPE:          PERFORM_BY_OS(tap_code16(SE_PIPE_WIN),  tap_code16(SE_PIPE_MAC));    break;
    case U_SE_LCBR:          PERFORM_BY_OS(tap_code16(SE_LCBR_WIN),  tap_code16(SE_LCBR_MAC));    break;
    case U_SE_RCBR:          PERFORM_BY_OS(tap_code16(SE_RCBR_WIN),  tap_code16(SE_RCBR_MAC));    break;
    case U_SE_BSLS:          PERFORM_BY_OS(tap_code16(SE_BSLS_WIN),  tap_code16(SE_BSLS_MAC));    break;   
    case U_FIND_PREV:        PERFORM_BY_OS(tap_code16(S(KC_F3)),     tap_code16(G(S(KC_G))));     break;
    case U_FIND_NEXT:        PERFORM_BY_OS(tap_code16(C(KC_G)),      tap_code16(G(KC_G)));        break;
    case U_SEARCH:           PERFORM_BY_OS(tap_code16(C(KC_F)),      tap_code16(G(KC_F)));        break;
    case U_REPLACE:          PERFORM_BY_OS(tap_code16(C(KC_H)),      tap_code16(A(G(KC_F))));     break;
    case U_CUT:              PERFORM_BY_OS(tap_code16(C(KC_X)),      tap_code16(G(KC_X)));        break;
    case U_COPY:             PERFORM_BY_OS(tap_code16(C(KC_C)),      tap_code16(G(KC_C)));        break;
    case U_PASTE:            PERFORM_BY_OS(tap_code16(C(KC_V)),      tap_code16(G(KC_V)));        break;
    case U_UNDO:
      PERFORM_BY_OS(
        (register_code(KC_LCTL), register_code(KC_Z)),
        (register_code(KC_LGUI), register_code(KC_Z))
      );
      break;
    case U_REDO:
      PERFORM_BY_OS(
        (register_code(KC_LCTL), register_code(KC_LSFT), register_code(KC_Z)),
        (register_code(KC_LGUI), register_code(KC_LSFT), register_code(KC_Z))
      );
      break;
    case U_SAVE:             PERFORM_BY_OS(tap_code16(C(KC_S)),      tap_code16(G(KC_S)));        break;
    case U_MARK_ALL:         PERFORM_BY_OS(tap_code16(C(KC_A)),      tap_code16(G(KC_A)));        break;
    case U_DOC_LEFT:         PERFORM_BY_OS(tap_code(KC_HOME),        tap_code16(G(KC_LEFT)));     break;
    case U_DOC_DOWN:         PERFORM_BY_OS(tap_code16(C(KC_END)),    tap_code16(G(KC_DOWN)));     break;
    case U_DOC_UP:           PERFORM_BY_OS(tap_code16(C(KC_HOME)),   tap_code16(G(KC_UP)));       break;
    case U_DOC_RIGHT:        PERFORM_BY_OS(tap_code(KC_END),         tap_code16(G(KC_RIGHT)));    break;
    case U_WORD_LEFT:
      PERFORM_BY_OS(
          (register_code(KC_LCTL), register_code(KC_LEFT)),
          (register_code(KC_LALT), register_code(KC_LEFT))
      );
      break;
    case U_WORD_RIGHT:
      PERFORM_BY_OS(
          (register_code(KC_LCTL), register_code(KC_RIGHT)),
          (register_code(KC_LALT), register_code(KC_RIGHT))
      );
      break;
    case U_5_ROWS_DOWN:      
      (
        fast_cursor_down_active = true, 
        fast_cursor_down_timer = timer_read(), 
        fast_cursor_move_down(5)
      );
      return false;
    case U_5_ROWS_UP:        
      (
        fast_cursor_up_active = true, 
        fast_cursor_up_timer = timer_read(), 
        fast_cursor_move_up(5)
      );
      return false;
    case U_SCREENSHOT:       PERFORM_BY_OS(tap_code16(G(S(KC_S))),   tap_code16(G(S(KC_4))));     break;
    case U_OS_SEARCH:        PERFORM_BY_OS(tap_code16(G(S(KC_S))),   tap_code16(G(KC_SPACE)));    break;
    case U_LOCK_SCREEN:      
      PERFORM_BY_OS(
        (tap_code16(G(KC_L)), layer_off(5)),      
        (tap_code16(C(G(KC_Q))), layer_off(5))
      );     
      break;
    case U_EMOJIS:           PERFORM_BY_OS(tap_code16(G(KC_DOT)),    tap_code16(C(G(KC_SPACE)))); break;
    case U_SHOW_APPS:        PERFORM_BY_OS(tap_code16(G(KC_TAB)),    tap_code16(C(KC_UP)));       break;
    case U_SHOW_DESKTOP:     PERFORM_BY_OS(tap_code16(G(KC_D)),      tap_code16(KC_F11));         break;
    case U_PREV_APP_WINDOW:  PERFORM_BY_OS(tap_code16(S(C(KC_TAB))), tap_code16(S(G(KC_GRV))));   break;
    case U_NEXT_APP_WINDOW:  PERFORM_BY_OS(tap_code16(C(KC_TAB)),    tap_code16(G(KC_GRV)));      break;
    case U_NEW_APP_WINDOW:   PERFORM_BY_OS(tap_code16(C(KC_N)),      tap_code16(G(KC_N)));        break;
    case U_CLOSE_APP_WINDOW: PERFORM_BY_OS(tap_code16(C(KC_W)),      tap_code16(G(KC_W)));        break;
    case U_PREV_TAB:         PERFORM_BY_OS(tap_code16(S(C(KC_TAB))), tap_code16(G(A(KC_LEFT))));  break;
    case U_NEXT_TAB:         PERFORM_BY_OS(tap_code16(C(KC_TAB)),    tap_code16(G(A(KC_RIGHT)))); break;
    case U_NEW_TAB:          PERFORM_BY_OS(tap_code16(C(KC_T)),      tap_code16(G(KC_T)));        break;
    case U_CLOSE_TAB:        PERFORM_BY_OS(tap_code16(C(KC_W)),      tap_code16(G(KC_W)));        break;
    case U_APP_SWITCHER:     PERFORM_BY_OS(switch_app_win(),         switch_app_mac());           return false;    
    case U_PREV_APP:         tap_code16(S(KC_TAB));                                               break;
    case U_NEXT_APP:         tap_code(KC_TAB);                                                    break;
  }
  return true;
}

bool process_non_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case U_5_ROWS_DOWN:      
      fast_cursor_down_active = false;
      fast_cursor_down_timer = 0;              
      break;
    case U_5_ROWS_UP:
      fast_cursor_up_active = false;
      fast_cursor_up_timer = 0;        
      break;
    case U_UNDO:
      PERFORM_BY_OS(
        (unregister_code(KC_Z), unregister_code(KC_LCTL)),
        (unregister_code(KC_Z), unregister_code(KC_LGUI))
      );
      break;
    case U_REDO:
      PERFORM_BY_OS(
        (unregister_code(KC_Z), unregister_code(KC_LSFT), unregister_code(KC_LCTL)),
        (unregister_code(KC_Z), unregister_code(KC_LSFT), unregister_code(KC_LGUI))
      );
      break;    
    case U_WORD_LEFT:
      PERFORM_BY_OS(
        (unregister_code(KC_LEFT), unregister_code(KC_LCTL)),
        (unregister_code(KC_LEFT), unregister_code(KC_LALT))
      );
      break;
    case U_WORD_RIGHT:
      PERFORM_BY_OS(
        (unregister_code(KC_RIGHT), unregister_code(KC_LCTL)),
        (unregister_code(KC_RIGHT), unregister_code(KC_LALT)) 
      );  
      break;  
    case U_APP_SWITCHER: PERFORM_BY_OS(unregister_code(KC_LALT), unregister_code(KC_LGUI)); return false;
  }
  return true;
}

/* ######### MAIN KEY PROCESSING ######### */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // As simplified as possible now
  return record->event.pressed 
    ? process_pressed_keycode(keycode)
    : process_non_pressed_keycode(keycode);
}

void matrix_scan_user(void) {
    if (fast_cursor_up_active && timer_elapsed(fast_cursor_up_timer) > FAST_CURSOR_INTERVAL) {
        fast_cursor_move_up(5);
        fast_cursor_up_timer = timer_read();
    }

    if (fast_cursor_down_active && timer_elapsed(fast_cursor_down_timer) > FAST_CURSOR_INTERVAL) {
        fast_cursor_move_down(5);
        fast_cursor_down_timer = timer_read();
    }
}