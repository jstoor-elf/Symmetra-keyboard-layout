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

#define T_L_OUT OSL(SHORTCUT)
#define T_L_IN  LT(NAV, KC_SPC)
#define T_R_IN  QK_REP
#define T_R_OUT OSL(SYS)

/* ######### ENUMS ######### */

enum layers {
  ALPHA,
  NUM,
  FUNC,
  NAV,
  MOUSE,
  SYS,
  SHORTCUT
};

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  U_SE_LESS,
  U_SE_GRTR,
  U_SE_PIPE,
  U_SE_LCBR,
  U_SE_RCBR,
  U_SE_BSLS,
  U_SE_BRC_PAIR,
  U_SE_PRN_PAIR,
  U_SE_CBR_PAIR,
  U_SE_ABK_PAIR,
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
  U_NUM_ENT_ANC,
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
const uint16_t PROGMEM combo_bspc[]      = {KC_O,        KC_U,          COMBO_END};
const uint16_t PROGMEM combo_del[]       = {KC_L,          KC_D,          COMBO_END};
const uint16_t PROGMEM combo_tab[]       = {KC_M,          KC_W,          COMBO_END};
const uint16_t PROGMEM combo_esc[]       = {KC_E,          KC_I,          COMBO_END};
const uint16_t PROGMEM combo_ent[]       = {KC_F,          SE_ADIA,       COMBO_END};
const uint16_t PROGMEM combo_caps_word[] = {KC_T,          KC_A,          COMBO_END};
const uint16_t PROGMEM combo_apos[]      = {KC_R,          KC_S,          COMBO_END};
const uint16_t PROGMEM combo_dquo[]      = {KC_H,          KC_E,          COMBO_END};
const uint16_t PROGMEM combo_dlr[]       = {KC_Q,          KC_M,          COMBO_END};
const uint16_t PROGMEM combo_eql[]       = {T_L_IN,        KC_Y,          COMBO_END};
// Dead-key accents — one per hand, both gapped pairs on the bottom row.
const uint16_t PROGMEM combo_grv[]       = {KC_Q,          KC_W,          COMBO_END};
const uint16_t PROGMEM combo_acut[]      = {KC_F,          SE_OSLH,       COMBO_END};
// FUNC is reached by pressing both outer thumbs together.
const uint16_t PROGMEM combo_func[]      = {T_L_OUT,       T_R_OUT,       COMBO_END};
// One-shot mods. Ctrl/Shift keep a per-hand pair each (home row); Alt and Gui are a
// single gapped combo apiece.
const uint16_t PROGMEM combo_osm_lctl[]  = {KC_R,          KC_T,          COMBO_END};
const uint16_t PROGMEM combo_osm_rctl[]  = {KC_A,          KC_E,          COMBO_END};
const uint16_t PROGMEM combo_osm_lsft[]  = {KC_T,          KC_S,          COMBO_END};
const uint16_t PROGMEM combo_osm_rsft[]  = {KC_H,          KC_A,          COMBO_END};
// Alt and Gui are single gapped combos — Alt on the left hand, Gui on the right.
const uint16_t PROGMEM combo_osm_alt[]   = {KC_L,          KC_C,          COMBO_END};
const uint16_t PROGMEM combo_osm_gui[]   = {KC_Y,          KC_U,          COMBO_END};
// Deactivation combos
const uint16_t PROGMEM combo_num_ent[]   = {KC_9,          U_NUM_ENT_ANC, COMBO_END};
const uint16_t PROGMEM combo_num_deac[]  = {U_NUM_SPACE,   U_NUM_TGL,     COMBO_END};
// Symbol combos: Space (T_L_IN) + right-side key
const uint16_t PROGMEM combo_sym_circ[]  = {T_L_IN,        KC_J,          COMBO_END};
const uint16_t PROGMEM combo_sym_at[]    = {T_L_IN,        KC_O,        COMBO_END};
const uint16_t PROGMEM combo_sym_perc[]  = {T_L_IN,        KC_P,          COMBO_END};
const uint16_t PROGMEM combo_sym_scln[]  = {T_L_IN,        KC_H,          COMBO_END};
const uint16_t PROGMEM combo_sym_sft[]   = {T_L_IN,        KC_I,          COMBO_END};
const uint16_t PROGMEM combo_sym_lbrc[]  = {T_L_IN,        KC_A,          COMBO_END};
const uint16_t PROGMEM combo_sym_rbrc[]  = {T_L_IN,        KC_E,          COMBO_END};
const uint16_t PROGMEM combo_sym_ampr[]  = {T_L_IN,        KC_F,          COMBO_END};
const uint16_t PROGMEM combo_sym_lprn[]  = {T_L_IN,        SE_ADIA,       COMBO_END};
const uint16_t PROGMEM combo_sym_rprn[]  = {T_L_IN,        SE_OSLH,       COMBO_END};
const uint16_t PROGMEM combo_sym_slsh[]  = {T_L_IN,        KC_DOT,        COMBO_END};
const uint16_t PROGMEM combo_sym_hash[]  = {T_L_IN,        KC_U,          COMBO_END};
// Bracket-pair combos: thumb + both bracket keys -> emit open and close
const uint16_t PROGMEM combo_brc_pair[] = {T_L_IN,        KC_A,          KC_E,     COMBO_END};
const uint16_t PROGMEM combo_prn_pair[] = {T_L_IN,        SE_ADIA,       SE_OSLH,  COMBO_END};
const uint16_t PROGMEM combo_cbr_pair[] = {T_R_IN,        KC_R,          KC_T,     COMBO_END};
const uint16_t PROGMEM combo_abk_pair[] = {T_R_IN,        KC_Q,          KC_M,     COMBO_END};
// Symbol combos: Enter (T_R_IN) + left-side key
const uint16_t PROGMEM combo_unds[]      = {T_R_IN,        KC_N,          COMBO_END};
const uint16_t PROGMEM combo_sym_plus[]  = {T_R_IN,        KC_L,          COMBO_END};
const uint16_t PROGMEM combo_sym_astr[]  = {T_R_IN,        KC_D,          COMBO_END};
const uint16_t PROGMEM combo_sym_exlm[]  = {T_R_IN,        KC_C,          COMBO_END};
const uint16_t PROGMEM combo_sym_tild[]  = {T_R_IN,        KC_V,          COMBO_END};
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
const uint16_t PROGMEM combo_num_5[]     = {T_R_IN,        KC_E,          COMBO_END};
const uint16_t PROGMEM combo_num_7[]     = {T_R_IN,        KC_I,           COMBO_END};
const uint16_t PROGMEM combo_num_9[]     = {T_R_IN,        KC_F,          COMBO_END};


