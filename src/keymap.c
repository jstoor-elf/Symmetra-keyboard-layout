/* ######### INCLUDES ######### */ 

#include QMK_KEYBOARD_H
#include "swedish_keys.h"
#include "version.h"

/* ######### DEFINES ######### */

#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif 

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  SYM,
  NAV,  
  MOUSE,
  NUM,
  SYS  
};

// For mac/win compatability
enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
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
  U_REPLACE_ALL,
  U_UNDO,
  U_REDO,
  U_COPY,
  U_PASTE,
  U_SAVE,
  U_MARK_ALL,
  U_MARK_LINE,
  U_MARK_WORD,
  U_DOC_LEFT,
  U_DOC_DOWN,
  U_DOC_UP,
  U_DOC_RIGHT,   
  U_WORD_LEFT,
  U_PARA_DOWN,
  U_PARA_UP,
  U_WORD_RIGHT, 
  U_SCREENSHOT,
  U_OS_SEARCH,
  U_EMOJIS,
  U_THUMBS_UP_EMOJI,
  U_TOGGLE_OS,
  U_TERMINAL,
  U_BROWSER,
  U_LOCK_SCREEN,
  U_SHOW_APPS,
  U_SHOW_DESKTOP,  
  U_PREV_APP,
  U_NEXT_APP,
  U_PREV_APP_WINDOW,
  U_NEXT_APP_WINDOW,
  U_APP_SWITCHER,
  U_PREV_TAB,
  U_NEXT_TAB,
  U_NEW_TAB,
  U_CLOSE_TAB    
};

typedef enum {
  OS_WINDOWS,
  OS_MAC,
} os_t;

/* ######### GLOBAL VARIABLES ######### */

