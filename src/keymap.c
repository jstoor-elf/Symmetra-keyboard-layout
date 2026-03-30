/* ######### INCLUDES ######### */

#include QMK_KEYBOARD_H
#include "swedish_keys.h"
#include "version.h"
#include <math.h>

/* ######### DEFINES ######### */

#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

/* ######### THUMB KEY ALIASES ######### */

#define T_L_OUT OSL(SYM_L)
#define T_L_IN  LT(NAV, KC_SPC)
#define T_R_IN  OSM(MOD_RSFT)
#define T_R_OUT OSL(SYM_R)

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  SYM_L,
  SYM_R,
  NUM,
  FUNC,
  NAV
};

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
  U_TOGGLE_OS,
  U_NUM_ENTER
};

typedef enum {
  OS_WINDOWS,
  OS_MAC
} os_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os = OS_WINDOWS;
uint16_t animation_os_timer = 0;
extern rgb_config_t rgb_matrix_config;

static bool fast_cursor_up_active = false;
static uint16_t fast_cursor_up_timer = 0;
static uint16_t fast_cursor_up_last_repeat = 0;

static bool fast_cursor_down_active = false;
static uint16_t fast_cursor_down_timer = 0;
static uint16_t fast_cursor_down_last_repeat = 0;

/* ######### COMBOS ######### */

const uint16_t PROGMEM combo_aa[]   = {SE_ADIA, SE_OSLH, COMBO_END};
const uint16_t PROGMEM combo_num[]  = {T_L_OUT, T_R_OUT, COMBO_END};
const uint16_t PROGMEM combo_caps[] = {T_L_IN,  T_R_IN,  COMBO_END};
const uint16_t PROGMEM combo_func[] = {KC_G,    KC_P,    COMBO_END};
const uint16_t PROGMEM combo_eql[]  = {KC_Y,    KC_O,    COMBO_END};
const uint16_t PROGMEM combo_coln[] = {KC_H,    KC_A,    COMBO_END};
const uint16_t PROGMEM combo_slsh[] = {KC_F,    SE_ADIA, COMBO_END};
const uint16_t PROGMEM combo_ent[]  = {KC_A,    KC_E,    COMBO_END}; 
const uint16_t PROGMEM combo_esc[]  = {KC_T,    KC_S,    COMBO_END};
const uint16_t PROGMEM combo_tab[]  = {KC_R,    KC_T,    COMBO_END};
const uint16_t PROGMEM combo_bspc[] = {KC_O,    KC_U,    COMBO_END};
const uint16_t PROGMEM combo_del[]  = {KC_Q,    KC_M,    COMBO_END};

