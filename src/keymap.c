/* ######### INCLUDES ######### */ 

#include "qmk_keyboard.h"
#include "raw_hid.h"
#include "version.h"
#include "i18n.h"

/* ######### DEFINES ######### */

#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

// TODO - Will change
// Used to highlight specific keys
#define LEFT_OPTION_KEY_INDEX 10
#define RIGHT_OPTION_KEY_INDEX 11
#define LEFT_CTRL_KEY_INDEX 12
#define RIGHT_CTRL_KEY_INDEX 13
#define CAPS_LOCK_KEY_INDEX 18

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  SYM,
  NAV,  
  MOUSE,
  NUM,
  SYS  
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
  U_TOGGLE_OS,
  U_LOCK_SCREEN,
  U_APP_VIEW,
  U_PREV_APP,
  U_NEXT_APP,
  U_PREV_APP_WINDOW,
  U_NEXT_APP_WINDOW,
  U_APP_SWITCHER,
  U_PREV_TAB,
  U_NEXT_TAB,  
  U_NEW_TAB    
};

typedef enum {
  OS_WINDOWS,
  OS_MAC,
} os_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os; // Used for storing info about the os
bool capslock_active = false; // Used for setting color for caps key lede
extern rgb_config_t rgb_matrix_config; // Global variable provided by QMK that stores the current RGB matrix settings

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [ALPHA] = LAYOUT_voyager(
    KC_NO,          KC_NO,              KC_NO,              KC_NO,              KC_NO,              KC_NO,                         KC_NO,           KC_NO,              KC_NO,               KC_NO,                 KC_NO,                 KC_NO,          
    OSM(MOD_LSFT),  KC_Q,               KC_W,               KC_E,               KC_R,               KC_T,                          KC_Y,            KC_U,               KC_I,                KC_O,                  KC_P,                  KC_ESCAPE,      
    CW_TOGG,        MT(MOD_LALT, KC_A), LT(4, KC_S),        MT(MOD_LCTL, KC_D), MT(MOD_LSFT, KC_F), KC_G,                          KC_H,            MT(MOD_RSFT, KC_J), MT(MOD_RCTL, KC_K),  LT(4, KC_L),           MT(MOD_RALT, SE_OSLH), SE_ADIA,        
    KC_CAPS,        LT(5, KC_Z),        MT(MOD_LGUI, KC_X), KC_C,               KC_V,               KC_B,                          KC_N,            KC_M,               KC_COMMA,            MT(MOD_RGUI,  KC_DOT), LT(5, SE_AA),          KC_DELETE,      
                                                                                LT(1, KC_TAB),      LT(2, KC_ENTER),               LT(2, KC_SPACE), LT(1, KC_BSPC)
  ),
  [SYM] = LAYOUT_voyager(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,           KC_NO,         KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          SE_QUES,        SE_LESS,        SE_GRTR,        SE_MINS,        SE_PIPE,                                       SE_CIRC,        SE_LCBR,        SE_RCBR,        SE_DLR,         SE_ACUT,        SE_GRV,         
    KC_PERC,        KC_EXLM,        SE_ASTR,        SE_SLSH,        SE_EQL,         SE_AMPR,                                       KC_HASH,        SE_LPRN,        SE_RPRN,        SE_SCLN,        SE_DQUO,        SE_BSLS,        
    KC_NO,          SE_TILD,        SE_PLUS,        SE_LBRC,        SE_RBRC,        SE_AT,                                         SE_UNDS,        SE_COLN,        KC_COMMA,       KC_DOT,         SE_APOS,        KC_NO,          
                                                                    KC_NO,          KC_NO,                                         KC_NO,          KC_NO
  ),
  [NAV] = LAYOUT_voyager(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,          KC_NO,         KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          U_FIND_PREV,    U_FIND_NEXT,    U_SEARCH,       U_REPLACE,      U_REPLACE_ALL,                                 KC_NO,          U_DOC_LEFT,     U_DOC_DOWN,     U_DOC_UP,       U_DOC_RIGHT,    KC_NO,          
    KC_NO,          U_UNDO,         U_REDO,         U_COPY,         U_PASTE,        U_SAVE,                                        KC_NO,          KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_NO,          
    KC_NO,          U_MARK_ALL,     U_MARK_LINE,    U_MARK_WORD,    MOD_LSFT,       KC_NO,                                         KC_NO,          U_WORD_LEFT,    U_PARA_DOWN,    U_PARA_UP,      U_WORD_RIGHT,   KC_NO,          
                                                                    MO(3),          KC_NO,                                         KC_NO,          MO(3)
  ),
  [MOUSE] = LAYOUT_voyager(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,          KC_MS_WH_LEFT,  KC_MS_UP,       KC_MS_WH_RIGHT, KC_NO,          KC_NO,          
    KC_NO,          QK_LLCK,        KC_MS_BTN2,     KC_MS_BTN3,     KC_MS_BTN1,     KC_NO,                                         KC_NO,          KC_MS_LEFT,     KC_MS_DOWN,     KC_MS_RIGHT,    KC_NO,          KC_NO,          
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,          KC_MS_WH_DOWN,  KC_NO,          KC_MS_WH_UP,    KC_NO,          KC_NO,          
                                                                    KC_NO,          KC_NO,                                         KC_NO,          KC_NO
  ),  
  [NUM] = LAYOUT_voyager(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                         KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          
    KC_F5,          KC_F4,          KC_F3,          KC_F2,          KC_F1,          KC_NO,                                         KC_NO,          KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           
    KC_F10,         KC_F9,          KC_F8,          KC_F7,          KC_F6,          KC_NO,                                         KC_NO,          KC_6,           KC_7,           KC_8,           KC_9,           KC_NO,          
                                                                    KC_F11,         KC_F12                                         KC_NO,          KC_0
  ),
  [SYS] = LAYOUT_voyager(
    KC_NO,          KC_NO,               KC_NO,               KC_NO,               KC_NO,               KC_NO,                     KC_NO,          KC_NO,          KC_NO,      KC_NO,             KC_NO,             KC_NO,
    KC_NO,          KC_AUDIO_VOL_DOWN,   KC_AUDIO_VOL_UP,     KC_NO,               KC_AUDIO_MUTE,       KC_NO,                     KC_NO,          U_OS_SEARCH,    U_TERMINAL, U_BROWSER,         U_EMOJIS,          U_LOCK_SCREEN,          
    KC_NO,          RGB_VAD,             RGB_VAI,             KC_NO,               RGB_TOG,             KC_NO,                     U_APP_VIEW,     U_PREV_APP,     U_NEXT_APP, U_PREV_APP_WINDOW, U_NEXT_APP_WINDOW, KC_NO,          
    KC_NO,          KC_MEDIA_PREV_TRACK, KC_MEDIA_NEXT_TRACK, KC_MEDIA_STOP,       KC_MEDIA_PLAY_PAUSE, KC_NO,                     KC_NO,          U_PREV_TAB,     U_NEXT_TAB, U_NEW_TAB,         KC_NO,             U_TOGGLE_OS,          
                                                                                   KC_NO,               KC_NO,                     U_SCREENSHOT,   U_THUMBS_UP_EMOJI
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

const HSV HSV_WIN_COLOR = {170, 255, 255};
const HSV HSV_MAC_COLOR = {43, 255, 255};
const HSV HSV_CAPSLOCK_COLOR = {0, 255, 255};

const HSV PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171},
    {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171},
    {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171},
    {160, 200, 171}, {160, 200, 171},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171},
    {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171},
    {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171}, {160, 200, 171},
    {160, 200, 171}, {160, 200, 171}
  },

  [SYM] = {
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {270,150,167}, {95,100,153}, {95,100,153}, {100,110,160}, {270,150,167},
    {270,150,167}, {95,100,153}, {100,110,160}, {100,110,160}, {95,100,153}, {270,150,167},
    {0,0,0}, {270,150,167}, {100,110,160}, {180,140,223}, {180,140,223}, {270,150,167},
    {0,0,0}, {0,0,0},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {100,110,160}, {180,140,223}, {180,140,223}, {270,150,167}, {270,150,167}, {270,150,167},
    {270,150,167}, {180,140,223}, {180,140,223}, {270,150,167}, {270,150,167}, {270,150,167},
    {270,150,167}, {270,150,167}, {270,150,167}, {270,150,167}, {270,150,167}, {0,0,0},
    {0,0,0}, {0,0,0}
  },

  [NAV] = {
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {180,250,241}, {180,250,241}, {180,250,241}, {180,250,241}, {180,250,241},
    {0,0,0}, {120,200,216}, {120,200,216}, {120,200,216}, {120,200,216}, {120,200,216},
    {0,0,0}, {150,220,220}, {150,220,220}, {150,220,220}, {150,220,220}, {150,220,220},
    {160,200,171}, {0,0,0},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {170,230,216}, {170,230,216}, {170,230,216}, {170,230,216}, {0,0,0},
    {0,0,0}, {100,220,140}, {100,220,140}, {100,220,140}, {100,220,140}, {0,0,0},
    {0,0,0}, {180,250,229}, {180,250,229}, {180,250,229}, {180,250,229}, {0,0,0},
    {0,0,0}, {160,200,171}
  },

  [MOUSE] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {150,230,224}, {140,230,207}, {140,230,207}, {140,230,207}, {0,0,0},
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {0,0,0},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {100,220,162}, {100,220,162}, {100,220,162}, {100,220,162}, {0,0,0},
    {0,0,0}, {140,230,207}, {140,230,207}, {140,230,207}, {140,230,207}, {0,0,0},
    {0,0,0}, {0,0,0}
  },

  [NUM] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {35, 220, 253}, {35, 220, 253}, {35, 220, 253}, {35, 220, 253}, {35, 220, 253}, {0,0,0},
    {35, 220, 253}, {35, 220, 253}, {35, 220, 253}, {35, 220, 253}, {35, 220, 253}, {0,0,0},
    {35, 220, 253}, {35, 220, 253},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {150,220,209}, {150,220,209}, {150,220,209}, {150,220,209}, {150,220,209},
    {0,0,0}, {150,220,209}, {150,220,209}, {150,220,209}, {150,220,209}, {0,0,0},
    {0,0,0}, {150,220,209}
  }, 

  [SYS] = {
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {0,0,0}, {100,230,158}, {100,230,158}, {0,0,0}, {100,230,158}, {0,0,0},
    {0,0,0}, {180,230,216}, {180,230,216}, {180,230,216}, {180,230,216}, {0,0,0},
    {0,0,0}, {170,255,255}, {170,255,255}, {170,255,255}, {170,255,255}, {0,0,0},
    {0,0,0}, {0,0,0},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {270,112,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {50,225,167},
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {270,112,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {50,225,167},
    {270,112,158}, {270,112,158}
  }

};

/* ######### INITIALIZATION HOOK ######### */

void keyboard_post_init_user(void) {
  // Called after keyboard finishes intitialization

  rgb_matrix_enable(); // enable rgb matrix in qmk

  // Example: set OS based on EEPROM value
  uint8_t saved_os = ee_read_byte(EECONFIG_OS_MODE); // Example: set OS based on EEPROM value
  
  // If EEPROM is uninitialized, set default to Windows (0), it is 255 if not initialized
  if (saved_os > 1) {
      saved_os = 0;
      ee_write_byte(EECONFIG_OS_MODE, saved_os);
  }

  current_os = saved_os == 0
    ? OS_WINDOWS
    : OS_MAC;
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

void set_layer_color_from_ledmap(uint8_t layer) {
  // Set the layer color from ledmap
  for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) { 
    
    HSV hsv;
    hsv.h = pgm_read_byte(&ledmap[layer][i].h);
    hsv.s = pgm_read_byte(&ledmap[layer][i].s);
    hsv.v = pgm_read_byte(&ledmap[layer][i].v);
    
    if (!hsv.h && !hsv.s && !hsv.v) {
      // Endarken the layer
      rgb_matrix_set_color( i, 0, 0, 0 );
    } else {
      // Value from ledmap
      RGB rgb = hsv_to_rgb_with_value(hsv);
      rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
  }
}

bool rgb_matrix_indicators_user(void) {
  // Called each RGB frame to set custom per-layer or per-key LED colors
  
  // Do we have a valid rgb control object? If, not we do nothing.
  if (rawhid_state.rgb_control.mode != RGB_MATRIX_NONE) {
    return false;
  }

  uint8_t layer = biton32(layer_state);
  
  // Set the layer color based on type of layer
  if (layer <= MOUSE && !keyboard_config.disable_layer_led) {
    // Set the base layer color from
    set_layer_color_from_ledmap(layer);

    if (layer == ALPHA) {
      
      if (capslock_active) {
        // Set specific red color for capslock key, only on alpha
        RGB rgb = hsv_to_rgb_with_value(HSV_CAPSLOCK_COLOR);
        rgb_matrix_set_color(CAPS_LOCK_KEY_INDEX, rgb.r, rgb.g, rgb.b);
      }

      if (current_os == OS_MAC) {
        // Set Option keys yellow for Mac on alpha
        RGB rgb = hsv_to_rgb_with_value(HSV_MAC_COLOR);
        rgb_matrix_set_color(LEFT_OPTION_KEY_INDEX, rgb.r, rgb.g, rgb.b);
        rgb_matrix_set_color(RIGHT_OPTION_KEY_INDEX, rgb.r, rgb.g, rgb.b);
      } else {
        // Set Ctrl keys blue for Windows on alpha
        RGB rgb = hsv_to_rgb_with_value(HSV_WIN_COLOR);
        rgb_matrix_set_color(LEFT_CTRL_KEY_INDEX, rgb.r, rgb.g, rgb.b);
        rgb_matrix_set_color(RIGHT_CTRL_KEY_INDEX, rgb.r, rgb.g, rgb.b);
      }
    }
  } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
    // Endarken the layer
    rgb_matrix_set_color_all( 0, 0, 0 );
  }

  return true;
}

/* ######### OS-SPECIFIC KEYCODES ######### */

// Handles custom keycodes and shortcuts for Windows OS.
// Kept separate from macOS for clarity and modularity.
bool process_keycode_win(uint16_t keycode) {
  
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
      tap_code(KC_HOME);
      register_code(KC_LSFT);
      tap_code(KC_END);
      unregister_code(KC_LSFT);
      break;
    case U_MARK_WORD:      // Ctrl + Shift + Left/Right (Select word)
      tap_code16(C(KC_LEFT));
      register_code(KC_LCTL);
      register_code(KC_LSFT);
      tap_code(KC_RIGHT);
      unregister_code(KC_LSFT);
      unregister_code(KC_LCTL);
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
    case U_THUMBS_UP_EMOJI:
      SEND_STRING("👍");
      break;          
    case U_TOGGLE_OS:
      current_os = OS_MAC;
      ee_write_byte(EECONFIG_OS_MODE, 1);
      return false;
    case U_APP_VIEW:
      tap_code16(G(KC_TAB));
      break;   
    case U_PREV_APP:
      tap_code16(S(A(KC_TAB)));
    case U_NEXT_APP:
      tap_code16(A(KC_TAB));
    case: U_PREV_APP_WINDOW:
      tap_code16(S(C(KC_TAB)));
      break; 
    case: U_NEXT_APP_WINDOW,
      tap_code16(C(KC_TAB));
      break; 
    case U_APP_VIEW:
      tap_code16(G(KC_TAB));
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
  }

  return true; // Let QMK handle other keycodes
}