static RGB runtime_led_buffer[RGB_MATRIX_LED_COUNT] = {0}; // Define a runtime buffer for your LEDs
os_t current_os = OS_WINDOWS; // Used for storing info about the os
bool capslock_active = false; // Used for setting color for caps key lede
extern rgb_config_t rgb_matrix_config; // Global variable provided by QMK that stores the current RGB matrix settings

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [ALPHA] = LAYOUT_voyager(
    XXXXXXX,       XXXXXXX,            XXXXXXX,            XXXXXXX,            XXXXXXX,            XXXXXXX,        /*|*/XXXXXXX,         XXXXXXX,            XXXXXXX,            XXXXXXX,              XXXXXXX,               XXXXXXX,          
    OSM(MOD_LSFT), KC_Q,               KC_W,               KC_E,               KC_R,               KC_T,           /*|*/KC_Y,            KC_U,               KC_I,               KC_O,                 KC_P,                  KC_ESCAPE,      
    CW_TOGG,       MT(MOD_LALT, KC_A), LT(4, KC_S),        MT(MOD_LCTL, KC_D), MT(MOD_LSFT, KC_F), KC_G,           /*|*/KC_H,            MT(MOD_RSFT, KC_J), MT(MOD_RCTL, KC_K), LT(4, KC_L),          MT(MOD_RALT, SE_OSLH), SE_ADIA,        
    KC_CAPS,       LT(5, KC_Z),        MT(MOD_LGUI, KC_X), KC_C,               KC_V,               KC_B,           /*|*/KC_N,            KC_M,               KC_COMMA,           MT(MOD_RGUI, KC_DOT), LT(5, SE_AA),          KC_DELETE,      
                                                                                LT(1, KC_TAB),     LT(2, KC_ENTER),/*|*/LT(2, KC_SPACE), LT(1, KC_BSPC)
  ),
  [SYM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX,   XXXXXXX, XXXXXXX,  /*|*/XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, SE_QUES, U_SE_LESS, U_SE_GRTR, SE_MINS, U_SE_PIPE,/*|*/SE_CIRC, U_SE_LCBR, U_SE_RCBR,  SE_DLR,  SE_ACUT, SE_GRV,         
    KC_PERC, KC_EXLM, SE_ASTR,   SE_SLSH,   SE_EQL,  SE_AMPR,  /*|*/KC_HASH, SE_LPRN,   SE_RPRN,    SE_SCLN, SE_DQUO, U_SE_BSLS,        
    XXXXXXX, SE_TILD, SE_PLUS,   SE_LBRC,   SE_RBRC, SE_AT,    /*|*/SE_UNDS, SE_COLN,   KC_COMMA,   KC_DOT,  SE_APOS, XXXXXXX,          
                                            XXXXXXX, XXXXXXX,  /*|*/XXXXXXX, XXXXXXX
  ),
  [NAV] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,     XXXXXXX,     XXXXXXX,     XXXXXXX,   XXXXXXX,      /*|*/XXXXXXX, XXXXXXX,     XXXXXXX,     XXXXXXX,  XXXXXXX,      XXXXXXX,
    XXXXXXX, U_FIND_PREV, U_FIND_NEXT, U_SEARCH,    U_REPLACE, U_REPLACE_ALL,/*|*/XXXXXXX, U_DOC_LEFT,  U_DOC_DOWN,  U_DOC_UP, U_DOC_RIGHT,  XXXXXXX,          
    XXXXXXX, U_UNDO,      U_REDO,      U_COPY,      U_PASTE,   U_SAVE,       /*|*/XXXXXXX, KC_LEFT,     KC_DOWN,     KC_UP,    KC_RIGHT,     XXXXXXX,          
    XXXXXXX, U_MARK_ALL,  U_MARK_LINE, U_MARK_WORD, MOD_LSFT,  XXXXXXX,      /*|*/XXXXXXX, U_WORD_LEFT, U_PARA_DOWN, U_PARA_UP,U_WORD_RIGHT, XXXXXXX,          
                                                    MO(3),     XXXXXXX,      /*|*/XXXXXXX, MO(3)
  ),
  [MOUSE] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          
    XXXXXXX, QK_LLCK, MS_BTN2, MS_BTN3, MS_BTN1, XXXXXXX,/*|*/XXXXXXX, MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, XXXXXXX,          
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, XXXXXXX,          
                                        XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX
  ),  
  [NUM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          
    KC_F5,   KC_F4,   KC_F3,   KC_F2,   KC_F1,   XXXXXXX,/*|*/XXXXXXX, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,           
    KC_F10,  KC_F9,   KC_F8,   KC_F7,   KC_F6,   XXXXXXX,/*|*/XXXXXXX, KC_6,    KC_7,    KC_8,    KC_9,    XXXXXXX,          
                                        KC_F11,  KC_F12, /*|*/XXXXXXX, KC_0
  ),
  [SYS] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,             XXXXXXX,             XXXXXXX,       XXXXXXX,             XXXXXXX,/*|*/XXXXXXX,      XXXXXXX,           XXXXXXX,           XXXXXXX,     XXXXXXX,        XXXXXXX,
    XXXXXXX, KC_AUDIO_VOL_DOWN,   KC_AUDIO_VOL_UP,     XXXXXXX,       KC_AUDIO_MUTE,       XXXXXXX,/*|*/U_BROWSER,    U_PREV_TAB,        U_NEXT_TAB,        U_NEW_TAB,   U_CLOSE_TAB,    U_LOCK_SCREEN,          
    XXXXXXX, RM_VALD,             RM_VALU,             XXXXXXX,       RM_TOGG,             XXXXXXX,/*|*/U_TERMINAL,   U_PREV_APP,        U_NEXT_APP,        U_SHOW_APPS, U_SHOW_DESKTOP, XXXXXXX,          
    XXXXXXX, KC_MEDIA_PREV_TRACK, KC_MEDIA_NEXT_TRACK, KC_MEDIA_STOP, KC_MEDIA_PLAY_PAUSE, XXXXXXX,/*|*/U_EMOJIS,     U_PREV_APP_WINDOW, U_NEXT_APP_WINDOW, U_OS_SEARCH, XXXXXXX,        U_TOGGLE_OS,          
                                                                      XXXXXXX,             XXXXXXX,/*|*/U_SCREENSHOT, U_THUMBS_UP_EMOJI
  )  
};

/* ######### LED MAPS ######### */

// Using HSV (Hue, Saturation, Value) instead of direct RGB allows for:
// 1. Easier color adjustments: changing the hue or brightness of a layer can be done
//    by modifying a single component without recalculating RGB values manually.
// 2. Consistent brightness control: the 'value' component can be scaled according
//    to the global RGB brightness setting (rgb_matrix_config.hsv.v), ensuring
//    uniform LED intensity across all keys.
// 3. More intuitive color mapping: HSV aligns better with how humans perceive colors,
//    making it easier to pick visually distinct and aesthetically pleasing colors
//    for different layers, statuses (e.g., Caps Lock), or OS-specific keys.