combo_t key_combos[] = {
  COMBO(combo_aa,   SE_AA),
  COMBO(combo_num,  TG(NUM)),
  COMBO(combo_caps, CW_TOGG),
  COMBO(combo_func, OSL(FUNC)),
  COMBO(combo_eql,  SE_EQL),
  COMBO(combo_coln, SE_COLN),
  COMBO(combo_slsh, SE_SLSH),
  COMBO(combo_ent,  KC_ENT),
  COMBO(combo_esc,  KC_ESC),
  COMBO(combo_bspc, KC_BSPC),
  COMBO(combo_del,  KC_DEL),
  COMBO(combo_tab,  KC_TAB),
};

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [ALPHA] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX,
    XXXXXXX, KC_B,    KC_L,    KC_D,    KC_C,    KC_V,       KC_J,    KC_Y,    KC_O,    KC_U,    KC_COMMA, XXXXXXX,
    XXXXXXX, KC_N,    KC_R,    KC_T,    KC_S,    KC_G,       KC_P,    KC_H,    KC_A,    KC_E,    KC_I,     XXXXXXX,
    XXXXXXX, KC_X,    KC_Q,    KC_M,    KC_W,    KC_Z,       KC_K,    KC_F,    SE_ADIA, SE_OSLH, KC_DOT,   XXXXXXX,
                                        T_L_OUT, T_L_IN,     T_R_IN,  T_R_OUT
  ),

  [SYM_L] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX,  XXXXXXX,
    XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,    SE_CIRC, KC_HASH, SE_AT,   SE_DQUO,  KC_COMMA, XXXXXXX,
    XXXXXXX, OSM(MOD_LALT), OSM(MOD_LGUI), OSM(MOD_LCTL), OSM(MOD_LSFT), XXXXXXX,    SE_PERC, SE_SCLN, SE_LBRC, SE_RBRC,  SE_UNDS,  XXXXXXX,
    XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,    SE_ACUT, SE_AMPR, SE_LPRN, SE_RPRN,  KC_DOT,   XXXXXXX, 
                                                          XXXXXXX,       XXXXXXX,    XXXXXXX, XXXXXXX
  ),

  [SYM_R] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX,    XXXXXXX,   XXXXXXX,     XXXXXX,  XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,
    XXXXXXX, SE_DLR,   SE_PLUS,   SE_ASTR,    KC_EXLM,   SE_TILD,     XXXXXX,  XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,
    XXXXXXX, SE_QUES,  U_SE_LCBR, U_SE_RCBR,  SE_MINS,   U_SE_BSLS,   XXXXXXX, OSM(MOD_RSFT), OSM(MOD_RCTL), OSM(MOD_RGUI), OSM(MOD_RALT), XXXXXXX,
    XXXXXXX, SE_APOS,  U_SE_LESS, U_SE_GRTR,  U_SE_PIPE, SE_GRV,      XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,
                                              XXXXXXX,  XXXXXXX,      XXXXXXX, XXXXXXX
  ),

  [NUM] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, KC_6,    KC_4,    KC_2,    KC_0,    XXXXXXX,          XXXXXXX, KC_1,    KC_3,    KC_5,    KC_7,    XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_8,    XXXXXXX,          XXXXXXX, KC_9,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                        XXXXXXX, U_NUM_ENTER,      TG(NUM), XXXXXXX
  ),
 
  [FUNC] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,
    XXXXXXX, KC_F6,   KC_F4,   KC_F2,   KC_F10,  KC_F12,     KC_F11,  KC_F1,   KC_F3,   KC_F5,   KC_F7,      XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_F8,   XXXXXXX,    XXXXXXX, KC_F9,   XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,
                                        XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX
  ),

  [NAV] = LAYOUT_voyager(
    XXXXXXX, XXXXXXX,     XXXXXXX,     XXXXXXX,    XXXXXXX,   XXXXXXX,      XXXXXXX, XXXXXXX,     XXXXXXX,       XXXXXXX,     XXXXXXX,      XXXXXXX,
    XXXXXXX, U_SAVE,      U_CUT,       U_COPY,     U_PASTE,   U_SEARCH,     XXXXXXX, U_DOC_LEFT,  U_DOC_DOWN,    U_DOC_UP,    U_DOC_RIGHT,  XXXXXXX,
    XXXXXXX, MOD_LALT,    MOD_LGUI,    MOD_LCTL,   MOD_LSFT,  SELWORD,      XXXXXXX, KC_LEFT,     KC_DOWN,       KC_UP,       KC_RIGHT,     XXXXXXX,
    XXXXXXX, U_UNDO,      U_REDO,      U_MARK_ALL, SELLINE,   SELWBAK,      XXXXXXX, U_WORD_LEFT, U_5_ROWS_DOWN, U_5_ROWS_UP, U_WORD_RIGHT, XXXXXXX,
                                                   XXXXXXX,   XXXXXXX,      XXXXXXX, XXXXXXX
  )
};

/* ######### LEDMAPS ######### */

// LED index layout per side (26 LEDs each):
//   0- 5: top row (disabled)
//   6-11: row 2
//  12-17: home row
//  18-23: bottom row
//  24-25: thumbs (L: outer=24, inner=25 | R: inner=50, outer=51)

#define C_OFF  {0,   0,   0  }
#define C_KEY  {83,  245, 131}  // green        – alpha keys
#define C_THB  {10,  245, 232}  // cyan         – thumb keys
#define C_MOD  {20,  230, 210}  // orange       – one-shot modifiers
#define C_SYM  {190, 200, 200}  // purple       – symbols
#define C_NUM  {120, 223, 209}  // teal         – numbers
#define C_FN   {30,  241, 180}  // yellow       – function keys
#define C_ACT  {0,   215, 255}  // red          – special actions (enter/cancel)
// NAV layer
#define C_NFND {46,  248, 241}  // blue         – find/search keys
#define C_NEDT {220, 238, 216}  // light green  – edit keys (copy/paste/save)
#define C_NSEL {5,   201, 240}  // steel blue   – selection keys
#define C_NUND {100, 255, 255}  // yellow-green – undo/redo
#define C_NTHB {0,   0,   180}  // dim blue     – nav thumb
#define C_NDOC {169, 243, 216}  // teal         – document navigation
#define C_NARW {83,  234, 140}  // green        – arrow keys
#define C_NWRD {18,  250, 229}  // teal         – word navigation
// MOUSE layer
#define C_MBTN {137, 241, 207}  // green        – mouse buttons
#define C_MWHL {26,  242, 207}  // teal         – mouse wheel
#define C_MMOV {71,  232, 162}  // dark green   – mouse movement

