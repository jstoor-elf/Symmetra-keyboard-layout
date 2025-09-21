import json
import yaml
import re
import sys

# Mapping from QMK layer indices to readable names
layer_names = {
    0: "ALPHA",
    1: "SYM",
    2: "NAV",
    3: "SYS",
    4: "NUM",
    5: "MOUSE"
}   

# Modifier keys
modifier_map = {
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
    "KC_NO": "∅",
    "KC_TRANSPARENT": "↓",
    "KC_BSPC": "Backspace",
    "OSM(MOD_LSFT)": "Caps Letter",
    "CW_TOGG": "Caps Word",
    "QK_LLCK": "Lock Layer",
}

# Swedish letter keys
swedish_letters = {
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

# RGB keys
rgb_map = {
    "TOGGLE_LAYER_COLOR": "RGB Dance",
    "RGB_VAD": "RGB Down",
    "RGB_VAI": "RGB Up",
    "RGB_TOG": "RGB Mute",
}

# Audio keys
audio_map = {
    "KC_AUDIO_VOL_DOWN": "Volume Down",
    "KC_AUDIO_VOL_UP": "Volume Up",
    "KC_AUDIO_MUTE": "Mute"
}

# Mouse keys
mouse_map = {
    "KC_MS_BTN1": "Left Click",
    "KC_MS_BTN2": "Right Click",
    "KC_MS_BTN3": "Middle Click",
    "KC_MS_UP": "Mouse Up",
    "KC_MS_DOWN": "Mouse Down",
    "KC_MS_LEFT": "Mouse Left",
    "KC_MS_RIGHT": "Mouse Right",    
    "KC_MS_WH_UP": "Mouse Wheel Up",
    "KC_MS_WH_DOWN": "Mouse Wheel Down",
    "KC_MS_WH_LEFT": "Mouse Wheel Left",
    "KC_MS_WH_RIGHT": "Mouse Wheel Right",
}

# Merge all lookup dictionaries
lookup_map = {
    **modifier_map, 
    **special_keys,
    **swedish_letters,
    **symbol_keys, 
    **rgb_map, 
    **audio_map, 
    **mouse_map
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
        keymap_yaml["layers"][layer_names.get(i, f"LAYER{i}")] = map_layer(layer)

    yaml.dump(keymap_yaml, sys.stdout, sort_keys=False, default_flow_style=False)


if __name__ == "__main__":
    main()