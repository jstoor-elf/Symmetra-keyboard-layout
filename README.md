# Symmetra-keyboard-layout

QMK layout for my ZSA Voyager.

## Keywords

_qwerty_, _nordic_, _swedish_, _symmetrical_, _40-keys_, _homerow mods_,  _2-row numpad_, _mouse_, _win-mac_, _OS-toggle_

## Design choices
 
I have deliberately chosen not to use the top row of the Voyager, partly to _stay_ forward-compatible with other 3×6 + thumb key layouts, and partly to reduce finger travel. My mind tends to favor symmetry, so the Mod-Tap and Layer-Tap keys are positioned symmetrically on both sides of the keyboard.

## Key Translations

The following table explains the meaning of symbols and keys in the SVG that might not be immediately obvious:

| Symbol       | Meaning                      |
|--------------|------------------------------|
| ∅            | No key                       |
| ↓            | Transparent / Inheritance    |
| ▷            | Layer toggle or modifier     |
| Caps Letter  | One-Shot Modifiers           |

## Layers

![Keymap](Visualization/Results/keymap.svg)

## Visualization Pipeline

The pipeline converts your QMK `keymap.c` into a visual representation of all layers. First, the official QMK CLI tool `qmk c2json` generates a JSON file from the keymap. A custom Python script (`qmkjson2yaml.py`) then converts this JSON into YAML, adding readable layer names and symbolic representations for special keys. Finally, the external tool `keymap-drawer` reads the YAML and produces an SVG diagram of the keyboard. All generated files are saved in the `Results` folder.  

**Dependencies:**  
- QMK CLI (`qmk c2json`)  
- Python 3  
- `keymap-drawer` Python module  
- Custom Python script: `qmkjson2yaml.py`  