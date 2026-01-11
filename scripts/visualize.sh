#!/bin/bash
set -euo pipefail

# Keymap name
SRC_ROOT="${1:?Usage: $0 <SRC_DIR>}"
KEYMAP_NAME="my_keymap"
LINK_DIR=~/qmk_firmware/keyboards/zsa/voyager/keymaps/"$KEYMAP_NAME"

# Src files
SRC_DIR="$SRC_ROOT/src"

# Results
RESULTS_DIR="$SRC_ROOT/scripts/output"
JSON_FILE="$RESULTS_DIR/keymap.json"
YAML_FILE="$RESULTS_DIR/keymap.yaml"
SVG_FILE="$RESULTS_DIR/keymap.svg"

# Create assets folder in repo if it doesn't exist
ASSETS_DIR="$SRC_ROOT/assets"

# Absolute path to Python conversion script
PYTHON_CONVERTER="$SRC_ROOT/scripts/yamlFromJson.py"

# Always clean up symlink when script exits
trap 'rm -f "$LINK_DIR"' EXIT

# Ensure no leftover symlink
[ -L "$LINK_DIR" ] && rm "$LINK_DIR"

# Create directory if it not exists
mkdir -p "$RESULTS_DIR"  
mkdir -p "$ASSETS_DIR"

# Create symlink
ln -s "$SRC_DIR" "$LINK_DIR"

echo "➡️ Generating JSON..."
qmk c2json -kb zsa/voyager -km "$KEYMAP_NAME" "$LINK_DIR/keymap.c" --no-cpp > "$JSON_FILE"

echo "➡️ Converting JSON to YAML..."
python3 "$PYTHON_CONVERTER" "$JSON_FILE" > "$YAML_FILE"

echo "➡️ Generating SVG..."
python3 -m keymap_drawer draw "$YAML_FILE" --qmk-keyboard zsa/voyager -o "$SVG_FILE"

cp "$SVG_FILE" "$ASSETS_DIR/"
    
echo "✅ Finished! Resulting svg in $ASSETS_DIR"
