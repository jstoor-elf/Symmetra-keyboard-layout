#pragma once

// Tap-hold configuration for home row mods.
#define TAPPING_TERM 250
#define TAPPING_TERM_PER_KEY
#define FLOW_TAP_TERM 100
#define CHORDAL_HOLD
#define PERMISSIVE_HOLD
#define QUICK_TAP_TERM_PER_KEY

/* ######### USB ######### */
#define PERMISSIVE_HOLD 
  
/* ######### MOUSEKEY ######### */
#ifdef MOUSEKEY_WHEEL_INTERVAL
  #undef MOUSEKEY_WHEEL_INTERVAL
#endif
#define MOUSEKEY_WHEEL_INTERVAL 120

#ifdef MOUSEKEY_WHEEL_MAX_SPEED
  #undef MOUSEKEY_WHEEL_MAX_SPEED
#endif
#define MOUSEKEY_WHEEL_MAX_SPEED 6

/* ######### LAYER / CAPS ######### */
#define CAPS_LOCK_STATUS      
#define LAYER_STATE_16BIT       