combo_t key_combos[] = {
  // Base combos
  COMBO(combo_aa,        SE_AA),
  COMBO(combo_num,       TG(NUM)),
  COMBO(combo_bspc,      KC_BSPC),
  COMBO(combo_del,       KC_DEL),
  COMBO(combo_tab,       KC_TAB),
  COMBO(combo_esc,       KC_ESC),
  COMBO(combo_ent,       KC_ENT),
  COMBO(combo_caps_word, CW_TOGG),
  COMBO(combo_apos,      SE_APOS),
  COMBO(combo_dquo,      SE_DQUO),
  COMBO(combo_dlr,       SE_DLR),
  COMBO(combo_eql,       SE_EQL),
  COMBO(combo_grv,       SE_GRV),
  COMBO(combo_acut,      SE_ACUT),
  // Shortcut combos via FUNC thumb + left-side key
  COMBO(combo_func,      OSL(FUNC)),
  // Home-row one-shot mods
  COMBO(combo_osm_lctl,  OSM(MOD_LCTL)),
  COMBO(combo_osm_rctl,  OSM(MOD_RCTL)),
  COMBO(combo_osm_lsft,  OSM(MOD_LSFT)),
  COMBO(combo_osm_rsft,  OSM(MOD_RSFT)),
  COMBO(combo_osm_alt,   OSM(MOD_LALT)),
  COMBO(combo_osm_gui,   OSM(MOD_RGUI)),
  // Deactivation combos
  COMBO(combo_num_ent,   U_NUM_ENTER),
  COMBO(combo_num_deac,  TG(NUM)),
  // Symbol combos via Space + right-side key
  COMBO(combo_sym_circ,  SE_CIRC),
  COMBO(combo_sym_at,    SE_AT),
  COMBO(combo_sym_perc,  KC_PERC),
  COMBO(combo_sym_scln,  SE_SCLN),
  COMBO(combo_sym_sft,   SE_COLN),
  COMBO(combo_sym_lbrc,  SE_LBRC),
  COMBO(combo_sym_rbrc,  SE_RBRC),
  COMBO(combo_sym_ampr,  SE_AMPR),
  COMBO(combo_sym_lprn,  SE_LPRN),
  COMBO(combo_sym_rprn,  SE_RPRN),
  COMBO(combo_sym_slsh,  KC_EXLM),
  COMBO(combo_sym_hash,  KC_HASH),
  COMBO(combo_brc_pair,  U_SE_BRC_PAIR),
  COMBO(combo_prn_pair,  U_SE_PRN_PAIR),
  COMBO(combo_cbr_pair,  U_SE_CBR_PAIR),
  COMBO(combo_abk_pair,  U_SE_ABK_PAIR),
  // Symbol combos via Enter + left-side key
  COMBO(combo_unds,      SE_SLSH),
  COMBO(combo_sym_plus,  SE_PLUS),
  COMBO(combo_sym_astr,  SE_ASTR),
  COMBO(combo_sym_exlm,  SE_MINS),
  COMBO(combo_sym_tild,  SE_TILD),
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
};

