/* ######### INCLUDES ######### */

#include QMK_KEYBOARD_H
#include "swedish_keys.h"
#include "version.h"

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
  NAV,
  MOUSE,
  SYS
};

enum custom_keycodes {
  U_SE_LESS = SAFE_RANGE,
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
  U_NUM_ENTER,
  U_SCREENSHOT,
  U_OS_SEARCH,
  U_EMOJIS
};

typedef enum {
  OS_WINDOWS,
  OS_MAC
} os_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os = OS_WINDOWS;

static bool fast_cursor_up_active = false;
static uint16_t fast_cursor_up_timer = 0;
static uint16_t fast_cursor_up_last_repeat = 0;

static bool fast_cursor_down_active = false;
static uint16_t fast_cursor_down_timer = 0;
static uint16_t fast_cursor_down_last_repeat = 0;

/* ######### COMBOS ######### */

const uint16_t PROGMEM combo_aa[]   = {SE_ADIA, SE_OSLH, COMBO_END};
const uint16_t PROGMEM combo_num[]  = {T_L_IN,  T_R_IN,  COMBO_END};
const uint16_t PROGMEM combo_func[] = {T_L_OUT, T_R_OUT, COMBO_END};
const uint16_t PROGMEM combo_caps[] = {KC_T,    KC_A,    COMBO_END};
const uint16_t PROGMEM combo_eql[]  = {KC_Y,    KC_O,    COMBO_END};
const uint16_t PROGMEM combo_coln[] = {KC_H,    KC_A,    COMBO_END};
const uint16_t PROGMEM combo_slsh[] = {KC_F,    SE_ADIA, COMBO_END};
const uint16_t PROGMEM combo_ent[]  = {KC_A,    KC_E,    COMBO_END};
const uint16_t PROGMEM combo_esc[]  = {KC_T,    KC_S,    COMBO_END};
const uint16_t PROGMEM combo_tab[]  = {KC_R,    KC_T,    COMBO_END};
const uint16_t PROGMEM combo_bspc[] = {KC_O,    KC_U,    COMBO_END};
const uint16_t PROGMEM combo_del[]  = {KC_Q,    KC_M,    COMBO_END};
const uint16_t PROGMEM combo_sys[]  = {KC_B,    KC_COMMA, COMBO_END};

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
  COMBO(combo_sys,  TG(SYS)),
};