const HSV PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,   // top row
    C_OFF, C_KEY, C_KEY, C_KEY, C_KEY, C_KEY,   // B L D C V
    C_OFF, C_KEY, C_KEY, C_KEY, C_KEY, C_KEY,   // N R T S G
    C_OFF, C_KEY, C_KEY, C_KEY, C_KEY, C_KEY,   // X Q M W Z
    C_THB, C_THB,                               // T_L_OUT, T_L_IN
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_KEY, C_KEY, C_KEY, C_KEY, C_KEY, C_OFF,   // J Y O U ,
    C_KEY, C_KEY, C_KEY, C_KEY, C_KEY, C_OFF,   // P H A E I
    C_KEY, C_KEY, C_KEY, C_KEY, C_KEY, C_OFF,   // K F Ä Ö .
    C_THB, C_THB                                // T_R_IN, T_R_OUT
  },

  [SYM_L] = {
    // Left side – home row gets OSMs, rest off
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_MOD, C_MOD, C_MOD, C_MOD, C_OFF,  // Alt GUI Ctrl Sft
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_THB, C_OFF,
    // Right side – symbols
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_SYM, C_SYM, C_SYM, C_SYM, C_SYM, C_OFF,  // ^ # @ " ,
    C_SYM, C_SYM, C_SYM, C_SYM, C_SYM, C_OFF,  // / ? [ ] -
    C_SYM, C_SYM, C_SYM, C_SYM, C_SYM, C_OFF,  // ` & ( ) .
    C_OFF, C_OFF
  },

  [SYM_R] = {
    // Left side – symbols
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_SYM, C_SYM, C_SYM, C_SYM, C_SYM,  // $ + * ! ~
    C_OFF, C_SYM, C_SYM, C_SYM, C_SYM, C_SYM,  // | { } - \
    C_OFF, C_SYM, C_SYM, C_SYM, C_SYM, C_SYM,  // ' < > % ,
    C_OFF, C_OFF,
    // Right side – home row gets OSMs, rest off
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_MOD, C_MOD, C_MOD, C_MOD, C_OFF,  // Sft Ctrl GUI Alt
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_THB
  },

  [NUM] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_NUM, C_NUM, C_NUM, C_NUM, C_OFF,   // 6 4 0 2
    C_OFF, C_OFF, C_OFF, C_OFF, C_NUM, C_OFF,   // 8
    C_ACT, C_OFF,                               // Enter, blocked
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_NUM, C_NUM, C_NUM, C_NUM, C_OFF,   // 3 1 5 7
    C_OFF, C_NUM, C_OFF, C_OFF, C_OFF, C_OFF,   // 9
    C_OFF, C_ACT                                // blocked, Cancel
  },

  [FUNC] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_FN,  C_FN,  C_FN,  C_FN,  C_FN,   // F6 F4 F10 F2 F12
    C_OFF, C_OFF, C_OFF, C_OFF, C_FN,  C_OFF,   // F8
    C_OFF, C_OFF,
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,
    C_FN,  C_FN,  C_FN,  C_FN,  C_FN,  C_OFF,  // F11 F3 F1 F5 F7
    C_OFF, C_FN,  C_OFF, C_OFF, C_OFF, C_OFF,   // F9
    C_OFF, C_OFF
  },

  [NAV] = {
    // Left side
    C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,
    C_OFF,  C_NFND, C_NFND, C_NFND, C_NFND, C_OFF,   // find/search/replace
    C_OFF,  C_NEDT, C_NEDT, C_NEDT, C_NEDT, C_NSEL,  // save/cut/copy/paste, selword
    C_OFF,  C_NUND, C_NUND, C_NSEL, C_NSEL, C_NSEL,  // undo/redo, mark/selline/selwbak
    C_NTHB, C_OFF,
    // Right side
    C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,
    C_OFF,  C_NDOC, C_NDOC, C_NDOC, C_NDOC, C_OFF,   // doc navigation
    C_OFF,  C_NARW, C_NARW, C_NARW, C_NARW, C_OFF,   // arrow keys
    C_OFF,  C_NWRD, C_NWRD, C_NWRD, C_NWRD, C_OFF,   // word/fast navigation
    C_NTHB, C_OFF
  }
};

/* ######### LED CONTROL FUNCTIONS ######### */

uint16_t hsv_value_for_os_animation(uint8_t index, uint8_t max_v) {
  uint16_t period = 1000;
  uint16_t period_offset = 100;
  uint16_t phase_offset = index * period_offset;
  uint16_t t = (timer_read() + phase_offset) % period;
  return (t < period / 2)
    ? (t * max_v) / (period / 2)
    : max_v - ((t - period / 2) * max_v) / (period / 2);
}

