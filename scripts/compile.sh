#!/bin/bash
set -euo pipefail

# Keymap name
KEYMAP_NAME="my_keymap"
SRC_DIR=~/Source/Symmetra-keyboard-layout/src
LINK_DIR=~/qmk_firmware/keyboards/zsa/voyager/keymaps/"$KEYMAP_NAME"

# Ensure no leftover symlink
[ -L "$LINK_DIR" ] && rm "$LINK_DIR"

# Always clean up symlink when script exits
trap 'rm -f "$LINK_DIR"' EXIT

# Confirm compilation
read -p "Do you really want to compile the keymap? (y/n): " confirm
if [[ "$confirm" != "y" && "$confirm" != "Y" ]]; then
    echo "Compilation canceled by user."
    exit 0
fi

# Create symlink
ln -s "$SRC_DIR" "$LINK_DIR"
 
qmk compile -kb zsa/voyager -km "$KEYMAP_NAME"

# Check result
if [ $? -eq 0 ]; then
    echo "✅ Compilation succeeded!"
else
    echo "❌ Compilation failed. Check QMK logs above."
fi