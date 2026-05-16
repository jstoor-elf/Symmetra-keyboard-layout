/* ######### INCLUDES ######### */

#include QMK_KEYBOARD_H
#include "swedish_keys.h"
#include "version.h"

/* ######### DEFINES ######### */

#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

/* ######### KEY ALIASES ######### */

#define _DEAD_  XXXXXXX  // physically removed switch
#define _OFF_   XXXXXXX  // within range, unassigned on this layer

#define T_L_OUT MO(SYS)
#define T_L_IN  LT(NAV, KC_SPC)
#define T_R_IN  KC_E
#define T_R_OUT MO(FUNC)

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  NUM,
  FUNC,
  NAV,
  MOUSE,
  SYS,
  MOD
};

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  U_SE_LESS,
  U_SE_GRTR,
  U_SE_PIPE,
  U_SE_LCBR,
  U_SE_RCBR,
  U_SE_BSLS,
  U_SEARCH,
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
  U_NUM_SPACE,
  U_NUM_TGL,
  U_SCREENSHOT,
  U_OS_SEARCH,
  U_EMOJIS,
  U_LOCK_SCREEN,
  U_RGB_TOG,

  U_FIND_PREV,
  U_FIND_NEXT,
  U_REPLACE
};

typedef enum {
  OS_WINDOWS,
  OS_MAC
} os_t;

/* ######### GLOBAL VARIABLES ######### */

os_t current_os = OS_WINDOWS;

uint16_t animation_os_timer = 0;
bool caps_word_active = false;
extern rgb_config_t rgb_matrix_config;

static bool fast_cursor_up_active = false;
static uint16_t fast_cursor_up_timer = 0;
static uint16_t fast_cursor_up_last_repeat = 0;

static bool fast_cursor_down_active = false;
static uint16_t fast_cursor_down_timer = 0;
static uint16_t fast_cursor_down_last_repeat = 0;

/* ######### COMBOS ######### */

