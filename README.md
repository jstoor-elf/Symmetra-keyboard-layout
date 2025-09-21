# Symmetra-keyboard-layout

QMK layout for my ZSA Voyager.

## Keywords

`qwerty`, `nordic`, `swedish`, `symmetrical`, `40-keys`, `homerow mods`, `2-row numpad`, `mouse`, `win-mac`, `OS-toggle`

## Design choices
 
I have deliberately chosen not to use the top row of the Voyager, partly to _stay_ forward-compatible with other 3×6 + thumb key layouts, and partly to reduce finger travel. My mind tends to favor symmetry, so the Mod-Tap and Layer-Tap keys are positioned symmetrically on both sides of the keyboard.

## Key Translations

Legend for symbols and special keys used in the SVG:

| Symbol       | Meaning                      |
|--------------|------------------------------|
| ∅            | No key                       |
| ↓            | Transparent / Inheritance    |
| ▷            | Layer toggle or modifier     |
| Caps Letter  | One-Shot shift modifier      |

## Layers

![Keymap](Visualization/Results/keymap.svg)

## Visualization Pipeline

This pipeline turns your QMK keymap.c into a visual representation of all layers. qmk c2json generates a JSON file, a custom Python script (qmkjson2yaml.py) converts it to YAML with readable layer names and symbolic keys, and keymap-drawer produces the SVG.

The shell script qmk_viz.sh automates all steps: creating a temporary symlink, generating JSON and YAML, and producing the SVG.

**Dependencies:**  
- QMK CLI (`qmk c2json`)  
- `keymap-drawer` Python module  
- Custom Python script: `qmkjson2yaml.py`  
- Shell script: `qmk_viz.sh`