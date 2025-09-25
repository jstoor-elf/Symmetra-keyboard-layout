#!/bin/bash
set -euo pipefail

# Keymap name
KEYMAP_NAME="my_keymap"
SRC_DIR=~/Source/Symmetra-keyboard-layout/src
LINK_DIR=~/qmk_firmware/keyboards/zsa/voyager/keymaps/"$KEYMAP_NAME"

# Results
RESULTS_DIR=~/Source/Symmetra-keyboard-layout/output
JSON_FILE="$RESULTS_DIR/keymap.json"
YAML_FILE="$RESULTS_DIR/keymap.yaml"
SVG_FILE="$RESULTS_DIR/keymap.svg"

# Absolute path to Python conversion script
PYTHON_CONVERTER=~/Source/Symmetra-keyboard-layout/scripts/yamlFromJson.py

# Always clean up symlink when script exits
trap 'rm -f "$LINK_DIR"' EXIT

# Ensure no leftover symlink
[ -L "$LINK_DIR" ] && rm "$LINK_DIR"

# Create directory if it not exists
mkdir -p "$RESULTS_DIR"  

# Create symlink
ln -s "$SRC_DIR" "$LINK_DIR"

echo "➡️ Generating JSON..."
qmk c2json -kb zsa/voyager -km "$KEYMAP_NAME" "$LINK_DIR/keymap.c" --no-cpp > "$JSON_FILE"

echo "➡️ Converting JSON to YAML..."
python3 "$PYTHON_CONVERTER" "$JSON_FILE" > "$YAML_FILE"

echo "➡️ Generating SVG..."
python3 -m keymap_drawer draw "$YAML_FILE" --qmk-keyboard zsa/voyager -o "$SVG_FILE"

echo "✅ Finished! Results in $RESULTS_DIR"