// Base combos
const uint16_t PROGMEM combo_aa[]        = {SE_ADIA,       SE_OSLH,       COMBO_END};
const uint16_t PROGMEM combo_num[]       = {T_L_IN,        T_R_IN,        COMBO_END};
const uint16_t PROGMEM combo_caps[]      = {KC_X,          KC_Q,          COMBO_END};
const uint16_t PROGMEM combo_nr_mod[]    = {KC_N,          KC_R,          COMBO_END};
const uint16_t PROGMEM combo_ts_sft[]    = {KC_T,          KC_S,          COMBO_END};
const uint16_t PROGMEM combo_ha_sft[]    = {KC_H,          KC_A,          COMBO_END};
const uint16_t PROGMEM combo_rt_ctl[]    = {KC_R,          KC_T,          COMBO_END};
const uint16_t PROGMEM combo_ai_ctl[]    = {KC_A,          KC_I,          COMBO_END};
const uint16_t PROGMEM combo_esc[]       = {KC_Q,          KC_M,          COMBO_END};
const uint16_t PROGMEM combo_tab[]       = {KC_M,          KC_W,          COMBO_END};
const uint16_t PROGMEM combo_ent[]       = {KC_F,          SE_ADIA,       COMBO_END};
const uint16_t PROGMEM combo_del[]       = {KC_I,          KC_BSPC,       COMBO_END};
// Deactivation combos
const uint16_t PROGMEM combo_num_deac[]  = {U_NUM_SPACE,   U_NUM_TGL,     COMBO_END};
// Symbol combos: Space (T_L_IN) + right-side key
const uint16_t PROGMEM combo_sym_circ[]  = {T_L_IN,        KC_J,          COMBO_END};
const uint16_t PROGMEM combo_sym_hash[]  = {T_L_IN,        KC_Y,          COMBO_END};
const uint16_t PROGMEM combo_sym_at[]    = {T_L_IN,        KC_O,          COMBO_END};
const uint16_t PROGMEM combo_sym_dquo[]  = {T_L_IN,        KC_U,          COMBO_END};
const uint16_t PROGMEM combo_sym_perc[]  = {T_L_IN,        KC_P,          COMBO_END};
const uint16_t PROGMEM combo_sym_k[]     = {T_L_IN,        KC_K,          COMBO_END};
const uint16_t PROGMEM combo_sym_scln[]  = {T_L_IN,        KC_H,          COMBO_END};
const uint16_t PROGMEM combo_sym_sft[]   = {T_L_IN,        KC_BSPC,       COMBO_END};
const uint16_t PROGMEM combo_sym_lbrc[]  = {T_L_IN,        KC_A,          COMBO_END};
const uint16_t PROGMEM combo_sym_rbrc[]  = {T_L_IN,        KC_I,          COMBO_END};
const uint16_t PROGMEM combo_sym_ampr[]  = {T_L_IN,        KC_F,          COMBO_END};
const uint16_t PROGMEM combo_sym_lprn[]  = {T_L_IN,        SE_ADIA,       COMBO_END};
const uint16_t PROGMEM combo_sym_rprn[]  = {T_L_IN,        SE_OSLH,       COMBO_END};
const uint16_t PROGMEM combo_sym_slsh[]  = {T_L_IN,        KC_COMMA,      COMBO_END};
const uint16_t PROGMEM combo_sym_eql[]   = {T_L_IN,        KC_DOT,        COMBO_END};
// Three-key symbol combos: Space + two right-side keys
const uint16_t PROGMEM combo_3_acut[]   = {T_L_IN,        KC_J,          KC_Y,    COMBO_END};
const uint16_t PROGMEM combo_3_grv[]    = {T_L_IN,        KC_P,          KC_H,    COMBO_END};
// Symbol combos: Enter (T_R_IN) + left-side key
const uint16_t PROGMEM combo_sym_dlr[]   = {T_R_IN,        KC_B,          COMBO_END};
const uint16_t PROGMEM combo_sym_plus[]  = {T_R_IN,        KC_L,          COMBO_END};
const uint16_t PROGMEM combo_sym_astr[]  = {T_R_IN,        KC_D,          COMBO_END};
const uint16_t PROGMEM combo_sym_exlm[]  = {T_R_IN,        KC_C,          COMBO_END};
const uint16_t PROGMEM combo_sym_tild[]  = {T_R_IN,        KC_V,          COMBO_END};
const uint16_t PROGMEM combo_sym_ques[]  = {T_R_IN,        KC_N,          COMBO_END};
const uint16_t PROGMEM combo_sym_lcbr[]  = {T_R_IN,        KC_R,          COMBO_END};
const uint16_t PROGMEM combo_sym_rcbr[]  = {T_R_IN,        KC_T,          COMBO_END};
const uint16_t PROGMEM combo_sym_mins[]  = {T_R_IN,        KC_S,          COMBO_END};
const uint16_t PROGMEM combo_sym_bsls[]  = {T_R_IN,        KC_G,          COMBO_END};
const uint16_t PROGMEM combo_sym_apos[]  = {T_R_IN,        KC_X,          COMBO_END};
const uint16_t PROGMEM combo_sym_less[]  = {T_R_IN,        KC_Q,          COMBO_END};
const uint16_t PROGMEM combo_sym_grtr[]  = {T_R_IN,        KC_M,          COMBO_END};
const uint16_t PROGMEM combo_sym_pipe[]  = {T_R_IN,        KC_W,          COMBO_END};
// Number combos: Space (T_L_IN) + left-side key
const uint16_t PROGMEM combo_num_6[]     = {T_L_IN,        KC_N,          COMBO_END};
const uint16_t PROGMEM combo_num_4[]     = {T_L_IN,        KC_R,          COMBO_END};
const uint16_t PROGMEM combo_num_2[]     = {T_L_IN,        KC_T,          COMBO_END};
const uint16_t PROGMEM combo_num_0[]     = {T_L_IN,        KC_S,          COMBO_END};
const uint16_t PROGMEM combo_num_8[]     = {T_L_IN,        KC_W,          COMBO_END};
// Number combos: Enter (T_R_IN) + right-side key
const uint16_t PROGMEM combo_num_1[]     = {T_R_IN,        KC_H,          COMBO_END};
const uint16_t PROGMEM combo_num_3[]     = {T_R_IN,        KC_A,          COMBO_END};
const uint16_t PROGMEM combo_num_5[]     = {T_R_IN,        KC_I,          COMBO_END};
const uint16_t PROGMEM combo_num_7[]     = {T_R_IN,        KC_BSPC,      COMBO_END};
const uint16_t PROGMEM combo_num_9[]     = {T_R_IN,        KC_F,          COMBO_END};
// Shortcut combos: Bspc + left-side key
const uint16_t PROGMEM combo_bspc_b[]   = {KC_BSPC,       KC_B,          COMBO_END};
const uint16_t PROGMEM combo_bspc_l[]   = {KC_BSPC,       KC_L,          COMBO_END};
const uint16_t PROGMEM combo_bspc_d[]   = {KC_BSPC,       KC_D,          COMBO_END};
const uint16_t PROGMEM combo_bspc_c[]   = {KC_BSPC,       KC_C,          COMBO_END};
const uint16_t PROGMEM combo_bspc_n[]   = {KC_BSPC,       KC_N,          COMBO_END};
const uint16_t PROGMEM combo_bspc_r[]   = {KC_BSPC,       KC_R,          COMBO_END};
const uint16_t PROGMEM combo_bspc_t[]   = {KC_BSPC,       KC_T,          COMBO_END};
const uint16_t PROGMEM combo_bspc_s[]   = {KC_BSPC,       KC_S,          COMBO_END};
const uint16_t PROGMEM combo_bspc_x[]   = {KC_BSPC,       KC_X,          COMBO_END};
const uint16_t PROGMEM combo_bspc_q[]   = {KC_BSPC,       KC_Q,          COMBO_END};
const uint16_t PROGMEM combo_bspc_m[]   = {KC_BSPC,       KC_M,          COMBO_END};