RGB hsv_to_rgb_with_value(HSV hsv) {
  RGB rgb = hsv_to_rgb(hsv);
  float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
  return (RGB){ f * rgb.r, f * rgb.g, f * rgb.b };
}

static HSV pgm_read_hsv(const HSV *addr) {
  HSV hsv;
  hsv.h = pgm_read_byte(&addr->h);
  hsv.s = pgm_read_byte(&addr->s);
  hsv.v = pgm_read_byte(&addr->v);
  return hsv;
}

bool try_set_leds_for_layer_with_os_animation(uint8_t layer) {
  if (!animation_os_timer) return false;
  if (timer_elapsed(animation_os_timer) > 5000) {
    animation_os_timer = 0;
    return false;
  }
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = pgm_read_hsv(&ledmap[layer][i]);
    hsv.v = hsv.v != 0 ? hsv_value_for_os_animation(i, hsv.v) : 0;
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
  }
  return true;
}

void set_leds_for_layer(uint8_t layer) {
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = pgm_read_hsv(&ledmap[layer][i]);
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
  }
}

bool rgb_matrix_indicators_user(void) {
  uint8_t active_layer = biton32(layer_state);
  if (try_set_leds_for_layer_with_os_animation(active_layer)) {
    return false;
  }
  set_leds_for_layer(active_layer);
  return true;
}

/* ######### EEPROM STATE ######### */

void update_eeprom(void) {
  eeconfig_update_user(current_os & 0x0F);
}

void load_eeprom(void) {
  if (!eeconfig_is_enabled()) {
    eeconfig_init();
    current_os = OS_WINDOWS;
    update_eeprom();
  } else {
    uint8_t ee = eeconfig_read_user();
    current_os = ee & 0x0F;
  }
}

/* ######### INITIALIZATION HOOK ######### */

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
  load_eeprom();
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
  for (int i = 0; i < lines; i++) tap_code(KC_UP);
}

void fast_cursor_move_down(uint16_t lines) {
  for (int i = 0; i < lines; i++) tap_code(KC_DOWN);
}

bool select_word_host_is_mac(void) {
  return current_os == OS_MAC;
}

void flip_os(void) {
  animation_os_timer = timer_read();
  current_os = current_os == OS_MAC ? OS_WINDOWS : OS_MAC;
  update_eeprom();
}

void undo_win(bool pressed) {
  if (pressed) { register_code(KC_LCTL); register_code(KC_Z); }
  else         { unregister_code(KC_Z);  unregister_code(KC_LCTL); }
}

void undo_mac(bool pressed) {
  if (pressed) { register_code(KC_LGUI); register_code(KC_Z); }
  else         { unregister_code(KC_Z);  unregister_code(KC_LGUI); }
}

void redo_win(bool pressed) {
  if (pressed) { register_code(KC_LCTL); register_code(KC_LSFT); register_code(KC_Z); }
  else         { unregister_code(KC_Z);  unregister_code(KC_LSFT); unregister_code(KC_LCTL); }
}

void redo_mac(bool pressed) {
  if (pressed) { register_code(KC_LGUI); register_code(KC_LSFT); register_code(KC_Z); }
  else         { unregister_code(KC_Z);  unregister_code(KC_LSFT); unregister_code(KC_LGUI); }
}

void word_left_win(bool pressed) {
  if (pressed) { register_code(KC_LCTL); register_code(KC_LEFT); }
  else         { unregister_code(KC_LEFT); unregister_code(KC_LCTL); }
}

void word_left_mac(bool pressed) {
  if (pressed) { register_code(KC_LALT); register_code(KC_LEFT); }
  else         { unregister_code(KC_LEFT); unregister_code(KC_LALT); }
}

void word_right_win(bool pressed) {
  if (pressed) { register_code(KC_LCTL); register_code(KC_RIGHT); }
  else         { unregister_code(KC_RIGHT); unregister_code(KC_LCTL); }
}

void word_right_mac(bool pressed) {
  if (pressed) { register_code(KC_LALT); register_code(KC_RIGHT); }
  else         { unregister_code(KC_RIGHT); unregister_code(KC_LALT); }
}

void five_rows_down(bool pressed) {
  if (pressed) {
    fast_cursor_down_active = true;
    fast_cursor_down_timer = timer_read();
    fast_cursor_down_last_repeat = 0;
    fast_cursor_move_down(5);
  } else {
    fast_cursor_down_active = false;
  }
}