const RGB PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171},
    {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171},
    {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171},
    {67,240,171}, {67,240,171},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171},
    {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171},
    {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171}, {67,240,171},
    {67,240,171}, {67,240,171}
  },

  [SYM] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {135,114,167}, {206,255,153}, {206,255,153}, {91,255,160}, {135,114,167},
    {135,114,167}, {206,255,153}, {91,255,160}, {91,255,160}, {206,255,153}, {135,114,167},
    {0,0,0}, {135,114,167}, {91,255,160}, {30,241,223}, {30,241,223}, {135,114,167},
    {0,0,0}, {0,0,0},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {91,255,160}, {30,241,223}, {30,241,223}, {135,114,167}, {135,114,167}, {135,114,167},
    {135,114,167}, {30,241,223}, {30,241,223}, {135,114,167}, {135,114,167}, {135,114,167},
    {135,114,167}, {135,114,167}, {135,114,167}, {135,114,167}, {135,114,167}, {0,0,0},
    {0,0,0}, {0,0,0}
  },

  [NAV] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {46,248,241}, {46,248,241}, {46,248,241}, {46,248,241}, {46,248,241}, 
    {0,0,0}, {220,238,216}, {220,238,216}, {220,238,216}, {220,238,216}, {220,238,216}, 
    {0,0,0}, {139,241,220}, {139,241,220}, {139,241,220}, {139,241,220}, {0,0,0}, 
    {0,240,171}, {0,0,0}, 
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {169,243,216}, {169,243,216}, {169,243,216}, {169,243,216}, {0,0,0}, 
    {0,0,0}, {83,234,140}, {83,234,140}, {83,234,140}, {83,234,140}, {0,0,0}, 
    {0,0,0}, {18,250,229}, {18,250,229}, {18,250,229}, {18,250,229}, {0,0,0}, 
    {0,0,0}, {0,240,171} 
  },

  [MOUSE] = {
  // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {208,241,224}, {137,241,207}, {137,241,207}, {137,241,207}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, 
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {71,232,162}, {71,232,162}, {71,232,162}, {71,232,162}, {0,0,0}, 
    {0,0,0}, {26,242,207}, {26,242,207}, {26,242,207}, {26,242,207}, {0,0,0}, 
    {0,0,0}, {0,0,0} 
  },

  [NUM] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {253,238,205}, {253,238,205}, {253,238,205}, {253,238,205}, {253,238,205}, {0,0,0}, 
    {253,238,205}, {253,238,205}, {253,238,205}, {253,238,205}, {253,238,205}, {0,0,0}, 
    {253,238,205}, {253,238,205}, 
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {190,223,209}, {190,223,209}, {190,223,209}, {190,223,209}, {190,223,209}, 
    {0,0,0}, {190,223,209}, {190,223,209}, {190,223,209}, {190,223,209}, {0,0,0}, 
    {0,0,0}, {190,223,209}
  }, 

  [SYS] = {
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {101,238,158}, {101,238,158}, {0,0,0}, {101,238,158}, {0,0,0}, 
    {0,0,0}, {40,239,216}, {40,239,216}, {0,0,0}, {40,239,216}, {0,0,0}, 
    {0,0,0}, {170,255,255}, {170,255,255}, {170,255,255}, {170,255,255}, {0,0,0}, 
    {0,0,0}, {0,0,0}, 
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {120,112,158}, {120,112,158}, {120,112,158}, {120,112,158}, {120,112,158}, {234,225,167},
    {120,112,158}, {120,112,158}, {120,112,158}, {120,112,158}, {120,112,158}, {0,0,0}, 
    {120,112,158}, {120,112,158}, {120,112,158}, {120,112,158}, {0,0,0}, {234,225,167}, 
    {120,112,158}, {120,112,158} 
  }
};

// Os key rgb indices and rgb values
const uint8_t W_RGB_KEY_INDEX         = 8;
const uint8_t I_RGB_KEY_INDEX         = 34;
const uint8_t N_RGB_KEY_INDEX         = 44;
const uint8_t M_RGB_KEY_INDEX         = 45;
const uint8_t A_RGB_KEY_INDEX         = 13;
const uint8_t C_RGB_KEY_INDEX         = 21;
const uint8_t CAPSLOCK_RGB_KEY_INDEX  = 18;