combo_t key_combos[] = {
  // Base combos
  COMBO(combo_aa,        SE_AA),
  COMBO(combo_num,       TG(NUM)),
  COMBO(combo_caps,      KC_DEL),
  COMBO(combo_nr_mod,    OSL(MOD)),
  COMBO(combo_ts_sft,    OSM(MOD_LSFT)),
  COMBO(combo_ha_sft,    OSM(MOD_LSFT)),
  COMBO(combo_rt_ctl,    OSM(MOD_LCTL)),
  COMBO(combo_ai_ctl,    OSM(MOD_LCTL)),
  COMBO(combo_tab,       KC_TAB),
  COMBO(combo_esc,       KC_ESC),
  COMBO(combo_ent,       KC_ENT),
  COMBO(combo_del,       CW_TOGG),
  // Deactivation combos
  COMBO(combo_num_deac,  TG(NUM)),
  // Symbol combos via Space + right-side key
  COMBO(combo_sym_circ,  SE_CIRC),
  COMBO(combo_sym_hash,  SE_EQL),
  COMBO(combo_sym_at,    SE_AT),
  COMBO(combo_sym_dquo,  SE_DQUO),
  COMBO(combo_sym_perc,  KC_HASH),
  COMBO(combo_sym_k,     SE_PERC),
  COMBO(combo_sym_scln,  SE_COLN),
  COMBO(combo_sym_sft,   SE_SCLN),
  COMBO(combo_sym_lbrc,  SE_LBRC),
  COMBO(combo_sym_rbrc,  SE_RBRC),
  COMBO(combo_sym_ampr,  SE_AMPR),
  COMBO(combo_sym_lprn,  SE_LPRN),
  COMBO(combo_sym_rprn,  SE_RPRN),
  COMBO(combo_sym_slsh,  KC_EXLM),
  COMBO(combo_sym_eql,   SE_APOS),
  COMBO(combo_3_acut,    SE_ACUT),
  COMBO(combo_3_grv,     SE_GRV),
  // Symbol combos via Enter + left-side key
  COMBO(combo_sym_dlr,   SE_DLR),
  COMBO(combo_sym_plus,  SE_PLUS),
  COMBO(combo_sym_astr,  SE_ASTR),
  COMBO(combo_sym_exlm,  SE_MINS),
  COMBO(combo_sym_tild,  SE_TILD),
  COMBO(combo_sym_ques,  SE_SLSH),
  COMBO(combo_sym_lcbr,  U_SE_LCBR),
  COMBO(combo_sym_rcbr,  U_SE_RCBR),
  COMBO(combo_sym_mins,  SE_UNDS),
  COMBO(combo_sym_bsls,  U_SE_BSLS),
  COMBO(combo_sym_apos,  SE_QUES),
  COMBO(combo_sym_less,  U_SE_LESS),
  COMBO(combo_sym_grtr,  U_SE_GRTR),
  COMBO(combo_sym_pipe,  U_SE_PIPE),
  // Number combos via Space + left-side key
  COMBO(combo_num_6,     KC_6),
  COMBO(combo_num_4,     KC_4),
  COMBO(combo_num_2,     KC_2),
  COMBO(combo_num_0,     KC_0),
  COMBO(combo_num_8,     KC_8),
  // Number combos via Enter + right-side key
  COMBO(combo_num_1,     KC_1),
  COMBO(combo_num_3,     KC_3),
  COMBO(combo_num_5,     KC_5),
  COMBO(combo_num_7,     KC_7),
  COMBO(combo_num_9,     KC_9),
  // Shortcut combos via Bspc + left-side key
  COMBO(combo_bspc_b,    U_FIND_PREV),
  COMBO(combo_bspc_l,    U_FIND_NEXT),
  COMBO(combo_bspc_d,    U_SEARCH),
  COMBO(combo_bspc_c,    U_REPLACE),
  COMBO(combo_bspc_n,    U_SAVE),
  COMBO(combo_bspc_r,    U_CUT),
  COMBO(combo_bspc_t,    U_COPY),
  COMBO(combo_bspc_s,    U_PASTE),
  COMBO(combo_bspc_x,    U_UNDO),
  COMBO(combo_bspc_q,    U_REDO),
  COMBO(combo_bspc_m,    U_MARK_ALL),
};