// NumWord, CapsWord and the main-board one-shot mods change state rather than emit a
// character, so an accidental fire is disruptive. They get a tighter window than the
// global COMBO_TERM, demanding a more deliberate simultaneous press. Matched on the
// key array rather than combo_index to avoid keeping a parallel enum in sync with
// key_combos[].
uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
  if (combo->keys == combo_num       || combo->keys == combo_caps_word ||
      combo->keys == combo_osm_lctl  || combo->keys == combo_osm_rctl  ||
      combo->keys == combo_osm_lsft  || combo->keys == combo_osm_rsft  ||
      combo->keys == combo_osm_alt   || combo->keys == combo_osm_gui) {
    return COMBO_TERM_STICKY;
  }
  return COMBO_TERM;
}

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
    _DEAD_, KC_B,   KC_L,   KC_D,   KC_C,    KC_V,     /*|*/   KC_J,   KC_Y,    KC_O,  KC_U,     KC_COMMA, _DEAD_,
    _DEAD_, KC_N,   KC_R,   KC_T,   KC_S,    KC_G,     /*|*/   KC_P,   KC_H,    KC_A,    KC_E,     KC_I,       _DEAD_,
    _DEAD_, KC_X,   KC_Q,   KC_M,   KC_W,    KC_Z,     /*|*/   KC_K,   KC_F,    SE_ADIA, SE_OSLH,  KC_DOT, _DEAD_,
                                    T_L_OUT, T_L_IN,   /*|*/   T_R_IN, T_R_OUT
  ),

  [NUM] = LAYOUT_voyager(
    _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,      /*|*/   _DEAD_,       _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,       /*|*/   _OFF_,        _OFF_,   _OFF_,   _OFF_,   _OFF_,   _DEAD_,
    _DEAD_, KC_6,    KC_4,    KC_2,    KC_0,    _OFF_,       /*|*/   _OFF_,        KC_1,    KC_3,    KC_5,    KC_7,    _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   KC_8,    _OFF_,       /*|*/   _OFF_,        KC_9,    U_NUM_ENT_ANC, _OFF_,   _OFF_,   _DEAD_,
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
    _DEAD_, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _OFF_,   /*|*/   _OFF_,     KC_LEFT,     KC_DOWN,       KC_UP,       KC_RIGHT,     _DEAD_,
    _DEAD_, TG(MOUSE), SELWBAK, SELWORD, SELLINE, _OFF_, /*|*/   _OFF_,     U_WORD_LEFT, U_5_ROWS_DOWN, U_5_ROWS_UP, U_WORD_RIGHT, _DEAD_,
                                       _OFF_,   _OFF_,   /*|*/   _OFF_,     _OFF_
  ),

  [MOUSE] = LAYOUT_voyager(
    _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  /*|*/   _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,   _DEAD_,  _DEAD_,
    _DEAD_, _OFF_,   _OFF_,   _OFF_,   _OFF_,   _OFF_,   /*|*/   _OFF_,   _OFF_,   _OFF_,   _OFF_,    _OFF_,   _DEAD_,
    _DEAD_, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _OFF_,   /*|*/   _OFF_,   MS_LEFT, MS_DOWN, MS_UP,    MS_RGHT,   _DEAD_,
    _DEAD_, TG(MOUSE), _OFF_, _OFF_,   _OFF_,   _OFF_,   /*|*/   _OFF_,   _OFF_,   MS_WHLD, MS_WHLU,  _OFF_,   _DEAD_,
                                       _OFF_,   _OFF_,   /*|*/   MS_BTN1, MS_BTN2
  ),

  [SYS] = LAYOUT_voyager(
    _DEAD_, _DEAD_,       _DEAD_,  _DEAD_,  _DEAD_,    _DEAD_,/*|*/   _DEAD_,  _DEAD_,        _DEAD_,       _DEAD_,   _DEAD_,       _DEAD_,
    _DEAD_, _OFF_,        RM_VALD, RM_VALU, U_RGB_TOG, _OFF_, /*|*/   _OFF_,   _OFF_,         _OFF_,        _OFF_,    _OFF_,        _DEAD_,
    _DEAD_, _OFF_,        KC_VOLD, KC_VOLU, KC_MUTE,   _OFF_, /*|*/   _OFF_,   U_OS_SEARCH,   U_SCREENSHOT, U_EMOJIS, _OFF_,        _DEAD_,
    _DEAD_, _OFF_,        KC_MPRV, KC_MNXT, KC_MPLY,   _OFF_, /*|*/   _OFF_,   U_LOCK_SCREEN, U_TOGGLE_OS,  KC_CAPS,  _OFF_,        _DEAD_,
                                            _OFF_,     _OFF_, /*|*/   _OFF_,   _OFF_
  ),

  [SHORTCUT] = LAYOUT_voyager(
    _DEAD_, _DEAD_,      _DEAD_,      _DEAD_,     _DEAD_,    _DEAD_, /*|*/   _DEAD_, _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,  _DEAD_,
    _DEAD_, U_FIND_PREV, U_FIND_NEXT, U_SEARCH,   U_REPLACE, _OFF_,  /*|*/   _OFF_,  _OFF_,   _OFF_,   _OFF_,   _OFF_,   _DEAD_,
    _DEAD_, U_SAVE,      U_CUT,       U_COPY,     U_PASTE,   _OFF_,  /*|*/   _OFF_,  _OFF_,   _OFF_,   _OFF_,   _OFF_,   _DEAD_,
    _DEAD_, U_UNDO,      U_REDO,      U_MARK_ALL, _OFF_,     _OFF_,  /*|*/   _OFF_,  _OFF_,   _OFF_,   _OFF_,   _OFF_,   _DEAD_,
                                                  _OFF_,     _OFF_,  /*|*/   _OFF_,  _OFF_
  )
};