const RGB RGB_OS_WIN   = {70, 130, 180};
const RGB RGB_OS_MAC   = {255, 191, 0};
const RGB RGB_CAPSLOCK = {114, 47, 55};

/* ######### LED CONTROL FUNCTIONS ######### */

static RGB apply_brightness(RGB in) {
  // Apply QMK brightness (v = 0–255) directly
  uint8_t v = rgb_matrix_config.hsv.v;
  RGB out;
  out.r = ((uint16_t)in.r * v) / 255;
  out.g = ((uint16_t)in.g * v) / 255;
  out.b = ((uint16_t)in.b * v) / 255;
  return out;
}

static RGB pgm_read_rgb(const RGB *addr) {
  // Reads RGB struct from PROGMEM safely
  RGB c;
  c.r = pgm_read_byte(&addr->r);
  c.g = pgm_read_byte(&addr->g);
  c.b = pgm_read_byte(&addr->b);
  return c;
}

void set_led_buffer_for_layer(uint8_t layer) {
  // Set the runtime buffer from PROGMEM for a given layer
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    RGB tmp = pgm_read_rgb(&ledmap[layer][i]);
    runtime_led_buffer[i] = apply_brightness(tmp);
  }

  // Change alpha layer
  if (layer == ALPHA) {
    if (capslock_active) {
      RGB rgb = apply_brightness(RGB_CAPSLOCK);
      runtime_led_buffer[CAPSLOCK_RGB_KEY_INDEX] = rgb;
    }

    if (current_os == OS_WINDOWS) {
      RGB rgb = apply_brightness(RGB_OS_WIN);
      runtime_led_buffer[W_RGB_KEY_INDEX] = rgb;
      runtime_led_buffer[I_RGB_KEY_INDEX] = rgb;
      runtime_led_buffer[N_RGB_KEY_INDEX] = rgb;
    } else {
      RGB rgb = apply_brightness(RGB_OS_MAC);
      runtime_led_buffer[M_RGB_KEY_INDEX] = rgb;
      runtime_led_buffer[A_RGB_KEY_INDEX] = rgb;
      runtime_led_buffer[C_RGB_KEY_INDEX] = rgb;
    }
  }
}

void update_layer_leds(void) {
  // Updates rgb cache for current layer
  uint8_t active_layer = biton32(layer_state);
  set_led_buffer_for_layer(active_layer);
}

bool led_update_user(led_t led_state) {
  // Called when the caps/num/scroll lock state changes
  bool changed_state = capslock_active != led_state.caps_lock;
  capslock_active = led_state.caps_lock; 

  if (changed_state) {
    update_layer_leds();
  }

  return true;
}

bool rgb_matrix_indicators_user(void) {
  // Called each RGB frame to set custom per-layer or per-key LED colors

  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
      RGB c = runtime_led_buffer[i];
      rgb_matrix_set_color(i, c.r, c.g, c.b);
  }

  return true;
}

/* ######### INITIALIZATION HOOK ######### */

void keyboard_post_init_user(void) {
  // Called after keyboard finishes intitialization
  rgb_matrix_enable(); // enable rgb matrix in qmk
  update_layer_leds();
}

/* ######### LAYER CHANGE ######### */

layer_state_t layer_state_set_user(layer_state_t state) {
    // Call your LED buffer update whenever the layer changes
    update_layer_leds();
    return state;
}

/* ######### OS-SPECIFIC KEYCODES ######### */