/* ######### KEYMAPS ######### */

// LED index layout per side (26 LEDs each):
//   0- 5: top row (disabled, outer switches removed)
//   6-11: row 2   (outer=6, keys=7-11)
//  12-17: home row (outer=12, keys=13-17)
//  18-23: bottom row (outer=18, keys=19-23)
//  24-25: thumbs (L: outer=24, inner=25 | R: inner=50, outer=51)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [ALPHA] = LAYOUT_voyager(
    _DEAD_, _DEAD_, _DEAD_, _DEAD_, _DEAD_,  _DEAD_,   /*|*/   _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,   _DEAD_,   _DEAD_,
    _DEAD_, KC_B,   KC_L,   KC_D,   KC_C,    KC_V,     /*|*/   KC_J,   KC_Y,    KC_O,    KC_U,     KC_DOT,   _DEAD_,
    _DEAD_, KC_N,   KC_R,   KC_T,   KC_S,    KC_G,     /*|*/   KC_P,   KC_H,    KC_A,    KC_I,     KC_BSPC, _DEAD_,
    _DEAD_, KC_X,   KC_Q,   KC_M,   KC_W,    KC_Z,     /*|*/   KC_K,   KC_F,    SE_ADIA, SE_OSLH,  KC_COMMA, _DEAD_,
                                    T_L_OUT, T_L_IN,   /*|*/   T_R_IN, T_R_OUT
  ),

  [NUM] = LAYOUT_voyager(
    _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,      /*|*/   _DEAD_,       _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,       /*|*/   _OFF_,        _OFF_,   _OFF_,   _OFF_,   _OFF_,   _DEAD_,
    _DEAD_, KC_6,    KC_4,    KC_2,    KC_0,    _OFF_,       /*|*/   _OFF_,        KC_1,    KC_3,    KC_5,    KC_7,    _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   KC_8,    _OFF_,       /*|*/   _OFF_,        KC_9,    _OFF_,   _OFF_,   _OFF_,   _DEAD_,
                                       _OFF_,   U_NUM_SPACE, /*|*/   U_NUM_TGL,    _OFF_
  ),

  [FUNC] = LAYOUT_voyager(
    _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,        _DEAD_,  /*|*/   _DEAD_,  _DEAD_,       _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,         _OFF_,   /*|*/   _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,   _DEAD_,
    _DEAD_, KC_F6,   KC_F4,   KC_F2,   _OFF_,         _OFF_,   /*|*/   _OFF_,   KC_F1,        KC_F3,   KC_F5,   KC_F7,   _DEAD_,
    _DEAD_, _OFF_,   KC_F12,  KC_F10,  KC_F8,         _OFF_,   /*|*/   _OFF_,   KC_F9,        KC_F11,  _OFF_,   _OFF_,   _DEAD_,
                                       _OFF_,         _OFF_,   /*|*/   _OFF_,   _OFF_
  ),

  [NAV] = LAYOUT_voyager(
    _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  /*|*/   _DEAD_,    _DEAD_,      _DEAD_,        _DEAD_,      _DEAD_,       _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,   /*|*/   _OFF_,     U_DOC_LEFT,  U_DOC_DOWN,    U_DOC_UP,    U_DOC_RIGHT,  _DEAD_,
    _DEAD_, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, SELWORD, /*|*/   _OFF_,     KC_LEFT,     KC_DOWN,       KC_UP,       KC_RIGHT,     _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   SELLINE, SELWBAK, /*|*/   _OFF_,     U_WORD_LEFT, U_5_ROWS_DOWN, U_5_ROWS_UP, U_WORD_RIGHT, _DEAD_,
                                       _OFF_,   _OFF_,   /*|*/   MO(MOUSE), _OFF_
  ),

  [MOUSE] = LAYOUT_voyager(
    _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  /*|*/   _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,   _DEAD_,  _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,   /*|*/   _OFF_,   MS_BTN1, MS_UP,   MS_BTN2,  _OFF_,   _DEAD_,
    _DEAD_, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _OFF_,   /*|*/   _OFF_,   MS_LEFT, MS_DOWN, MS_RGHT,  _OFF_,   _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,   /*|*/   _OFF_,   MS_WHLD, MS_BTN3, MS_WHLU,  _OFF_,   _DEAD_,
                                       _OFF_,   _OFF_,   /*|*/   _OFF_,   _OFF_
  ),

  [SYS] = LAYOUT_voyager(
    _DEAD_, _DEAD_,       _DEAD_,  _DEAD_,  _DEAD_,    _DEAD_,/*|*/   _DEAD_,  _DEAD_,        _DEAD_,       _DEAD_,   _DEAD_,       _DEAD_,
    _DEAD_, _OFF_,        RM_VALD, RM_VALU, U_RGB_TOG, _OFF_, /*|*/   _OFF_,   _OFF_,         _OFF_,        _OFF_,    _OFF_,        _DEAD_,
    _DEAD_, _OFF_,        KC_VOLD, KC_VOLU, KC_MUTE,   _OFF_, /*|*/   _OFF_,   U_OS_SEARCH,   U_SCREENSHOT, U_EMOJIS, _OFF_,        _DEAD_,
    _DEAD_, _OFF_,        KC_MPRV, KC_MNXT, KC_MPLY,   _OFF_, /*|*/   _OFF_,   U_LOCK_SCREEN, U_TOGGLE_OS,  KC_CAPS,  _OFF_,        _DEAD_,
                                            _OFF_,     _OFF_, /*|*/   _OFF_,   _OFF_
  ),

  [MOD] = LAYOUT_voyager(
    _DEAD_, _DEAD_,        _DEAD_,        _DEAD_,        _DEAD_,        _DEAD_,  /*|*/   _DEAD_, _DEAD_,        _DEAD_,        _DEAD_,        _DEAD_,        _DEAD_,
    _DEAD_, _OFF_,         _OFF_,         _OFF_,         _OFF_,         _OFF_,   /*|*/   _OFF_,  _OFF_,         _OFF_,         _OFF_,         _OFF_,         _DEAD_,
    _DEAD_, _OFF_,         _OFF_,         _OFF_,         _OFF_,         _OFF_,   /*|*/   _OFF_,  OSM(MOD_RSFT), OSM(MOD_RCTL), OSM(MOD_RALT), OSM(MOD_RGUI), _DEAD_,
    _DEAD_, _OFF_,         _OFF_,         _OFF_,         _OFF_,         _OFF_,   /*|*/   _OFF_,  _OFF_,         _OFF_,         _OFF_,         _OFF_,         _DEAD_,
                                                          _OFF_,         _OFF_,   /*|*/   _OFF_,  _OFF_
  )
};