// Handles custom keycodes and shortcuts for Mac.
// Kept separate from Windows OS for clarity and modularity.
bool process_keycode_mac(uint16_t keycode) {

  switch (keycode) {
    case RGB_SLD:          // Set RGB to static mode
      rgblight_mode(1); 
      break;
    case U_FIND_PREV:      // Cmd + Shift + G (Find previous)
    tap_code16(G(S(KC_G)));
      break;
    case U_FIND_NEXT:      // Cmd + G (Find next)
      tap_code16(G(KC_G));
      break;
    case U_SEARCH:         // Cmd + F (Search)
      tap_code16(G(KC_F));
      break;
    case U_REPLACE:        // Cmd + Option + F (Replace)
      tap_code16(A(G(KC_F)));
      break;
    case U_REPLACE_ALL:    // Cmd + Option + Shift + F (Replace all)
      tap_code16(A(G(S(KC_F))));
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
      tap_code16(G(KC_LEFT));
      register_code(KC_LGUI);
      register_code(KC_LSFT);
      tap_code(KC_RIGHT);
      unregister_code(KC_LSFT);
      unregister_code(KC_LGUI);
      break;
    case U_MARK_WORD:      // Select the current word (Alt + Left, then Alt + Shift + Right)
      tap_code16(A(KC_LEFT));
      register_code(KC_LALT);
      register_code(KC_LSFT);
      tap_code(KC_RIGHT);
      unregister_code(KC_LSFT);
      unregister_code(KC_LALT);
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
    case U_THUMBS_UP_EMOJI:
      SEND_STRING("👍");
      break;      
    case U_TOGGLE_OS:
      current_os = OS_WINDOWS;
      ee_write_byte(EECONFIG_OS_MODE, 0);     
      return false;
    case U_APP_VIEW:
      tap_code16(C(KC_UP));
      break;   
    case U_PREV_APP:
      tap_code16(S(G(KC_TAB)));
      break; 
    case U_NEXT_APP:
      tap_code16(G(KC_TAB));  
      break;     
    case: U_PREV_APP_WINDOW:
      tap_code16(S(G(KC_GRV)));
      break; 
    case: U_NEXT_APP_WINDOW,
      tap_code16(G(KC_GRV));
      break;   
    case U_APP_VIEW:
      tap_code16(C(KC_UP));
      break;      
    case U_PREV_TAB:
      tap_code16(G(A(KC_LEFT)));
      break; 
    case U_NEXT_TAB:
      tap_code16(G(A(KC_RGHT)));
      break;
    case U_NEW_TAB:
      tap_code16(G(KC_T));
      break;
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