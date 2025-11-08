#pragma once

/* ######### TAPPING ######### */
#undef TAPPING_TERM
#define TAPPING_TERM 200
#define TAPPING_TERM_THUMBS 330
#define PERMISSIVE_HOLD
  
/* ######### MOUSEKEYS ######### */ 
#undef MOUSEKEY_WHEEL_INTERVAL  
#define MOUSEKEY_WHEEL_INTERVAL 120

#undef MOUSEKEY_WHEEL_MAX_SPEED
#define MOUSEKEY_WHEEL_MAX_SPEED 6

/* ######### CAPS ######### */
#define CAPS_LOCK_STATUS

/* ######### LAYER STATE ######### */
#define LAYER_STATE_16BIT // handle more layers     

/* ### WORD AND LINE SELECTION ### */
#define SELECT_WORD_OS_DYNAMIC
#define SELECT_WORD_TIMEOUT 2000 

/* ######### USB ######### */
#define USB_SUSPEND_WAKEUP_DELAY 0

/* ######### FAST CURSOR ######### */
#define FAST_CURSOR_INTERVAL 100    
#define FAST_CURSOR_INITIAL_DELAY 500 