/* ######### LEDMAPS ######### */

#define C_OFF  {0,   0,   0  }
#define C_ORG  {20,  255, 200}  // orange       – top row keys
#define C_DGR  {0,   0,   60 }  // dark grey    – os indicator
#define C_PRP  {190, 160, 255}  // purple       – outer column accent
#define C_WHT  {0,   0,   200}  // white        – home + bottom row keys
#define C_RED  {0,   245, 200}  // red          – thumb keys
#define C_CW   {85,  255, 200}  // green        – caps word active
#define C_THB  {10,  245, 232}  // cyan         – thumb keys (other layers)
#define C_MOD  {20,  230, 210}  // teal-orange  – one-shot modifiers
#define C_SYM  {190, 160, 255}  // purple       – symbols
#define C_NUM  {120, 223, 209}  // teal         – numbers
#define C_FN   {30,  241, 180}  // yellow       – function keys
#define C_ACT  {0,   215, 255}  // red          – special actions (enter/cancel/toggle)
// Edit layer
#define C_NEDT {220, 238, 216}  // light green  – edit keys (save/cut/copy/paste)
#define C_NUND {100, 255, 255}  // yellow-green – undo/redo
#define C_NFND {46,  248, 241}  // blue         – search
#define C_NSEL {0,   30,  200}  // silver       – selection/mark
// NAV layer
#define C_NDOC {170, 150, 230}  // teal         – document navigation
#define C_NARW {83,  234, 140}  // green        – arrow keys
#define C_NWRD {230, 140, 245}  // pink         – word/fast navigation
#define C_NTHB {0,   215, 255}  // red          – nav thumb
// MOUSE layer
#define C_MBTN {137, 241, 207}  // green        – mouse buttons
#define C_MMOV {71,  232, 162}  // dark green   – mouse movement
#define C_MWHL {0,   245, 200}  // red          – mouse wheel
// SYS layer
#define C_SVOL {150, 150, 220}  // light blue   – volume controls
#define C_SMDA {40,  150, 200}  // yellow       – media transport
#define C_SSYS {70,  218, 255}  // blue         – system actions (screenshot/search/emojis)
#define C_SOGT {120,  170, 200} // mint         – OS toggle
#define C_SRGB {30,  239, 216}  // teal         – RGB brightness/toggle

