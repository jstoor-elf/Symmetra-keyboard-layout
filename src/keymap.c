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
  ALPHA0,
  ALPHA1,
  SYM,
  NUM,
  EXT,
  NAV,
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
  U_TOGGLE_OS,
  U_TOGGLE_ALPHA,
  U_LOCK_SCREEN
};

typedef enum {
  OS_WINDOWS,
  OS_MAC
} os_t;

typedef enum {
  ALPHA_QWERTY,
  ALPHA_KVIKK,
} alpha_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os = OS_WINDOWS; // Used for storing info about the os
alpha_t current_alpha = ALPHA_QWERTY; // Used for storing info about the alpha layer

uint16_t animation_os_timer = 0; // Used for visualizing os and layer switch
uint16_t animation_alpha_timer = 0; // Used for visualizing os and layer switch
bool capslock_active = false; // Used for setting color for caps key leda
extern rgb_config_t rgb_matrix_config; // Global variable provided by QMK that stores the current RGB matrix settings

static bool fast_cursor_up_active = false; // Used to know if fast cursor is held
static uint16_t fast_cursor_up_timer = 0; // Used to know the time between the last cursor movement
static uint16_t fast_cursor_up_last_repeat = 0; // Used for repeating scroll

static bool fast_cursor_down_active = false; // Used to know if fast cursor is held
static uint16_t fast_cursor_down_timer = 0; // Used to know the time between the last cursor movement
static uint16_t fast_cursor_down_last_repeat = 0; // Used for repeating scroll

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [ALPHA0] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,           XXXXXXX,         /*|*/XXXXXXX,            XXXXXXX,         XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,          
    XXXXXXX, KC_Q,    KC_W,    KC_E,    KC_R,              KC_T,            /*|*/KC_Y,               KC_U,            KC_I,     KC_O,    KC_P,    XXXXXXX,      
    XXXXXXX, KC_A,    KC_S,    KC_D,    KC_F,              KC_G,            /*|*/KC_H,               KC_J,            KC_K,     KC_L,    SE_OSLH, XXXXXXX,        
    XXXXXXX, KC_Z,    KC_X,    KC_C,    KC_V,              KC_B,            /*|*/KC_N,               KC_M,            SE_ADIA,  KC_DOT,  SE_AA,   XXXXXXX,      
                                        LT(SYM, KC_ENTER), LT(NAV, SE_ADIA),/*|*/LT(NAV, KC_ESCAPE), LT(NUM, KC_SPACE)
  ),
  [ALPHA1] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,           XXXXXXX,      /*|*/XXXXXXX,            XXXXXXX,               XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,          
    XXXXXXX, KC_X,    KC_C,    KC_O,    SE_ADIA,           KC_U,         /*|*/KC_H,               KC_K,                  KC_M,     KC_B,    KC_J,    XXXXXXX,      
    XXXXXXX, KC_L,    KC_R,    KC_A,    KC_E,              KC_I,         /*|*/KC_D,               KC_T,                  KC_N,     KC_S,    KC_G,    XXXXXXX,        
    XXXXXXX, KC_Q,    KC_Z,    SE_AA,   SE_OSLH,           KC_Y,         /*|*/KC_F,               KC_P,                  KC_V,     KC_DOT,  KC_W,    XXXXXXX,      
                                        LT(SYM, KC_ENTER), LT(NAV, KC_V),/*|*/LT(NAV, KC_ESCAPE), LT(KC_SPACE, KC_SPACE)
  ),                                   
  [SYM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  /*|*/XXXXXXX, XXXXXXX,           XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, SE_QUES, U_SE_LESS, U_SE_GRTR, SE_MINS,  U_SE_PIPE,/*|*/SE_CIRC, U_SE_LCBR,         U_SE_RCBR,  SE_DLR,  SE_ACUT, SE_GRV,         
    XXXXXXX, KC_EXLM, SE_ASTR,   SE_SLSH,   SE_EQL,   SE_AMPR,  /*|*/KC_HASH, SE_LPRN,           SE_RPRN,    SE_SCLN, SE_DQUO, XXXXXXX,        
    XXXXXXX, SE_TILD, SE_PLUS,   SE_LBRC,   SE_RBRC,  SE_AT,    /*|*/SE_UNDS, SE_COLN,           KC_COMMA,   KC_DOT,  SE_APOS, XXXXXXX,          
                                            KC_ENTER, XXXXXXX,  /*|*/KC_PERC, LT(MOD, U_SE_BSLS)
  ),
  [NUM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,/*|*/XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, KC_F10,  KC_F3,   KC_F2,   KC_F1,   XXXXXXX,/*|*/XXXXXXX, KC_1,    KC_2,    KC_3,    XXXXXXX, XXXXXXX,          
    XXXXXXX, KC_F11,  KC_F6,   KC_F5,   KC_F4,   XXXXXXX,/*|*/XXXXXXX, KC_4,    KC_5,    KC_6,    KC_0,    XXXXXXX,           
    XXXXXXX, KC_F12,  KC_F9,   KC_F8,   KC_F7,   XXXXXXX,/*|*/XXXXXXX, KC_7,    KC_8,    KC_9,    XXXXXXX, XXXXXXX,          
                                        MO(MOD), XXXXXXX,/*|*/XXXXXXX, KC_SPACE
  ), 
  [EXT] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,/*|*/XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,
    XXXXXXX, KC_ESCAPE,     KC_TAB,        KC_ENTER,      KC_SPACE,      XXXXXXX,/*|*/XXXXXXX, KC_SPACE,      KC_ENTER,      KC_TAB,       KC_ESCAPE,     XXXXXXX,
    XXXXXXX, OSM(MOD_LALT), OSM(MOD_LGUI), OSM(MOD_LCTL), OSM(MOD_LSFT), XXXXXXX,/*|*/XXXXXXX, OSM(MOD_RSFT), OSM(MOD_RCTL), OSM(MOD_RGUI), OSM(MOD_RALT), XXXXXXX,
    XXXXXXX, KC_CAPS,       CW_TOGG,       KC_DELETE,     KC_BSPC,       XXXXXXX,/*|*/XXXXXXX, KC_BSPC,       KC_DELETE,     CW_TOGG,       KC_CAPS,       XXXXXXX,
                                                          XXXXXXX,       XXXXXXX,/*|*/XXXXXXX, XXXXXXX
  ),  
  [NAV] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,     XXXXXXX,     XXXXXXX,    XXXXXXX,   XXXXXXX,        /*|*/XXXXXXX,          XXXXXXX,     XXXXXXX,       XXXXXXX,     XXXXXXX,      XXXXXXX,
    XXXXXXX, U_FIND_PREV, U_FIND_NEXT, U_SEARCH,   U_REPLACE, XXXXXXX,        /*|*/XXXXXXX,          U_DOC_LEFT,  U_DOC_DOWN,    U_DOC_UP,    U_DOC_RIGHT,  XXXXXXX,          
    XXXXXXX, U_SAVE,      U_CUT,       U_COPY,     U_PASTE,   SELWORD,        /*|*/XXXXXXX,          KC_LEFT,     KC_DOWN,       KC_UP,       KC_RIGHT,     XXXXXXX,          
    XXXXXXX, U_UNDO,      U_REDO,      U_MARK_ALL, SELLINE,   SELWBAK,        /*|*/XXXXXXX,          U_WORD_LEFT, U_5_ROWS_DOWN, U_5_ROWS_UP, U_WORD_RIGHT, XXXXXXX,          
                                                   XXXXXXX,   LT(SYS, KC_TAB),/*|*/LT(SYS, KC_BSPC), XXXXXXX     
  ), 
  [SYS] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX,           XXXXXXX,         XXXXXXX,       XXXXXXX,       /*|*/XXXXXXX,       XXXXXXX,             XXXXXXX,       XXXXXXX,       XXXXXXX,                   XXXXXXX,
    XXXXXXX, XXXXXXX, KC_AUDIO_VOL_DOWN, KC_AUDIO_VOL_UP, KC_AUDIO_MUTE, U_SCREENSHOT,  /*|*/U_OS_SEARCH,   MS_WHLL,             MS_WHLD,       MS_WHLU,       MS_WHLR,                   XXXXXXX,          
    XXXXXXX, XXXXXXX, MS_BTN2,           MS_BTN3,         MS_BTN1,       U_TOGGLE_ALPHA,/*|*/XXXXXXX,       MS_LEFT,             MS_DOWN,       MS_UP,         MS_RGHT,                   XXXXXXX,         
    XXXXXXX, XXXXXXX, RM_VALD,           RM_VALU,         U_RGB_TOG,     U_TOGGLE_OS,   /*|*/U_LOCK_SCREEN, KC_MEDIA_PREV_TRACK, KC_MEDIA_STOP, KC_MEDIA_PLAY_PAUSE, KC_MEDIA_NEXT_TRACK, XXXXXXX,          
                                                          XXXXXXX,       XXXXXXX,       /*|*/XXXXXXX,       XXXXXXX
  )
};

