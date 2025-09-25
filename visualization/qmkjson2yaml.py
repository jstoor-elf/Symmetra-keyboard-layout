import json
import yaml
import re
import sys

# Mapping from QMK layer indices to readable names
layer_names = {
    0: "ALPHA",
    1: "SYM",
    2: "NAV",
    3: "MOUSE",
    4: "NUM/FUNC",
    5: "SYS"       
}   

# Modifier keys
modifier_keys = {
    "MOD_LCTL": "Ctrl",
    "MOD_RCTL": "Ctrl",    
    "MOD_LSFT": "Shift",
    "MOD_RSFT": "Shift",
    "MOD_LALT": "Alt",
    "MOD_RALT": "Alt",
    "MOD_LGUI": "GUI",
    "MOD_RGUI": "GUI",
}

# Special / one-shot keys
special_keys = {
    "KC_NO": " ",
    "KC_BSPC": "Backspace",
    "OSM(MOD_LSFT)": "Caps Letter",
    "CW_TOGG": "Caps Word",
    "QK_LLCK": "Lock Layer",
}

# Swedish letter keys
swedish_keys = {
    "SE_ADIA": "Ä",
    "SE_OSLH": "Ö",
    "SE_AA": "Å",
}

# Symbol keys
symbol_keys = {
    "SE_QUES": "?",
    "SE_LESS": "<",
    "SE_GRTR": ">",
    "SE_MINS": "-",
    "SE_PIPE": "|",
    "SE_CIRC": "^",
    "SE_LCBR": "{",
    "SE_RCBR": "}",
    "SE_DLR": "$",
    "SE_ACUT": "´",
    "SE_GRV": "`",
    "SE_ASTR": "*",
    "SE_SLSH": "/",
    "SE_EQL": "=",
    "SE_AMPR": "&",
    "SE_LPRN": "(",
    "SE_RPRN": ")",
    "SE_SCLN": ";",
    "SE_DQUO": '"',
    "SE_BSLS": "\\",
    "SE_TILD": "~",
    "SE_PLUS": "+",
    "SE_LBRC": "[",
    "SE_RBRC": "]",
    "SE_AT": "@",
    "SE_UNDS": "_",
    "SE_COLN": ":",
    "SE_APOS": "'",
    "KC_PERC": "%",
    "KC_EXLM": "!",
    "KC_HASH": "#",
    "KC_COMMA": ",",
    "KC_DOT": ".",
}

# Editing keys
editing_keys = { 
    "U_FIND_PREV": "Find ↶",
    "U_FIND_NEXT": "Find ↷",
    "U_MARK_WORD_PREV": "Mark Word ↶",
    "U_MARK_WORD_NEXT": "Mark Word ↷"     
}

# Mouse keys
navigation_keys = {
    "KC_LEFT": "⭠",
    "KC_DOWN": "⭣",
    "KC_UP": "⭡",
    "KC_RIGHT": "⭢",
    "U_DOC_LEFT": "Doc ⭠",
    "U_DOC_DOWN": "Doc ⭣",
    "U_DOC_UP": "Doc ⭡",
    "U_DOC_RIGHT": "Doc ⭢",   
    "U_WORD_LEFT": "Word ⭠",
    "U_PARA_DOWN": "Para ⭣",
    "U_PARA_UP": " Para ⭡",
    "U_WORD_RIGHT": "Word ⭢"
}

# Mouse keys
mouse_keys = {
    "KC_MS_BTN1": "Mouse Click L",
    "KC_MS_BTN2": "Mouse Click R",
    "KC_MS_BTN3": "Mouse Click M",
    "KC_MS_UP": "Mouse ⭡",
    "KC_MS_DOWN": "Mouse ⭣",
    "KC_MS_LEFT": "Mouse ⭠",
    "KC_MS_RIGHT": "Mouse ⭢",    
    "KC_MS_WH_UP": "Mouse Wheel ⭡",
    "KC_MS_WH_DOWN": "Mouse Wheel ⭣",
    "KC_MS_WH_LEFT": "Mouse Wheel ⭠",
    "KC_MS_WH_RIGHT": "Mouse Wheel ⭢",
}