/* ######### KEYMAPS ######### */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [ALPHA] = LAYOUT_split_3x5_2(
    KC_B,    KC_L,    KC_D,    KC_C,    KC_V,       KC_J,    KC_Y,    KC_O,    KC_U,    KC_DOT,
    KC_N,    KC_R,    KC_T,    KC_S,    KC_G,       KC_P,    KC_H,    KC_A,    KC_E,    KC_I,
    KC_X,    KC_Q,    KC_M,    KC_W,    KC_Z,       KC_K,    KC_F,    SE_ADIA, SE_OSLH, KC_COMMA,
                               T_L_OUT, T_L_IN,     T_R_IN,  T_R_OUT
  ),

  [SYM_L] = LAYOUT_split_3x5_2(
    XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,    SE_CIRC, KC_HASH, SE_AT,   SE_DQUO, KC_DOT,
    OSM(MOD_LALT), OSM(MOD_LGUI), OSM(MOD_LCTL), OSM(MOD_LSFT), XXXXXXX,    SE_PERC, SE_SCLN, SE_LBRC, SE_RBRC, SE_UNDS,
    XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,    SE_ACUT, SE_AMPR, SE_LPRN, SE_RPRN, KC_COMMA,
                                                 XXXXXXX,       XXXXXXX,    XXXXXXX, XXXXXXX
  ),

  [SYM_R] = LAYOUT_split_3x5_2(
    SE_DLR,  SE_PLUS,   SE_ASTR,   KC_EXLM,   SE_TILD,     XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,
    SE_QUES, U_SE_LCBR, U_SE_RCBR, SE_MINS,   U_SE_BSLS,   XXXXXXX, OSM(MOD_RSFT), OSM(MOD_RCTL), OSM(MOD_RGUI), OSM(MOD_RALT),
    SE_APOS, U_SE_LESS, U_SE_GRTR, U_SE_PIPE, SE_GRV,      XXXXXXX, XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,
                                   XXXXXXX,   XXXXXXX,      XXXXXXX, XXXXXXX
  ),

  [NUM] = LAYOUT_split_3x5_2(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_6,    KC_4,    KC_2,    KC_0,    XXXXXXX,          XXXXXXX, KC_1,    KC_3,    KC_5,    KC_7,
    XXXXXXX, XXXXXXX, XXXXXXX, KC_8,   XXXXXXX,          XXXXXXX, KC_9,    XXXXXXX, XXXXXXX, XXXXXXX,
                               XXXXXXX, U_NUM_ENTER,      TG(NUM), XXXXXXX
  ),

  [FUNC] = LAYOUT_split_3x5_2(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_F6,   KC_F4,   KC_F2,   KC_F10,  KC_F12,     KC_F11,  KC_F1,   KC_F3,   KC_F5,   KC_F7,
    XXXXXXX, XXXXXXX, XXXXXXX, KC_F8,   XXXXXXX,    XXXXXXX, KC_F9,   XXXXXXX, XXXXXXX, XXXXXXX,
                               XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX
  ),

  [NAV] = LAYOUT_split_3x5_2(
    U_SAVE,   U_CUT,   U_COPY,     U_PASTE, U_SEARCH,    XXXXXXX, U_DOC_LEFT,  U_DOC_DOWN,    U_DOC_UP,    U_DOC_RIGHT,
    MOD_LALT, MOD_LGUI, MOD_LCTL,  MOD_LSFT, SELWORD,    XXXXXXX, KC_LEFT,     KC_DOWN,       KC_UP,       KC_RIGHT,
    U_UNDO,   U_REDO,  U_MARK_ALL, SELLINE, SELWBAK,     XXXXXXX, U_WORD_LEFT, U_5_ROWS_DOWN, U_5_ROWS_UP, U_WORD_RIGHT,
                                   XXXXXXX, XXXXXXX,     MO(MOUSE), XXXXXXX
  ),

  [MOUSE] = LAYOUT_split_3x5_2(
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,      XXXXXXX, MS_BTN1, MS_UP,   MS_BTN2,  XXXXXXX,
    MOD_LALT, MOD_LGUI, MOD_LCTL, MOD_LSFT, XXXXXXX,      XXXXXXX, MS_LEFT, MS_DOWN, MS_RIGHT, XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,      XXXXXXX, MS_WHLD, MS_BTN3, MS_WHLU,  XXXXXXX,
                                  XXXXXXX,  XXXXXXX,      XXXXXXX, XXXXXXX
  ),

  [SYS] = LAYOUT_split_3x5_2(
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,      XXXXXXX, XXXXXXX,     XXXXXXX,       XXXXXXX,     XXXXXXX,
    XXXXXXX,  KC_VOLD,  KC_VOLU,  KC_MUTE,  XXXXXXX,      XXXXXXX, U_OS_SEARCH, U_SCREENSHOT,  U_TOGGLE_OS, U_EMOJIS,
    XXXXXXX,  KC_MPRV,  KC_MNXT,  KC_MPLY,  XXXXXXX,      XXXXXXX, XXXXXXX,     XXXXXXX,       XXXXXXX,     XXXXXXX,
                                  XXXXXXX,  XXXXXXX,      TG(SYS), XXXXXXX
  )
};

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
    case U_TOGGLE_OS:   flip_os();                                                                       return false;
    case U_SCREENSHOT:  PERFORM_BY_OS(tap_code16(G(S(KC_S))),   tap_code16(G(S(KC_4))));             break;
    case U_OS_SEARCH:   PERFORM_BY_OS(tap_code16(G(KC_S)),      tap_code16(G(KC_SPACE)));             break;
    case U_EMOJIS:      PERFORM_BY_OS(tap_code16(G(KC_DOT)),    tap_code16(C(G(KC_SPACE))));         break;
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