/* ######### LEDMAPS ######### */
const HSV PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA0] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {10,245,232}, {10,245,232},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {83,245,131}, {10,245,232}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {10,245,232}, {10,245,232}
  },

  [ALPHA1] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131}, {83,245,131},
    {10,245,232}, {10,245,232},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {83,245,131}, {83,245,131}, {10,245,232}, {83,245,131}, {83,245,131}, {83,245,131},
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

  [SYS] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {101,238,158}, {101,238,158}, {101,238,158}, {70,218,255}, 
    {0,0,0}, {0,0,0}, {71,232,162}, {71,232,162}, {71,232,162}, {149,245,100}, 
    {0,0,0}, {0,0,0}, {30,239,216}, {30,239,216}, {30,239,216}, {85,255,200}, 
    {0,0,0}, {0,0,0}, 
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {70,218,255}, {71,232,162}, {71,232,162}, {71,232,162}, {71,232,162}, {0,0,0}, 
    {0,0,0}, {71,232,162}, {71,232,162}, {71,232,162}, {71,232,162}, {0,0,0}, 
    {0,255,200}, {180,255,255}, {180,255,255}, {180,255,255}, {180,255,255}, {0,0,0}, 
    {0,0,0}, {0,0,0},
  }
};

const HSV PROGMEM ledmap_alt[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA0] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {83,245,131}, {83,245,131},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {10,245,232}, {83,245,131}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {83,245,131}, {83,245,131}
  },

  [ALPHA1] = {
    // Left side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {83,245,131}, {83,245,131},
    // Right side
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
    {10,245,232}, {10,245,232}, {83,245,131}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232}, {10,245,232},
    {83,245,131}, {83,245,131}
  }
};