/* ######### LEDMAPS ######### */

// Base palette — used on the base layer
#define C_OFF  {0,   0,   0  }
#define C_ORG  {20,  255, 245}  // orange
#define C_GRN  {83,  245, 180}  // green
#define C_RED  {0,   255, 170}  // red
#define C_SLV  {0,   15,  120}  // silver
// Extra palette — used on non-base layers only
#define C_YLW  {30,  241, 180}  // yellow
#define C_BLU  {148, 220, 230}  // blue
#define C_PRP  {180, 180, 170}  // purple
#define C_TEA  {120, 223, 209}  // teal
#define C_PNK  {234, 240, 255}  // hot pink
#define C_LBL  {150, 150, 220}  // light blue

// LED indices for dynamic overrides
#define LED_TOGGLE_OS 46  // U_TOGGLE_OS key on the SYS layer

const HSV PROGMEM ledmap[][RGB_MATRIX_LED_COUNT] = {

  [ALPHA] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (dead)
    C_OFF, C_GRN, C_ORG, C_ORG, C_ORG, C_ORG,       // B(outer) L D C V
    C_OFF, C_GRN, C_SLV, C_SLV, C_SLV, C_SLV,       // N(outer) R T S G
    C_OFF, C_GRN, C_SLV, C_SLV, C_SLV, C_SLV,       // X(outer) Q M W Z
    C_RED, C_RED,                                   // T_L_OUT, T_L_IN
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (dead)
    C_ORG, C_ORG, C_ORG, C_ORG, C_GRN, C_OFF,       // J Y REPEAT U COMMA(outer)
    C_SLV, C_SLV, C_SLV, C_SLV, C_GRN, C_OFF,       // P H A I O(outer)
    C_SLV, C_SLV, C_SLV, C_SLV, C_GRN, C_OFF,       // K F ADIA OSLH DOT(outer)
    C_RED, C_RED                                    // T_R_IN, T_R_OUT
  },

  [NUM] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_TEA, C_TEA, C_TEA, C_TEA, C_OFF,       // 6 4 2 0
    C_OFF, C_OFF, C_OFF, C_OFF, C_TEA, C_OFF,       // 8
    C_OFF, C_RED,                                   // U_NUM_SPACE
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_TEA, C_TEA, C_TEA, C_TEA, C_OFF,       // 1 3 5 7
    C_OFF, C_TEA, C_OFF, C_OFF, C_OFF, C_OFF,       // 9
    C_RED, C_OFF                                    // U_NUM_TGL, T_R_OUT
  },

  [FUNC] = {
    // Left side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (V = deac, unlit)
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_YLW,  C_YLW,  C_YLW,  C_OFF, C_OFF,       // F6 F4 F2
    C_OFF, C_OFF, C_YLW,  C_YLW,  C_YLW,  C_OFF,       // F8
    C_OFF, C_OFF,                                   // thumbs
    // Right side
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // top row (J = deac, unlit)
    C_OFF, C_OFF, C_OFF, C_OFF, C_OFF, C_OFF,       // all XXXXXXX
    C_OFF, C_YLW,  C_YLW,  C_YLW,  C_YLW,  C_OFF,       // F1 F3 F5 F7
    C_OFF, C_YLW,  C_YLW,  C_OFF, C_OFF, C_OFF,       // F9
    C_OFF, C_OFF                                    // thumbs
  },

  [NAV] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_ORG,  C_ORG,  C_ORG,  C_ORG,  C_OFF,  // GUI ALT CTL SFT
    C_OFF, C_RED,  C_SLV,  C_SLV,  C_SLV,  C_OFF,  // TG(MOUSE) SELWBAK SELWORD SELLINE
    C_OFF, C_OFF,                                   // T_L_OUT, T_L_IN (active)
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_TEA, C_TEA, C_TEA, C_TEA, C_OFF,   // DOC_LEFT DOWN UP RIGHT
    C_OFF, C_GRN, C_GRN, C_GRN, C_GRN, C_OFF,   // LEFT DOWN UP RIGHT
    C_OFF, C_PNK, C_PNK, C_PNK, C_PNK, C_OFF,   // WORD_LEFT 5DOWN 5UP WORD_RIGHT
    C_OFF, C_OFF                                   // _OFF_, _OFF_
  },

  [MOUSE] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_ORG,  C_ORG,  C_ORG,  C_ORG,  C_OFF,   // GUI ALT CTL SFT
    C_OFF, C_RED,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // TG(MOUSE)->NAV
    C_OFF, C_OFF,                                    // thumbs
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_GRN, C_GRN, C_GRN, C_GRN, C_OFF,   // LEFT DOWN UP RIGHT
    C_OFF, C_OFF, C_PNK, C_PNK, C_OFF,  C_OFF,   // WHLD WHLU
    C_BLU, C_BLU                                    // BTN1 BTN2
  },

  [SYS] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_TEA, C_TEA, C_TEA, C_OFF,   // RM_VALD RM_VALU U_RGB_TOG
    C_OFF, C_OFF,  C_PRP, C_PRP, C_PRP, C_OFF,   // VOLD VOLU MUTE
    C_OFF, C_OFF,  C_YLW, C_YLW, C_YLW, C_OFF,   // MPRV MNXT MPLY
    C_OFF, C_OFF,                                    // thumbs
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_SLV, C_SLV, C_SLV, C_OFF,  C_OFF,   // OS_SEARCH SCREENSHOT EMOJIS
    C_OFF, C_GRN, C_GRN, C_GRN, C_OFF,  C_OFF,   // LOCK_SCREEN TOGGLE_OS CAPS
    C_OFF, C_OFF                                    // thumbs
  },

  [SHORTCUT] = {
    // Left side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_BLU,  C_BLU,  C_BLU,  C_BLU,  C_OFF,   // FindPrev FindNext Search Replace
    C_OFF, C_PNK,  C_PNK,  C_PNK,  C_PNK,  C_OFF,   // Save Cut Copy Paste
    C_OFF, C_YLW,  C_YLW,  C_GRN,  C_OFF,  C_OFF,   // Undo Redo MarkAll
    C_OFF, C_OFF,                                   // thumbs
    // Right side
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // top row
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_OFF,  C_OFF,  C_OFF,  C_OFF,  C_OFF,   // all XXXXXXX
    C_OFF, C_OFF                                    // thumbs
  }
};