bool process_keycode_win(uint16_t keycode) {
  // Handles custom keycodes and shortcuts for Windows OS.
  // Kept separate from macOS for clarity and modularity.
  switch (keycode) {
    case RGB_SLD:
      rgblight_mode(1); 
      break;
    case U_SE_LESS: 
      tap_code16(SE_LESS_WIN);
      break;
    case U_SE_GRTR: 
      tap_code16(SE_GRTR_WIN);
      break;
    case U_SE_PIPE: 
      tap_code16(SE_PIPE_WIN);
      break;
    case U_SE_LCBR:
      tap_code16(SE_LCBR_WIN);
      break;
    case U_SE_RCBR: 
      tap_code16(SE_RCBR_WIN);
      break;
    case U_SE_BSLS: 
      tap_code16(SE_BSLS_WIN);
      break;  
    case U_FIND_PREV:
      tap_code16(S(KC_F3)); 
      break;
    case U_FIND_NEXT:
      tap_code16(C(KC_G)); 
      break;
    case U_SEARCH:
      tap_code16(C(KC_F)); 
      break;
    case U_REPLACE:
      tap_code16(C(KC_H)); 
      break;
    case U_REPLACE_ALL:
      tap_code16(C(S(KC_H))); 
      break;
    case U_UNDO:
      tap_code16(C(KC_Z)); 
      break;
    case U_REDO:
      tap_code16(C(KC_Y)); 
      break;
    case U_COPY:
      tap_code16(C(KC_C)); 
      break;
    case U_PASTE:
      tap_code16(C(KC_V)); 
      break;
    case U_SAVE:
      tap_code16(C(KC_S)); 
      break;
    case U_MARK_ALL:
      tap_code16(C(KC_A)); 
      break;
    case U_MARK_LINE:
      tap_code(KC_HOME);
      register_code(KC_LSFT);
      tap_code(KC_END);
      unregister_code(KC_LSFT);
      break;
    case U_MARK_WORD:
      tap_code16(C(KC_LEFT));
      register_code(KC_LCTL);
      register_code(KC_LSFT);
      tap_code(KC_RIGHT);
      unregister_code(KC_LSFT);
      unregister_code(KC_LCTL);
      break;
    case U_DOC_LEFT:
      tap_code(KC_HOME); 
      break;
    case U_DOC_DOWN:
      tap_code16(C(KC_END)); 
      break;
    case U_DOC_UP:
      tap_code16(C(KC_HOME)); 
      break;
    case U_DOC_RIGHT:
      tap_code(KC_END); 
      break;
    case U_WORD_LEFT:
      tap_code16(C(KC_LEFT)); 
      break;
    case U_PARA_DOWN:
      tap_code16(C(KC_DOWN)); 
      break;
    case U_PARA_UP:
      tap_code16(C(KC_UP)); 
      break;
    case U_WORD_RIGHT:
      tap_code16(C(KC_RIGHT));
      tap_code(KC_LEFT);
      break;
    case U_SCREENSHOT:
      tap_code16(G(KC_S)); 
      break;
    case U_OS_SEARCH:
      tap_code16(G(S(KC_S))); 
      break;
    case U_LOCK_SCREEN:
      tap_code16(G(KC_L)); 
      break;
    case U_EMOJIS:
      tap_code16(G(KC_DOT)); 
      break;
    case U_THUMBS_UP_EMOJI:
      SEND_STRING("👍");
      break;          
    case U_TOGGLE_OS:
      current_os = OS_MAC;
      return false;
    case U_SHOW_APPS:
      tap_code16(G(KC_TAB));
      break;    
    case U_SHOW_DESKTOP:
      tap_code16(G(KC_D));
      break;        
    case U_PREV_APP:
      tap_code16(S(A(KC_TAB)));
      break; 
    case U_NEXT_APP:
      tap_code16(A(KC_TAB));
      break; 
    case U_PREV_APP_WINDOW:
      tap_code16(S(C(KC_TAB)));
      break; 
    case U_NEXT_APP_WINDOW:
      tap_code16(C(KC_TAB));
      break;  
    case U_PREV_TAB:
      tap_code16(S(C(KC_TAB)));
      break; 
    case U_NEXT_TAB:
      tap_code16(C(KC_TAB)); 
      break;  
    case U_NEW_TAB:
      tap_code16(C(KC_T));
      break; 
    case U_CLOSE_TAB:
      tap_code16(C(KC_W));
      break;           
  }

  return true; // Let QMK handle other keycodes
}