/* ######### LED CONTROL FUNCTIONS ######### */

uint16_t hsv_value_for_os_animation(uint8_t index, uint8_t max_v) {
  uint16_t period = 1000;
  uint16_t period_offset = 100;
  uint16_t phase_offset = index * period_offset;
  uint16_t t = (timer_read() + phase_offset) % period;
  uint16_t value = (t < period/2) 
    ? (t * max_v) / (period/2) 
    : max_v - ((t - period/2) * max_v) / (period/2);
  return value;
}

// Pulses a single key back and forth
uint16_t hsv_value_for_alpha_animation(uint8_t max_v) {
    uint16_t period = 1000;
    uint16_t t = timer_read() % period;
    uint16_t value = (t < period / 2)
      ? (t * max_v) / (period / 2)
      : max_v - ((t - period / 2) * max_v) / (period / 2);
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
  if ((layer == ALPHA0 || layer == ALPHA1) && capslock_active && index == 6) {
    hsv = (HSV) { 0, 0, 180 };
  } else if ((layer == ALPHA0 || layer == ALPHA1) && current_os == OS_MAC) {
    hsv = pgm_read_hsv(&ledmap_alt[layer][index]);
  } else {
    hsv = pgm_read_hsv(&ledmap[layer][index]);
  } 
  return hsv;
}

bool try_set_leds_for_layer_with_os_animation(uint8_t layer) {
  if (!animation_os_timer) {
    return false;
  } 

  if (timer_elapsed(animation_os_timer) > 5000) {
    animation_os_timer = 0; 
  }

  // Set the runtime buffer from PROGMEM for a given layer, and sets pulsating value
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {    
    HSV hsv = pgm_read_hsv_for_layer(layer, i);
    hsv.v = hsv.v != 0 ? hsv_value_for_os_animation(i, hsv.v) : 0;
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
  }

  return true;
}