// LED indices for dynamic overrides
#define LED_KC_Y 33
#define LED_KC_D 9

const HSV PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (dead)
    C_OFF, C_PRP, C_ORG, C_ORG, C_ORG, C_ORG,       // B(outer) L D C V
    C_OFF, C_PRP, C_WHT, C_WHT, C_WHT, C_WHT,       // N(outer) R T S G
    C_OFF, C_PRP, C_WHT, C_WHT, C_WHT, C_WHT,       // X(outer) Q M W Z
    C_ACT, C_RED,                                   // T_L_OUT, T_L_IN
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (dead)
    C_ORG, C_ORG, C_ORG, C_ORG, C_PRP, C_OFF,       // J Y O U DOT(outer)
    C_WHT, C_WHT, C_WHT, C_WHT, C_WHT, C_OFF,       // P H A I BSPC
    C_WHT, C_WHT, C_WHT, C_WHT, C_PRP, C_OFF,       // K F ADIA OSLH COMMA(outer)
    C_RED, C_ACT                                    // T_R_IN, T_R_OUT
  },

  [NUM] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_NUM, C_NUM, C_NUM, C_NUM, C_OFF,       // 6 4 2 0
    C_OFF, C_OFF, C_OFF, C_OFF, C_NUM, C_OFF,       // 8
    C_OFF, C_ACT,                                   // U_NUM_SPACE
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_NUM, C_NUM, C_NUM, C_NUM, C_OFF,       // 1 3 5 7
    C_OFF, C_NUM, C_OFF, C_OFF, C_OFF, C_OFF,       // 9
    C_ACT, C_OFF                                    // U_NUM_TGL, T_R_OUT
  },

  [FUNC] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (V = deac, unlit)
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_FN,  C_FN,  C_FN,  C_OFF, C_OFF,       // F6 F4 F2
    C_OFF, C_OFF, C_FN,  C_FN,  C_FN,  C_OFF,       // F8
    C_OFF, C_OFF,                                   // thumbs
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (J = deac, unlit)
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_FN,  C_FN,  C_FN,  C_FN,  C_OFF,       // F1 F3 F5 F7
    C_OFF, C_FN,  C_FN,  C_OFF, C_OFF, C_OFF,       // F9
    C_OFF, C_OFF                                    // thumbs
  },

  [NAV] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_MOD,  C_MOD,  C_MOD,  C_MOD,  C_NSEL,  // GUI ALT CTL SFT SELWORD
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_NSEL, C_NSEL,  // SELLINE SELWBAK
    C_OFF, C_OFF,                                   // T_L_OUT, T_L_IN (active)
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_NDOC, C_NDOC, C_NDOC, C_NDOC, C_OFF,   // DOC_LEFT DOWN UP RIGHT
    C_OFF, C_NARW, C_NARW, C_NARW, C_NARW, C_OFF,   // LEFT DOWN UP RIGHT
    C_OFF, C_NWRD, C_NWRD, C_NWRD, C_NWRD, C_OFF,   // WORD_LEFT 5DOWN 5UP WORD_RIGHT
    C_NTHB, C_OFF                                   // MO(MOUSE), _OFF_
  },

  [MOUSE] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_MOD,  C_MOD,  C_MOD,  C_MOD,  C_OFF,   // GUI ALT CTL SFT
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_OFF,                                    // thumbs
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_MBTN, C_MMOV, C_MBTN, C_OFF,  C_OFF,   // BTN1 UP BTN2
    C_OFF, C_MMOV, C_MMOV, C_MMOV, C_OFF,  C_OFF,   // LEFT DOWN RIGHT
    C_OFF, C_MWHL, C_MBTN, C_MWHL, C_OFF,  C_OFF,   // WHLD BTN3 WHLU
    C_OFF, C_OFF                                    // thumbs
  },

  [SYS] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_SRGB, C_SRGB, C_SRGB, C_OFF,   // RM_VALD RM_VALU U_RGB_TOG
    C_OFF, C_OFF,  C_SVOL, C_SVOL, C_SVOL, C_OFF,   // VOLD VOLU MUTE
    C_OFF, C_OFF,  C_SMDA, C_SMDA, C_SMDA, C_OFF,   // MPRV MNXT MPLY
    C_OFF, C_OFF,                                    // thumbs
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_SSYS, C_SSYS, C_SSYS, C_OFF,  C_OFF,   // OS_SEARCH SCREENSHOT EMOJIS
    C_OFF, C_SOGT, C_SOGT, C_SOGT, C_OFF,  C_OFF,   // LOCK_SCREEN TOGGLE_OS CAPS
    C_OFF, C_OFF                                    // thumbs
  },

  [MOD] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // home row (cleared)
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_OFF,                                   // _OFF_, T_L_IN
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_MOD, C_MOD, C_MOD, C_MOD, C_OFF,       // OSM: RSFT RCTL RALT RGUI
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_OFF                                    // thumbs
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

