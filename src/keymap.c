/* ######### INCLUDES ######### */ 

#include QMK_KEYBOARD_H
#include "version.h"
#include "i18n.h"

/* ######### DEFINES ######### */

#define MOON_LED_LEVEL LED_LEVEL
#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  SYMBOL,
  NAVIGATION,  
  SYSTEM,
  NUMBER,
  MOUSE
};

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE, 
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
  U_MARK_SMART,
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
  U_LOCK_SCREEN,
  U_EMOJIS,
  U_TOGGLE_OS,
};

typedef enum {
  OS_WINDOWS,
  OS_MAC,
} os_t;

typedef enum {
  ENC_NONE,
  ENC_PAREN,        // ()
  ENC_BRACKET,      // []
  ENC_CURLY,        // {}
  ENC_ANGLE,        // <>
  ENC_DOUBLE_QUOTE, // ""
  ENC_SINGLE_QUOTE  // ''
} enclosure_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os; // Used for storing info about the os
bool capslock_active = false; // Used for setting color for caps key led
bool waiting_for_enclosure = false; // Used to mark text within encolusre: (), {}, [], <>, "". ''
extern rgb_config_t rgb_matrix_config; // Global variable provided by QMK that stores the current RGB matrix settings

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [ALPHA] = LAYOUT_voyager(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          
    OSM(MOD_LSFT),  KC_Q,           LT(4, KC_W),    LT(3, KC_E),    KC_R,           KC_T,                                           KC_Y,           KC_U,           LT(3, KC_I),    LT(4, KC_O),    KC_P,           KC_ESCAPE,      
    CW_TOGG,        MT(MOD_LALT, KC_A),MT(MOD_LGUI, KC_S),MT(MOD_LCTL, KC_D),MT(MOD_LSFT, KC_F),KC_G,                               KC_H,           MT(MOD_RSFT, KC_J),MT(MOD_RCTL, KC_K),MT(MOD_RGUI, KC_L),MT(MOD_RALT, SE_OSLH),SE_ADIA,        
    KC_CAPS,        KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,                                           KC_N,           KC_M,           KC_COMMA,       KC_DOT,         SE_AA,          KC_DELETE,      
                                                                    LT(1, KC_TAB),  LT(2, KC_ENTER),                                LT(2, KC_SPACE), LT(1, KC_BSPC)
  ),
  [SYMBOL] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_NO,          SE_QUES,        SE_LESS,        SE_GRTR,        SE_MINS,        SE_PIPE,                                        SE_CIRC,        SE_LCBR,        SE_RCBR,        SE_DLR,         SE_ACUT,        SE_GRV,         
    KC_PERC,        KC_EXLM,        SE_ASTR,        SE_SLSH,        SE_EQL,         SE_AMPR,                                        KC_HASH,        SE_LPRN,        SE_RPRN,        SE_SCLN,        SE_DQUO,        SE_BSLS,        
    KC_NO,          SE_TILD,        SE_PLUS,        SE_LBRC,        SE_RBRC,        SE_AT,                                          SE_UNDS,        SE_COLN,        KC_COMMA,       KC_DOT,         SE_APOS,        KC_NO,          
                                                                    KC_TRANSPARENT, KC_NO,                                          KC_NO, KC_TRANSPARENT
  ),
  [NAVIGATION] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_NO,          U_FIND_PREV,    U_FIND_NEXT,     U_SEARCH,     U_REPLACE,     U_REPLACE_ALL,                                    KC_NO,          U_DOC_LEFT,     U_DOC_DOWN,     U_DOC_UP,       U_DOC_RIGHT,    KC_NO,          
    KC_NO,          U_UNDO,         U_REDO,          U_COPY,       U_PASTE,       U_SAVE,                                           KC_NO,          KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_NO,          
    KC_NO,          U_MARK_ALL,     U_MARK_LINE,     U_MARK_WORD,  U_MARK,        U_MARK_SMART,                                     KC_NO,          U_WORD_LEFT,    U_PARA_DOWN,    U_PARA_UP,      U_WORD_RIGHT,   KC_NO,          
                                                                   MO(5),         KC_TRANSPARENT,                                   KC_TRANSPARENT, MO(5)
  ),
  [SYSTEM] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_NO,          KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_TRANSPARENT,KC_AUDIO_MUTE,KC_NO,                                           KC_NO,          KC_NO,          KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          
    KC_NO,          RGB_VAD,        RGB_VAI,        RGB_TOG,        TOGGLE_LAYER_COLOR,KC_NO,                                       KC_NO,          U_OS_SEARCH,    U_SCREENSHOT,   U_LOCK_SCREEN,  U_TOGGLE_OS,    KC_NO,          
    KC_NO,          KC_MEDIA_PREV_TRACK,KC_MEDIA_NEXT_TRACK,KC_MEDIA_PLAY_PAUSE,KC_MEDIA_STOP,  KC_NO,                              KC_NO,          U_EMOJIS,       KC_NO,          KC_NO,          KC_NO,          KC_NO,          
                                                                   KC_NO,         KC_NO,                                            KC_NO,          KC_NO
  ),
  [NUMBER] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_NO,          KC_NO,          KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT, KC_NO,          KC_NO,          
    KC_F5,          KC_F4,          KC_F3,          KC_F2,          KC_F1,          KC_NO,                                          KC_NO,          KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           
    KC_F10,         KC_F9,          KC_F8,          KC_F7,          KC_F6,          KC_NO,                                          KC_NO,          KC_6,           KC_7,           KC_8,           KC_9,           KC_NO,          
                                                                    KC_F11,         KC_F12,                                         KC_NO,          KC_0
  ),
  [MOUSE] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          
    KC_NO,          QK_LLCK,        KC_MS_BTN2,     KC_MS_BTN3,     KC_MS_BTN1,     KC_NO,                                          KC_NO,          KC_MS_LEFT,     KC_MS_DOWN,     KC_MS_UP,       KC_MS_RIGHT,    KC_NO,          
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_MS_WH_LEFT,  KC_MS_WH_DOWN,  KC_MS_WH_UP,    KC_MS_WH_RIGHT, KC_NO,          
                                                                    KC_TRANSPARENT, KC_NO,                                          KC_NO,          KC_TRANSPARENT
  ),
};

