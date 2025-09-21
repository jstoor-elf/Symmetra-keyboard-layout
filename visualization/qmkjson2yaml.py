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

def map_key(key):
    
    # Mod-Tap (MT)
    mt_match = re.match(r"MT\(([^,]+),\s*([^)]+)\)", key)
    if mt_match:
        mod, tap_key = mt_match.groups()
        tap_text = map_key(tap_key).split("\n")[0]
        mod_text = map_key(mod).split("\n")[0]
        return f"{tap_text}\n▷{mod_text}"
    
    # Layer Tap (LT)
    lt_match = re.match(r"LT\((\d+),\s*([^)]+)\)", key)
    if lt_match:
        layer_index, tap_key = lt_match.groups()
        layer_index = int(layer_index)
        tap_text = map_key(tap_key).split("\n")[0]  # main key text
        layer_text = layer_names.get(layer_index, f"LAYER{layer_index}").upper()
        return f"{tap_text}\n▷{layer_text}"


    # Layer toggle / momentary
    mo_match = re.match(r"MO\((\d+)\)", key)
    if mo_match:
        layer_index = int(mo_match.group(1))
        return f"▷{layer_names.get(layer_index, f"LAYER{layer_index}")}"       

    if key.startswith("MOD_"):
        mod_name = key[5:]
        match mod_name:
            case "CTL":
                return "Ctrl"
            case "SFT":
                return "Shift"
            case _:
                return mod_name.title()

    # No keys and transparent keys
    if key == "KC_NO":
        return "∅"
    if key == "KC_TRANSPARENT":
        return "↓"     

    # Caps
    if key == "OSM(MOD_LSFT)":
        return "Caps Letter"    
    if key == "CW_TOGG":
        return "Caps Word"  
    
    # Lock layer
    if key == "QK_LLCK":
        return "Lock Layer"  

    # Swedish letters
    if key == "SE_ADIA":
       return "Ä"  
    if key == "SE_OSLH": 
       return "Ö"  
    if key == "SE_AA":
       return "Å" 
          
    # Symbols
    if key == "SE_QUES":
        return "?"
    if key == "SE_LESS":
        return "<"
    if key == "SE_GRTR":
        return ">"
    if key == "SE_MINS":
        return "-"
    if key == "SE_PIPE":
        return "|"
    if key == "SE_CIRC":
        return "^"
    if key == "SE_LCBR":
        return "{"
    if key == "SE_RCBR":
        return "}"
    if key == "SE_DLR":
        return "$"
    if key == "SE_ACUT":
        return "´"
    if key == "SE_GRV":
        return "`"
    if key == "SE_ASTR":
        return "*"
    if key == "SE_SLSH":
        return "/"
    if key == "SE_EQL":
        return "="
    if key == "SE_AMPR":
        return "&"
    if key == "SE_LPRN":
        return "("
    if key == "SE_RPRN":
        return ")"
    if key == "SE_SCLN":
        return ";"
    if key == "SE_DQUO":
        return '"'
    if key == "SE_BSLS":
        return "\\"
    if key == "SE_TILD":
        return "~"
    if key == "SE_PLUS":
        return "+"
    if key == "SE_LBRC":
        return "["
    if key == "SE_RBRC":
        return "]"
    if key == "SE_AT":
        return "@"
    if key == "SE_UNDS":
        return "_"
    if key == "SE_COLN":
        return ":"
    if key == "SE_APOS":
        return "'"
    if key == "KC_PERC":
        return "%"
    if key == "KC_EXLM":
        return "!"
    if key == "KC_HASH":
        return "#"
    if key == "KC_COMMA":
        return ","
    if key == "KC_DOT":
        return "."   
    
    # Media symbols
    if key.startswith("KC_MEDIA_"):
        return key[9:].replace("_", " ").title()  
     # Media keys
    if key.startswith("KC_AUDIO_"):
        return key[9:].replace("_", " ").title()         
    if key.startswith("KC_BSPC"):
        return "Backspace"
 
    # RGB
    if key == "TOGGLE_LAYER_COLOR":
        return "RGB Dance"
    if key.startswith("RGB_"):
        btn_name = key[4:]
        match btn_name:
            case "VAD":
                return "RGB Down"
            case "VAI":
                return "RGB Up"
            case "TOG":
                return "RGB Mute"                                
            case _:
                return f"{btn_name.title()}"

    # Mouse
    if key.startswith("KC_MS_"):
        if "WH" in key:
            return f"Mouse Wheel {key[9:].title()}"
        btn_name = key[6:]
        match btn_name:
            case "BTN1":
                return "Left Click"
            case "BTN2":
                return "Right Click"
            case "BTN3":
                return "Middle Click"                                
            case _:
                return f"Mouse {btn_name.title()}"
   
    # Qmk defined keys
    if key.startswith("KC_"):
       return key[3:].capitalize()

    # My defined keys       
    if key.startswith("U_"):
       return key[2:].replace("_", " ").title()    

    return key

# Map a list of keys
def map_keys(keys):
    """
    Takes a list of keys and applies map_key to each element.
    Returns a new list with mapped keys.
    """
    return [map_key(k) for k in keys]   

# Read JSON from stdin if no filename is passed
if len(sys.argv) != 2:
    print("Usage: python qmkjson2yaml.py <keymap.json>")
    sys.exit(1)

with open(sys.argv[1]) as f:
    keymap_json = json.load(f)

# Prepare YAML structure
keymap_yaml = {"keyboard": "ZSA Voyager", "layers": {}}

# Reshape each layer
for i, layer in enumerate(keymap_json["layers"]):
    rows = []
    # Main 4 rows of 12 keys each
    for r in range(4):
        rows.append(map_keys(layer[r*12:(r+1)*12]))
       
    # Fifth row: thumb keys, 3 on left and 3 on right
    thumb_row = layer[48:51] + layer[51:54]  # left 3 + right 3
    rows.append(map_keys(thumb_row))

    keymap_yaml["layers"][layer_names.get(i, f"LAYER{i}")] = rows

# Save YAML
yaml.dump(keymap_yaml, sys.stdout, sort_keys=False, default_flow_style=False)