void set_leds_for_layer(uint8_t layer) {
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = pgm_read_hsv(&ledmap[layer][i]);
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
  }
}

void apply_os_indicators(void) {
  HSV win = C_DGR;
  HSV wht = C_WHT;
  HSV y_color = (current_os == OS_WINDOWS) ? win : wht;
  HSV d_color = (current_os == OS_MAC)     ? win : wht;
  RGB y_rgb = hsv_to_rgb_with_value(y_color);
  RGB d_rgb = hsv_to_rgb_with_value(d_color);
  rgb_matrix_set_color(LED_KC_Y, y_rgb.r, y_rgb.g, y_rgb.b);
  rgb_matrix_set_color(LED_KC_D, d_rgb.r, d_rgb.g, d_rgb.b);
}

void apply_os_animation(void) {
  if (!animation_os_timer) return;
  if (timer_elapsed(animation_os_timer) > 3000) {
    animation_os_timer = 0;
    return;
  }
  uint8_t led = (current_os == OS_WINDOWS) ? LED_KC_Y : LED_KC_D;
  uint8_t v = hsv_value_for_os_animation(led, 200);
  RGB rgb = hsv_to_rgb_with_value((HSV){190, 200, v});
  rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
}

void apply_caps_word_animation(void) {
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = pgm_read_hsv(&ledmap[ALPHA][i]);
    if (hsv.v == 0) {
      rgb_matrix_set_color(i, 0, 0, 0);
    } else {
      hsv.h = 0;
      hsv.s = 245;
      hsv.v = hsv_value_for_os_animation(i, hsv.v);
      RGB rgb = hsv_to_rgb_with_value(hsv);
      rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
  }
}

bool rgb_matrix_indicators_user(void) {
  uint8_t active_layer = biton32(layer_state);
  if (active_layer == ALPHA && caps_word_active) {
    apply_caps_word_animation();
  } else {
    set_leds_for_layer(active_layer);
  }
  if (active_layer == ALPHA) {
    apply_os_indicators();
    apply_os_animation();
  }
  return true;
}