bool process_keycode_mac(uint16_t keycode) {
  // Handles custom keycodes and shortcuts for Mac.
  // Kept separate from Windows OS for clarity and modularity.
  switch (keycode) {
    case RGB_SLD:
      rgblight_mode(1); 
      break;
    case U_SE_LESS: 
      tap_code16(SE_LESS_MAC);
      break;
    case U_SE_GRTR: 
      tap_code16(SE_GRTR_MAC);
      break;
    case U_SE_PIPE: 
      tap_code16(SE_PIPE_MAC);
      break;
    case U_SE_LCBR:
      tap_code16(SE_LCBR_MAC);
      break;
    case U_SE_RCBR: 
      tap_code16(SE_RCBR_MAC);
      break;
    case U_SE_BSLS: 
      tap_code16(SE_BSLS_MAC);
      break;
    case U_FIND_PREV:
    tap_code16(G(S(KC_G)));
      break;
    case U_FIND_NEXT:
      tap_code16(G(KC_G));
      break;
    case U_SEARCH:
      tap_code16(G(KC_F));
      break;
    case U_REPLACE:
      tap_code16(A(G(KC_F)));
      break;
    case U_REPLACE_ALL:
      tap_code16(A(G(S(KC_F))));
      break;
    case U_UNDO:
      tap_code16(G(KC_Z));
      break;
    case U_REDO:
      tap_code16(G(S(KC_Z)));
      break;
    case U_COPY:
      tap_code16(G(KC_C));
      break;
    case U_PASTE:
      tap_code16(G(KC_V));
      break;
    case U_SAVE: 
      tap_code16(G(KC_S));
      break;
    case U_MARK_ALL:
      tap_code16(G(KC_A));
      break;
    case U_MARK_LINE:
      tap_code16(G(KC_LEFT));
      register_code(KC_LGUI);
      register_code(KC_LSFT);
      tap_code(KC_RIGHT);
      unregister_code(KC_LSFT);
      unregister_code(KC_LGUI);
      break;
    case U_MARK_WORD:
      tap_code16(A(KC_LEFT));
      register_code(KC_LALT);
      register_code(KC_LSFT);
      tap_code(KC_RIGHT);
      unregister_code(KC_LSFT);
      unregister_code(KC_LALT);
      break;
    case U_DOC_LEFT:
      tap_code16(G(KC_LEFT)); 
      break;
    case U_DOC_DOWN:
      tap_code16(G(KC_DOWN)); 
      break;
    case U_DOC_UP:
      tap_code16(G(KC_UP)); 
      break;
    case U_DOC_RIGHT:
      tap_code16(G(KC_RIGHT)); 
      break;
    case U_WORD_LEFT:
      tap_code16(A(KC_LEFT)); 
      break;
    case U_PARA_DOWN:
      tap_code16(A(KC_DOWN)); 
      break;
    case U_PARA_UP:
      tap_code16(A(KC_UP)); 
      break;
    case U_WORD_RIGHT:
      tap_code16(A(KC_RIGHT));
      tap_code(KC_LEFT);
      break;
    case U_SCREENSHOT:
      tap_code16(G(S(KC_4))); 
      break;
    case U_OS_SEARCH:
      tap_code16(G(KC_SPACE)); 
      break;
    case U_LOCK_SCREEN:
      tap_code16(C(G(KC_Q))); 
      break;
    case U_EMOJIS:
      tap_code16(C(G(KC_SPACE))); 
      break;
    case U_THUMBS_UP_EMOJI:
      SEND_STRING("👍");
      break;      
    case U_TOGGLE_OS:
      current_os = OS_WINDOWS; 
      return false;  
    case U_SHOW_APPS:
      tap_code16(C(KC_UP));
      break;
    case U_SHOW_DESKTOP:
      tap_code16(KC_F11);
      break;         
    case U_PREV_APP:
      tap_code16(S(G(KC_TAB)));
      break; 
    case U_NEXT_APP:
      tap_code16(G(KC_TAB));  
      break;     
    case U_PREV_APP_WINDOW:
      tap_code16(S(G(KC_GRV)));
      break; 
    case U_NEXT_APP_WINDOW:
      tap_code16(G(KC_GRV));
      break;       
    case U_PREV_TAB:
      tap_code16(G(A(KC_LEFT)));
      break; 
    case U_NEXT_TAB:
      tap_code16(G(A(KC_RIGHT)));
      break;
    case U_NEW_TAB:
      tap_code16(G(KC_T));
      break; 
    case U_CLOSE_TAB:
      tap_code16(G(KC_W));
      break;           
  }

  return true; // Let QMK handle other keycodes
}

/* ######### MAIN KEY PROCESSING ######### */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  if (!record->event.pressed) {
    return true;
  }

  switch (current_os) {
    case OS_WINDOWS:
      return process_keycode_win(keycode);
    case OS_MAC:
      return process_keycode_mac(keycode);
    default:
      return true; // fallback
  }
}