# System keys
systems_keys = {
    "RGB_VAI": "RGB +",
    "RGB_VAD": "RGB -",
    "RGB_TOG": "RGB Mute",    
    "KC_MEDIA_PREV_TRACK": "⏮️", 
    "KC_MEDIA_NEXT_TRACK": "⏭️",
    "KC_MEDIA_PLAY_PAUSE": "⏯️", 
    "KC_MEDIA_STOP": "⏹️",   
    "U_EMOJIS": "😀😎 🤔😭",
    "U_THUMBS_UP_EMOJI": "👍",
    "U_SCREENSHOT": "📸",       
    "U_OS_SEARCH": "🔍",
    "U_TERMINAL": "📟",     
    "U_BROWSER": "🌐",
    "U_LOCK_SCREEN": "🔒",     
    "U_TOGGLE_OS": "🔄 🪟🍏",  
    "U_PREV_WINDOW": "Window ↶",
    "U_NEXT_WINDOW": "Window ↷",
    "U_PREV_APP": "App ↶",
    "U_NEXT_APP": "App ↷",  
    "U_PREV_APP_WINDOW": "App Window ↶",        
    "U_NEXT_APP_WINDOW": "App Window ↷",
    "U_PREV_TAB": "Tab ↶",
    "U_NEXT_TAB": "Tab ↷",    
    "U_NEW_TAB": "Tab ➕",
    "U_CLOSE_TAB": "Tab ➖",
    "KC_AUDIO_VOL_DOWN": "Volume -",
    "KC_AUDIO_VOL_UP": "Volume +",
    "KC_AUDIO_MUTE": "Volume Mute",
}

# Merge all lookup dictionaries
lookup_map = {
    **modifier_keys, 
    **special_keys,
    **swedish_keys,
    **symbol_keys,
    **editing_keys,
    **navigation_keys, 
    **mouse_keys,   
    **systems_keys
}

def map_special_tap(key: str) -> str | None:
    """
    Handles Mod-Tap (MT), Layer-Tap (LT), and Momentary layer (MO) keys.
    Returns None if the key is not one of these types.
    """

    # Mod-Tap: MT(mod, key)
    mt_match = re.match(r"MT\(([^,]+),\s*([^)]+)\)", key)
    if mt_match:
        mod, tap = mt_match.groups()
        return f"{map_key(tap).splitlines()[0]}\n▷{map_key(mod).splitlines()[0]}"

    # Layer-Tap: LT(layer_index, key)
    lt_match = re.match(r"LT\((\d+),\s*([^)]+)\)", key)
    if lt_match:
        layer_index, tap = lt_match.groups()
        return f"{map_key(tap).splitlines()[0]}\n▷{layer_names.get(int(layer_index), f'LAYER{layer_index}').upper()}"

    # Momentary layer: MO(layer_index)
    mo_match = re.match(r"MO\((\d+)\)", key)
    if mo_match:
        layer_index = int(mo_match.group(1))
        return f"▷{layer_names.get(layer_index, f'LAYER{layer_index}')}"
    
    return None

def map_key(key: str) -> str:
    """Map a QMK keycode to a human-readable string."""
    
    # Check special MT / LT / MO keys
    if (special_tap := map_special_tap(key)):
        return special_tap    

    # Lookup in merged dictionary
    if key in lookup_map:
        return lookup_map[key] 
 
    # Prefix-based keys
    for prefix in ("KC_", "U_"):
        if key.startswith(prefix):
            return key.removeprefix(prefix).replace("_", " ").title()

    return key

def map_keys(keys: list[str]) -> list[str]:
    """Map a list of keys using map_key."""

    return [map_key(k) for k in keys]

def map_layer(layer: list[str]) -> list[list[str]]:
    """Convert flat layer list to 5 rows (4 main + 1 thumb)."""

    rows = [map_keys(layer[r*12:(r+1)*12]) for r in range(4)]
    rows.append(map_keys(layer[48:51] + layer[51:54]))
    return rows

def main():
    """Load JSON keymap, convert to readable YAML, and write YAML to stdout."""

    if len(sys.argv) != 2:
        print("Usage: python qmkjson2yaml.py <keymap.json>")
        sys.exit(1)

    with open(sys.argv[1]) as f:
        keymap_json = json.load(f)

    keymap_yaml = {"keyboard": "ZSA Voyager", "layers": {}}

    for i, layer in enumerate(keymap_json["layers"]):
        keymap_yaml["layers"][layer_names.get(i)] = map_layer(layer)

    yaml.dump(keymap_yaml, sys.stdout, sort_keys=False, default_flow_style=False)

if __name__ == "__main__":
    main()