void caps_word_set_user(bool active) {
  caps_word_active = active;
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
    case RGB_SLD:        rgblight_mode(1);                                                      break;
    case U_RGB_TOG:      rgb_matrix_toggle();                                                   return false;
    case U_TOGGLE_OS:    flip_os();                                                             return false;
    case U_SCREENSHOT:   PERFORM_BY_OS(tap_code16(G(S(KC_S))),   tap_code16(G(S(KC_4))));       break;
    case U_LOCK_SCREEN:  PERFORM_BY_OS(tap_code16(G(KC_L)),      tap_code16(C(G(KC_Q))));       break;
    case U_OS_SEARCH:    PERFORM_BY_OS(tap_code16(G(KC_S)),      tap_code16(G(KC_SPACE)));      break;
    case U_EMOJIS:       PERFORM_BY_OS(tap_code16(G(KC_DOT)),    tap_code16(C(G(KC_SPACE))));   break;
    case U_NUM_ENTER:    tap_code(KC_ENT);   layer_off(NUM);                                    return false;
    case U_NUM_SPACE:    tap_code(KC_SPC);   layer_off(NUM);                                    return false;
    case U_SE_LESS:      PERFORM_BY_OS(tap_code16(SE_LESS_WIN),  tap_code16(SE_LESS_MAC));      break;
    case U_SE_GRTR:      PERFORM_BY_OS(tap_code16(SE_GRTR_WIN),  tap_code16(SE_GRTR_MAC));      break;
    case U_SE_PIPE:      PERFORM_BY_OS(tap_code16(SE_PIPE_WIN),  tap_code16(SE_PIPE_MAC));      break;
    case U_SE_LCBR:      PERFORM_BY_OS(tap_code16(SE_LCBR_WIN),  tap_code16(SE_LCBR_MAC));      break;
    case U_SE_RCBR:      PERFORM_BY_OS(tap_code16(SE_RCBR_WIN),  tap_code16(SE_RCBR_MAC));      break;
    case U_SE_BSLS:      PERFORM_BY_OS(tap_code16(SE_BSLS_WIN),  tap_code16(SE_BSLS_MAC));      break;
    case U_SEARCH:       PERFORM_BY_OS(tap_code16(C(KC_F)),      tap_code16(G(KC_F)));          break;
    case U_CUT:          PERFORM_BY_OS(tap_code16(C(KC_X)),      tap_code16(G(KC_X)));          break;
    case U_COPY:         PERFORM_BY_OS(tap_code16(C(KC_C)),      tap_code16(G(KC_C)));          break;
    case U_PASTE:        PERFORM_BY_OS(tap_code16(C(KC_V)),      tap_code16(G(KC_V)));          break;
    case U_UNDO:         PERFORM_BY_OS(undo_win(true),           undo_mac(true));               break;
    case U_REDO:         PERFORM_BY_OS(redo_win(true),           redo_mac(true));               break;
    case U_SAVE:         PERFORM_BY_OS(tap_code16(C(KC_S)),      tap_code16(G(KC_S)));          break;
    case U_MARK_ALL:     PERFORM_BY_OS(tap_code16(C(KC_A)),      tap_code16(G(KC_A)));          break;
    case U_DOC_LEFT:     PERFORM_BY_OS(tap_code(KC_HOME),        tap_code16(G(KC_LEFT)));       break;
    case U_DOC_DOWN:     PERFORM_BY_OS(tap_code16(C(KC_END)),    tap_code16(G(KC_DOWN)));       break;
    case U_DOC_UP:       PERFORM_BY_OS(tap_code16(C(KC_HOME)),   tap_code16(G(KC_UP)));         break;
    case U_DOC_RIGHT:    PERFORM_BY_OS(tap_code(KC_END),         tap_code16(G(KC_RIGHT)));      break;
    case U_WORD_LEFT:    PERFORM_BY_OS(word_left_win(true),      word_left_mac(true));          break;
    case U_WORD_RIGHT:   PERFORM_BY_OS(word_right_win(true),     word_right_mac(true));         break;
    case U_5_ROWS_DOWN:  five_rows_down(true);                                                  return false;
    case U_5_ROWS_UP:    five_rows_up(true);                                                    return false;
    case U_FIND_PREV:    PERFORM_BY_OS(tap_code16(S(KC_F3)),     tap_code16(G(S(KC_G))));       break;
    case U_FIND_NEXT:    PERFORM_BY_OS(tap_code16(C(KC_G)),      tap_code16(G(KC_G)));          break;
    case U_REPLACE:      PERFORM_BY_OS(tap_code16(C(KC_H)),      tap_code16(A(G(KC_F))));       break;

    case U_NUM_TGL:                                                                             return false;
  }
  return true;
}

bool process_non_pressed_keycode(uint16_t keycode) {
  switch (keycode) {
    case U_5_ROWS_DOWN:  five_rows_down(false);                                                 break;
    case U_5_ROWS_UP:    five_rows_up(false);                                                   break;
    case U_UNDO:         PERFORM_BY_OS(undo_win(false),       undo_mac(false));                 break;
    case U_REDO:         PERFORM_BY_OS(redo_win(false),       redo_mac(false));                 break;
    case U_WORD_LEFT:    PERFORM_BY_OS(word_left_win(false),  word_left_mac(false));            break;
    case U_WORD_RIGHT:   PERFORM_BY_OS(word_right_win(false), word_right_mac(false));           break;
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