/* ######### LED MAPS ######### */

const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {

  [ALPHA] = { 
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171},
    {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171},
    {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171},
    {67, 240, 171}, {67, 240, 171},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171},
    {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171},
    {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171}, {67, 240, 171},
    {67, 240, 171}, {67, 240, 171}
  },

  [SYMBOL] = { 
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

  [NAVIGATION] = { 
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {46,248,241}, {46,248,241}, {46,248,241}, {46,248,241}, {46,248,241}, 
    {0,0,0}, {220,238,216}, {220,238,216}, {220,238,216}, {220,238,216}, {220,238,216},
    {0,0,0}, {139,241,220}, {139,241,220}, {139,241,220}, {139,241,220}, {139,241,220}, 
    {0,240,171}, {0,0,0}, 
    // Right side       
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {169,243,216}, {169,243,216}, {169,243,216}, {169,243,216}, {0,0,0}, 
    {0,0,0}, {83,234,140}, {83,234,140}, {83,234,140}, {83,234,140}, {0,0,0},        
    {0,0,0}, {18,250,229}, {18,250,229},  {18,250,229}, {18,250,229}, {0,0,0}, 
    {0,0,0}, {0,240,171} 
  },

  [SYSTEM] = { 
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {101,238,158}, {101,238,158}, {0,0,0}, {101,238,158}, {0,0,0}, 
    {0,0,0}, {40,239,216}, {40,239,216},  {40,239,216}, {40,239,216}, {0,0,0}, 
    {0,0,0}, {170,255,255}, {170,255,255}, {170,255,255}, {170,255,255}, {0,0,0},
    {0,0,0}, {0,0,0}, 
    // Right side     
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {120,112,158}, {120,112,158}, {234,225,167}, {234,225,167}, {0,0,0}, 
    {0,0,0}, {120,112,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0} 
  },

  [NUMBER] = { 
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
};

/* ######### INITIALIZATION HOOK ######### */

void keyboard_post_init_user(void) {
  // Called after keyboard finishes intitialization

  rgb_matrix_enable(); // enable rgb matrix in qmk

  // Example: set OS based on EEPROM value
  uint8_t saved_os = ee_read_byte(EECONFIG_OS_MODE); // Example: set OS based on EEPROM value
  
  if (saved_os == 1) {
      current_os = OS_MAC;
  } else {
      current_os = OS_WINDOWS;
  }
}

/* ######### LED CONTROL FUNCTIONS ######### */

bool led_update_user(led_t led_state) {
  // Called when the caps/num/scroll lock state changes
  capslock_active = led_state.caps_lock; // update caps lock active -> use it later to indicate turn on with rgb
  return true;
} 

RGB hsv_to_rgb_with_value(HSV hsv) {
// Convert HSV to RGB while taking the current RGB brightness into account
  RGB rgb = hsv_to_rgb( hsv );
  float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
  return (RGB){ f * rgb.r, f * rgb.g, f * rgb.b };
}

void set_layer_color(int layer) {
  for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) { 
    HSV hsv = {
      .h = pgm_read_byte(&ledmap[layer][i][0]),
      .s = pgm_read_byte(&ledmap[layer][i][1]),
      .v = pgm_read_byte(&ledmap[layer][i][2]),
    };
    
    if (!hsv.h && !hsv.s && !hsv.v) {
        rgb_matrix_set_color( i, 0, 0, 0 );
    } else {
        RGB rgb = hsv_to_rgb_with_value(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
  }
}

bool rgb_matrix_indicators_user(void) {
  // Called each RGB frame to set custom per-layer or per-key LED colors
  
  if (rawhid_state.rgb_control) {
    return false;
  }

  uint8_t layer = biton32(layer_state);

  if (!keyboard_config.disable_layer_led && layer <= MOUSE) {
    
    // Set color for layer
    set_layer_color(layer);

    // TODO
    // Write logic for win/mac visualzation

    // Set spedific color for capslock key
    if (capslock_active && layer == ALPHA) {
      RGB rgb = hsv_to_rgb_with_value((HSV){255, 0, 0});
      rgb_matrix_set_color(18, rgb.r, rgb.g, rgb.b);
    }

  } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
    rgb_matrix_set_color_all(0, 0, 0);
  }

  return true;
}

/* ######### KEY MAPPING HELPERS ######### */

enclosure_t map_key_to_enclosure(uint16_t keycode) {
  switch (keycode) {
    case KC_LPRN: 
    case SE_LPRN: 
      return ENC_PAREN;
    case KC_LBRC: 
    case SE_LBRC: 
      return ENC_BRACKET;
    case KC_LCBR: 
    case SE_LCBR: 
      return ENC_CURLY;
    case KC_LABK:             
      return ENC_ANGLE;
    case KC_DQUO:             
      return ENC_DOUBLE_QUOTE;
    case KC_QUOTE:            
      return ENC_SINGLE_QUOTE;
    default:                 
      return ENC_NONE;
  }
}

/* ######### OS-SPECIFIC KEYCODES ######### */

bool process_keycode_win(uint16_t keycode) {
  
  if (waiting_for_enclosure) {

    waiting_for_enclosure = false; 
    enclosure_t enc = map_key_to_enclosure(keycode);

    switch (enc) {
      case ENC_PAREN: // Cmd+Shift+M → select inside ()        
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_M)))); 
        break;
      case ENC_BRACKET: // Cmd+Shift+B → select inside []      
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_B)))); 
        break;
      case ENC_CURLY: // Cmd+Shift+C → select inside {}       
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_C)))); 
        break;
      case ENC_ANGLE: // Cmd+Shift+A → select inside <>        
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_A)))); 
        break;
      case ENC_DOUBLE_QUOTE: // Cmd+Shift+Q → select inside "" 
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_Q)))); 
        break;
      case ENC_SINGLE_QUOTE: // Cmd+Shift+S → select inside ''
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_S)))); 
        break;
      default: 
        break;
    }
  
    return false;
  }
  
  switch (keycode) {
    case RGB_SLD:          // Set RGB to static mode
      rgblight_mode(1); 
      break;
    case U_FIND_PREV:      // Shift + F3 (Find previous)
      tap_code16(S(KC_F3)); 
      break;
    case U_FIND_NEXT:      // Ctrl + G (Find next)
      tap_code16(C(KC_G)); 
      break;
    case U_SEARCH:         // Ctrl + F (Search)
      tap_code16(C(KC_F)); 
      break;
    case U_REPLACE:        // Ctrl + H (Replace)
      tap_code16(C(KC_H)); 
      break;
    case U_REPLACE_ALL:    // Ctrl + Shift + H (Replace all)
      tap_code16(C(S(KC_H))); 
      break;
    case U_UNDO:           // Ctrl + Z (Undo)
      tap_code16(C(KC_Z)); 
      break;
    case U_REDO:           // Ctrl + Y (Redo)
      tap_code16(C(KC_Y)); 
      break;
    case U_COPY:           // Ctrl + C (Copy)
      tap_code16(C(KC_C)); 
      break;
    case U_PASTE:          // Ctrl + V (Paste)
      tap_code16(C(KC_V)); 
      break;
    case U_SAVE:           // Ctrl + S (Save)
      tap_code16(C(KC_S)); 
      break;
    case U_MARK_ALL:       // Ctrl + A (Select all)
      tap_code16(C(KC_A)); 
      break;
    case U_MARK_LINE:      // Shift + Home -> Shift + End (Select line)
      SEND_STRING(SS_TAP(X_HOME) SS_DELAY(100) SS_LSFT(SS_TAP(X_END))); 
      break;
    case U_MARK_WORD:      // Ctrl + Shift + Left/Right (Select word)
      SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)) SS_DELAY(100) SS_LCTL(SS_LSFT(SS_TAP(X_RIGHT)))); 
      break;
    case U_MARK_SMART:     // Smart mark (custom logic)
      waiting_for_enclosure = true; 
      break;
    case U_DOC_LEFT:       // Home (Go to start of document)
      tap_code(KC_HOME); 
      break;
    case U_DOC_DOWN:       // Ctrl + End (Go to document end)
      tap_code16(C(KC_END)); 
      break;
    case U_DOC_UP:         // Ctrl + Home (Go to document start)
      tap_code16(C(KC_HOME)); 
      break;
    case U_DOC_RIGHT:      // End (Go to line end)
      tap_code(KC_END); 
      break;
    case U_WORD_LEFT:      // Ctrl + Left Arrow
      tap_code16(C(KC_LEFT)); 
      break;
    case U_PARA_DOWN:      // Ctrl + Down Arrow
      tap_code16(C(KC_DOWN)); 
      break;
    case U_PARA_UP:        // Ctrl + Up Arrow
      tap_code16(C(KC_UP)); 
      break;
    case U_WORD_RIGHT:     // Ctrl + Right Arrow
      tap_code16(C(KC_RIGHT)); 
      break;
    case U_SCREENSHOT:     // GUI + S (Screenshot)
      tap_code16(G(KC_S)); 
      break;
    case U_OS_SEARCH:      // GUI + Shift + S (OS search)
      tap_code16(G(S(KC_S))); 
      break;
    case U_LOCK_SCREEN:    // GUI + L (Lock screen)
      tap_code16(G(KC_L)); 
      break;
    case U_EMOJIS:         // GUI + . (Open emoji picker)
      tap_code16(G(KC_DOT)); 
      break;
    case U_TOGGLE_OS:
      current_os = OS_MAC;
      ee_write_byte(EECONFIG_OS_MODE, 1);
      return false;
  }

  return true; // Let QMK handle other keycodes
}

