# Symmetra-keyboard-layout

QMK layout for my ZSA Voyager.

## Keywords

`qwerty`, `nordic`, `swedish`, `symmetrical`, `40-keys`, `homerow mods`, `2-row numpad`, `mouse`, `win-mac`, `OS-toggle`

## Design choices
 
I have deliberately chosen not to use the top row of the Voyager, partly to _stay_ forward-compatible with other 3×6 + thumb key layouts, and partly to reduce finger travel. 

My mind tends to favor symmetry, so the Mod-Tap and Layer-Tap keys are positioned symmetrically on both sides of the keyboard.

## Layers

![Keymap](visualization/results/keymap.svg)

## Key Translations

Legend for symbols and special keys used in the SVG:

| Symbol       | Meaning                      |
|--------------|------------------------------|
| ▷            | Layer toggle or modifier     |
| Caps Letter  | One-Shot shift modifier      |
| 📟           | Open terminal                | 
| 🌐           | Open browser                 |
| 🔍           | Windows search / Spotlight   | 
| 😀😎🤔😭     | Emoji menu                   |
| 🔒           | Lock screen                  |
| 🔄🪟🍏       | Switch OS (Win/Mac)          |
| 📸           | Screenshot                   |
| 👍           | Thumbs up emoji              | 
| ⏮️           | Rewind                       | 
| ⏭️           | Fast forward                 | 
| ⏹️           | Stop                         | 
| ⏯️           | Play/Pause                   | 

## Visualization Pipeline

The QMK keymap.c is converted into a visual representation of all layers. qmk c2json generates a JSON file, a custom Python script (qmkjson2yaml.py) converts it to YAML with readable layer names and symbolic keys, and keymap-drawer produces the SVG.

The shell script qmk_viz.sh automates all steps: creating a temporary symlink, generating JSON and YAML, and producing the SVG.

**Dependencies:**  
- QMK CLI (`qmk c2json`)  
- `keymap-drawer` Python module  
- Custom Python script: `qmkjson2yaml.py`  
- Shell script: `qmk_viz.sh`