void five_rows_up(bool pressed) {
  if (pressed) {
    fast_cursor_up_active = true;
    fast_cursor_up_timer = timer_read();
    fast_cursor_up_last_repeat = 0;
    fast_cursor_move_up(5);
  } else {
    fast_cursor_up_active = false;
  }
}

/* ######### MAIN KEY PROCESSING ######### */

bool process_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case RGB_SLD:       rgblight_mode(1);                                                    break;
    case U_TOGGLE_OS:   flip_os();                                                           return false;
    case U_NUM_ENTER:   tap_code(KC_ENT); layer_off(NUM);                                   return false;
    case U_SE_LESS:     PERFORM_BY_OS(tap_code16(SE_LESS_WIN),  tap_code16(SE_LESS_MAC));   break;
    case U_SE_GRTR:     PERFORM_BY_OS(tap_code16(SE_GRTR_WIN),  tap_code16(SE_GRTR_MAC));   break;
    case U_SE_PIPE:     PERFORM_BY_OS(tap_code16(SE_PIPE_WIN),  tap_code16(SE_PIPE_MAC));   break;
    case U_SE_LCBR:     PERFORM_BY_OS(tap_code16(SE_LCBR_WIN),  tap_code16(SE_LCBR_MAC));   break;
    case U_SE_RCBR:     PERFORM_BY_OS(tap_code16(SE_RCBR_WIN),  tap_code16(SE_RCBR_MAC));   break;
    case U_SE_BSLS:     PERFORM_BY_OS(tap_code16(SE_BSLS_WIN),  tap_code16(SE_BSLS_MAC));   break;
    case U_FIND_PREV:   PERFORM_BY_OS(tap_code16(S(KC_F3)),     tap_code16(G(S(KC_G))));    break;
    case U_FIND_NEXT:   PERFORM_BY_OS(tap_code16(C(KC_G)),      tap_code16(G(KC_G)));       break;
    case U_SEARCH:      PERFORM_BY_OS(tap_code16(C(KC_F)),      tap_code16(G(KC_F)));       break;
    case U_REPLACE:     PERFORM_BY_OS(tap_code16(C(KC_H)),      tap_code16(A(G(KC_F))));    break;
    case U_CUT:         PERFORM_BY_OS(tap_code16(C(KC_X)),      tap_code16(G(KC_X)));       break;
    case U_COPY:        PERFORM_BY_OS(tap_code16(C(KC_C)),      tap_code16(G(KC_C)));       break;
    case U_PASTE:       PERFORM_BY_OS(tap_code16(C(KC_V)),      tap_code16(G(KC_V)));       break;
    case U_UNDO:        PERFORM_BY_OS(undo_win(true),           undo_mac(true));            break;
    case U_REDO:        PERFORM_BY_OS(redo_win(true),           redo_mac(true));            break;
    case U_SAVE:        PERFORM_BY_OS(tap_code16(C(KC_S)),      tap_code16(G(KC_S)));       break;
    case U_MARK_ALL:    PERFORM_BY_OS(tap_code16(C(KC_A)),      tap_code16(G(KC_A)));       break;
    case U_DOC_LEFT:    PERFORM_BY_OS(tap_code(KC_HOME),        tap_code16(G(KC_LEFT)));    break;
    case U_DOC_DOWN:    PERFORM_BY_OS(tap_code16(C(KC_END)),    tap_code16(G(KC_DOWN)));    break;
    case U_DOC_UP:      PERFORM_BY_OS(tap_code16(C(KC_HOME)),   tap_code16(G(KC_UP)));      break;
    case U_DOC_RIGHT:   PERFORM_BY_OS(tap_code(KC_END),         tap_code16(G(KC_RIGHT)));   break;
    case U_WORD_LEFT:   PERFORM_BY_OS(word_left_win(true),      word_left_mac(true));       break;
    case U_WORD_RIGHT:  PERFORM_BY_OS(word_right_win(true),     word_right_mac(true));      break;
    case U_5_ROWS_DOWN: five_rows_down(true);                                               return false;
    case U_5_ROWS_UP:   five_rows_up(true);                                                 return false;
  }
  return true;
}

bool process_non_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case U_5_ROWS_DOWN: five_rows_down(false); break;
    case U_5_ROWS_UP:   five_rows_up(false);   break;
    case U_UNDO:        PERFORM_BY_OS(undo_win(false),       undo_mac(false));       break;
    case U_REDO:        PERFORM_BY_OS(redo_win(false),       redo_mac(false));       break;
    case U_WORD_LEFT:   PERFORM_BY_OS(word_left_win(false),  word_left_mac(false));  break;
    case U_WORD_RIGHT:  PERFORM_BY_OS(word_right_win(false), word_right_mac(false)); break;
  }
  return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