bool process_enclosure_keycode_mac(uint16_t keycode) {
  
}

bool process_keycode_mac(uint16_t keycode) {
  
  if (waiting_for_enclosure) {

    waiting_for_enclosure = false; 
    enclosure_t enc = map_key_to_enclosure(keycode);

    switch (enc) {
      case ENC_PAREN: // Cmd+Shift+M → select inside ()        
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_M)))); 
        break;
      case ENC_BRACKET: // Cmd+Shift+B → select inside []      
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_B)))); 
        break;
      case ENC_CURLY: // Cmd+Shift+C → select inside {}       
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_C)))); 
        break;
      case ENC_ANGLE: // Cmd+Shift+A → select inside <>        
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_A)))); 
        break;
      case ENC_DOUBLE_QUOTE: // Cmd+Shift+Q → select inside "" 
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_Q)))); 
        break;
      case ENC_SINGLE_QUOTE: // Cmd+Shift+S → select inside ''
        SEND_STRING(SS_LGUI(SS_LSFT(SS_TAP(X_S)))); 
        break;
      default: break;
    }

    return false;
  }

  switch (keycode) {
    case RGB_SLD:          // Set RGB to static mode
      rgblight_mode(1); 
      break;
    case U_FIND_PREV:      // Shift + F3 (Find previous)
      tap_code16(S(KC_F3));
      break;
    case U_FIND_NEXT:      // Cmd + G (Find next)
      tap_code16(G(KC_G));
      break;
    case U_SEARCH:         // Cmd + F (Search)
      tap_code16(G(KC_F));
      break;
    case U_REPLACE:        // Cmd + H (Replace)
      tap_code16(G(KC_H));
      break;
    case U_REPLACE_ALL:    // Cmd + Shift + H (Replace all)
      tap_code16(G(S(KC_H)));
      break;
    case U_UNDO:           // Cmd + Z (Undo)
      tap_code16(G(KC_Z));
      break;
    case U_REDO:           // Cmd + Shift + Z (Redo)
      tap_code16(G(S(KC_Z)));
      break;
    case U_COPY:           // Cmd + C (Copy)
      tap_code16(G(KC_C));
      break;
    case U_PASTE:          // Cmd + V (Paste)
      tap_code16(G(KC_V));
      break;
    case U_SAVE:           // Cmd + S (Save)
      tap_code16(G(KC_S));
      break;
    case U_MARK_ALL:       // Cmd + A (Select all)
      tap_code16(G(KC_A));
      break;
    case U_MARK_LINE:      // Select the current line (Cmd + Left, then Cmd + Shift + Right)
      SEND_STRING(SS_LGUI(SS_TAP(X_LEFT)) SS_DELAY(100) SS_LGUI(SS_LSFT(SS_TAP(X_RIGHT)))); 
      break;
    case U_MARK_WORD:      // Select the current word (Alt + Left, then Alt + Shift + Right)
      SEND_STRING(SS_LALT(SS_TAP(X_LEFT)) SS_DELAY(100) SS_LALT(SS_LSFT(SS_TAP(X_RIGHT)))); 
      break;
    case U_MARK_SMART:     // Enable smart marking logic (custom feature)
      waiting_for_enclosure = true; 
      break;
    case U_DOC_LEFT:       // Cmd + Left Arrow (Start of line)
      tap_code16(G(KC_LEFT)); 
      break;
    case U_DOC_DOWN:       // Cmd + Down Arrow (End of document)
      tap_code16(G(KC_DOWN)); 
      break;
    case U_DOC_UP:         // Cmd + Up Arrow (Start of document)
      tap_code16(G(KC_UP)); 
      break;
    case U_DOC_RIGHT:      // Cmd + Right Arrow (End of line)
      tap_code16(G(KC_RIGHT)); 
      break;
    case U_WORD_LEFT:      // Alt + Left Arrow (Move cursor one word left)
      tap_code16(A(KC_LEFT)); 
      break;
    case U_PARA_DOWN:      // Alt + Down Arrow (Move cursor one paragraph down)
      tap_code16(A(KC_DOWN)); 
      break;
    case U_PARA_UP:        // Alt + Up Arrow (Move cursor one paragraph up)
      tap_code16(A(KC_UP)); 
      break;
    case U_WORD_RIGHT:     // Alt + Right Arrow (Move cursor one word right)
      tap_code16(A(KC_RIGHT)); 
      break;
    case U_SCREENSHOT:     // Cmd + Shift + 4 (Screenshot selection)
      tap_code16(G(S(KC_4))); 
      break;
    case U_OS_SEARCH:      // Cmd + Space (Spotlight search)
      tap_code16(G(KC_SPACE)); 
      break;
    case U_LOCK_SCREEN:    // Ctrl + Cmd + Q (Lock screen)
      tap_code16(C(G(KC_Q))); 
      break;
    case U_EMOJIS:         // Ctrl + Cmd + Space (Emoji picker)
      tap_code16(C(G(KC_SPACE))); 
      break;
    case U_TOGGLE_OS:
      current_os = OS_WINDOWS;
      ee_write_byte(EECONFIG_OS_MODE, 0);     
      return false;
  }

  return true; // Let QMK handle other keycodes
}

/* ######### MOUSE HELPER ######### */

bool process_mouse_key_press(uint16_t keycode, keyrecord_t *record) {
  
  if (record->event.pressed) {
    add_mods(QK_MODS_GET_MODS(keycode));
    send_keyboard_report();
    wait_ms(1);
    register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
    return false;
  } 

  del_mods(QK_MODS_GET_MODS(keycode));
  return true;
}

/* ######### MAIN KEY PROCESSING ######### */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) { // This keycode is a "mod + key" combo (like Ctrl+C, Shift+Tab, etc.)
    return IS_MOUSE_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))
      ? process_mouse_key_press(keycode, record)
      : true;
  }

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