bool try_set_leds_for_layer_with_alpha_animation(uint8_t layer) {
  if (!animation_alpha_timer) {
    return false;
  } 

  if (timer_elapsed(animation_alpha_timer) > 5000) {
    animation_alpha_timer = 0; 
  }

  uint8_t alpha_key = current_alpha == ALPHA_QWERTY ? 33 : 34;

  // Set the runtime buffer from PROGMEM for a given layer, and sets pulsating value
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {    
    HSV hsv = pgm_read_hsv_for_layer(layer, i);
    if (i == alpha_key) {
      hsv.v = hsv_value_for_alpha_animation(hsv.v);
    }
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
  if (try_set_leds_for_layer_with_os_animation(active_layer) ||
      try_set_leds_for_layer_with_alpha_animation(active_layer)) {
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

void update_eeprom(void) {
  eeconfig_update_user(
    ((current_os & 0x0F) << 4) |
    (current_alpha & 0x0F));
}

void load_eeprom(void) {
  // Load OS and ALPHA from EEPROM
  if (!eeconfig_is_enabled()) {
    eeconfig_init();
    current_os = OS_WINDOWS;
    current_alpha = ALPHA_QWERTY;
    update_eeprom();
  } else {
    uint8_t ee = eeconfig_read_user();
    current_os = (ee >> 4) & 0x0F;
    current_alpha = ee & 0x0F;
    layer_move(current_alpha);
  }
}

/* ######### INITIALIZATION HOOK ######### */

void keyboard_post_init_user(void) {
  // Called after keyboard finishes intitialization
  rgb_matrix_enable(); // enable rgb matrix in qmk
  load_eeprom(); // Read OS and alpha layer
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
  animation_os_timer = timer_read();
  animation_alpha_timer = 0;
  current_os = current_os == OS_MAC ? OS_WINDOWS : OS_MAC;
  update_eeprom();
}

void flip_alpha(void) {
  animation_alpha_timer = timer_read();
  animation_os_timer = 0;
  current_alpha = current_alpha == ALPHA_QWERTY ? ALPHA_KVIKK : ALPHA_QWERTY;
  layer_move(current_alpha);
  update_eeprom();
}

void undo_win(bool pressed) {
  if (pressed){
    register_code(KC_LCTL); 
    register_code(KC_Z);
  } else {
    unregister_code(KC_Z);
    unregister_code(KC_LCTL);
  } 
}

void undo_mac(bool pressed) {
  if (pressed){
    register_code(KC_LGUI); 
    register_code(KC_Z);
  } else {
    unregister_code(KC_Z); 
    unregister_code(KC_LGUI);
  } 
}

void redo_win(bool pressed) {
  if (pressed){
    register_code(KC_LCTL);
    register_code(KC_LSFT); 
    register_code(KC_Z);
  } else {
    unregister_code(KC_Z); 
    unregister_code(KC_LSFT); 
    unregister_code(KC_LCTL);
  }
}

void redo_mac(bool pressed) {
  if (pressed){
    register_code(KC_LGUI); 
    register_code(KC_LSFT); 
    register_code(KC_Z);
  } else {
    unregister_code(KC_Z); 
    unregister_code(KC_LSFT); 
    unregister_code(KC_LGUI);
  }
}

void word_left_win(bool pressed) {
  if (pressed){
    register_code(KC_LCTL); 
    register_code(KC_LEFT);
  } else {
    unregister_code(KC_LEFT); 
    unregister_code(KC_LCTL);
  }
}

void word_left_mac(bool pressed) {
  if (pressed){
    register_code(KC_LALT);   
    register_code(KC_LEFT);
  } else {
    unregister_code(KC_LEFT); 
    unregister_code(KC_LALT);
  }
}

void word_right_win(bool pressed) {
  if (pressed){
    register_code(KC_LCTL);
    register_code(KC_RIGHT);
  } else {
    unregister_code(KC_RIGHT); 
    unregister_code(KC_LCTL);
  }
}

void word_right_mac(bool pressed) {
  if (pressed){
    register_code(KC_LALT);
    register_code(KC_RIGHT);
  } else {
    unregister_code(KC_RIGHT); 
    unregister_code(KC_LALT);
  }
}

void five_rows_down(bool pressed) {
  if (pressed){
    fast_cursor_down_active = true;
    fast_cursor_down_timer = timer_read(); 
    fast_cursor_down_last_repeat = 0;
    fast_cursor_move_down(5);
  } else {
    fast_cursor_down_active = false;  
  }
}

void five_rows_up(bool pressed) {
  if (pressed){
    fast_cursor_up_active = true;
    fast_cursor_up_timer = timer_read(); 
    fast_cursor_up_last_repeat = 0;
    fast_cursor_move_up(5);
  } else {
    fast_cursor_up_active = false;  
  }
}

bool process_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case RGB_SLD:            rgblight_mode(1);                                                    break;
    case U_RGB_TOG:          rgb_matrix_toggle();                                                 return false;
    case U_TOGGLE_OS:        flip_os();                                                           return false;
    case U_TOGGLE_ALPHA:     flip_alpha();                                                        return false;
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
    case U_UNDO:             PERFORM_BY_OS(undo_win(true),           undo_mac(true));             break;
    case U_REDO:             PERFORM_BY_OS(redo_win(true),           redo_mac(true));             break;
    case U_SAVE:             PERFORM_BY_OS(tap_code16(C(KC_S)),      tap_code16(G(KC_S)));        break;
    case U_MARK_ALL:         PERFORM_BY_OS(tap_code16(C(KC_A)),      tap_code16(G(KC_A)));        break;
    case U_DOC_LEFT:         PERFORM_BY_OS(tap_code(KC_HOME),        tap_code16(G(KC_LEFT)));     break;
    case U_DOC_DOWN:         PERFORM_BY_OS(tap_code16(C(KC_END)),    tap_code16(G(KC_DOWN)));     break;
    case U_DOC_UP:           PERFORM_BY_OS(tap_code16(C(KC_HOME)),   tap_code16(G(KC_UP)));       break;
    case U_DOC_RIGHT:        PERFORM_BY_OS(tap_code(KC_END),         tap_code16(G(KC_RIGHT)));    break;
    case U_WORD_LEFT:        PERFORM_BY_OS(word_left_win(true),      word_left_mac(true));        break;
    case U_WORD_RIGHT:       PERFORM_BY_OS(word_right_win(true),     word_right_mac(true));       break;
    case U_5_ROWS_DOWN:      five_rows_down(true);                                                return false;
    case U_5_ROWS_UP:        five_rows_up(true);                                                  return false;
    case U_SCREENSHOT:       PERFORM_BY_OS(tap_code16(G(S(KC_S))),   tap_code16(G(S(KC_4))));     break;
    case U_OS_SEARCH:        PERFORM_BY_OS(tap_code16(G(KC_S)),      tap_code16(G(KC_SPACE)));    break;
    case U_LOCK_SCREEN:      PERFORM_BY_OS(tap_code16(G(KC_L)),      tap_code16(C(G(KC_Q))));     break;
  }
  return true;
}

bool process_non_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case U_5_ROWS_DOWN:      five_rows_down(false);                                               break;
    case U_5_ROWS_UP:        five_rows_up(false);                                                 break;
    case U_UNDO:             PERFORM_BY_OS(undo_win(false),          undo_mac(false));            break;
    case U_REDO:             PERFORM_BY_OS(redo_win(false),          redo_mac(false));            break;
    case U_WORD_LEFT:        PERFORM_BY_OS(word_left_win(false),     word_left_mac(false));       break;
    case U_WORD_RIGHT:       PERFORM_BY_OS(word_right_win(false),    word_right_mac(false));      break;
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
  if (fast_cursor_up_active) {
    uint16_t elapsed = timer_elapsed(fast_cursor_up_timer);
    if (elapsed > FAST_CURSOR_INITIAL_DELAY &&
      elapsed - fast_cursor_up_last_repeat > FAST_CURSOR_INTERVAL) {
      fast_cursor_move_up(5);
      fast_cursor_up_last_repeat = elapsed;
    }
  }

  if (fast_cursor_down_active) {
    uint16_t elapsed = timer_elapsed(fast_cursor_down_timer);
    if (elapsed > FAST_CURSOR_INITIAL_DELAY &&
      elapsed - fast_cursor_down_last_repeat > FAST_CURSOR_INTERVAL) {
      fast_cursor_move_down(5);
      fast_cursor_down_last_repeat = elapsed;
    }
  }
}