/* ######### LED CONTROL FUNCTIONS ######### */

uint16_t breathing_value(uint8_t index, uint8_t max_v) {
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

void apply_os_indicator(void) {
  // Continuously breathe the Toggle-OS key: teal for Windows, red for Mac.
  uint8_t h = (current_os == OS_WINDOWS) ? 120 : 0;
  uint8_t s = (current_os == OS_WINDOWS) ? 223 : 255;
  uint8_t v = breathing_value(LED_TOGGLE_OS, 200);
  RGB rgb = hsv_to_rgb_with_value((HSV){h, s, v});
  rgb_matrix_set_color(LED_TOGGLE_OS, rgb.r, rgb.g, rgb.b);
}

void apply_caps_word_animation(void) {
  for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = pgm_read_hsv(&ledmap[ALPHA][i]);
    if (hsv.v == 0) {
      rgb_matrix_set_color(i, 0, 0, 0);
    } else {
      hsv.h = 0;
      hsv.s = 30;
      hsv.v = breathing_value(i, 200);
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
  if (active_layer == SYS) {
    apply_os_indicator();
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
  switch (keycode) {
    case T_L_OUT:
    case T_L_IN:
    case T_R_IN:
    case T_R_OUT:
      return TAPPING_TERM_THUMBS;
    default:
      return TAPPING_TERM;
  }
}

/* ######### LAYER STATE ######### */

// MOUSE is a child of NAV: toggled on with TG(MOUSE) while NAV is held, but it
// can never linger past the NAV hold. The moment NAV drops, MOUSE drops too, so
// releasing the nav thumb always lands you on base regardless of sub-mode.
layer_state_t layer_state_set_user(layer_state_t state) {
  if (!(state & (1UL << NAV))) {
    state &= ~(1UL << MOUSE);
  }
  return state;
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

static void tap_pair(uint16_t open, uint16_t close) {
  tap_code16(open);
  tap_code16(close);
}

static void tap_pair_by_os(uint16_t win_open, uint16_t win_close,
                           uint16_t mac_open, uint16_t mac_close) {
  PERFORM_BY_OS(tap_pair(win_open, win_close), tap_pair(mac_open, mac_close));
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
    case U_SE_BRC_PAIR:  tap_pair(SE_LBRC, SE_RBRC);                                            return false;
    case U_SE_PRN_PAIR:  tap_pair(SE_LPRN, SE_RPRN);                                            return false;
    case U_SE_CBR_PAIR:  tap_pair_by_os(SE_LCBR_WIN, SE_RCBR_WIN, SE_LCBR_MAC, SE_RCBR_MAC);    return false;
    case U_SE_ABK_PAIR:  tap_pair_by_os(SE_LESS_WIN, SE_GRTR_WIN, SE_LESS_MAC, SE_GRTR_MAC);    return false;
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
    case U_NUM_ENT_ANC:                                                                         return false;
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
