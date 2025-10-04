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
read -p "Do you want to compile the keymap? (y/n): " confirm
if [[ "$confirm" != "y" && "$confirm" != "Y" ]]; then
    echo "Flashing and compilation canceled by user."
    exit 0
fi

# Create symlink
ln -s "$SRC_DIR" "$LINK_DIR"
  
# Compile  

qmk compile -kb zsa/voyager -km "$KEYMAP_NAME"

echo "✅ Compilation succeeded!"

# Ask if user wants to flash
read -p "Do you want to flash the keyboard now? (y/n), if so flashing will start in 5 seconds: " flash_confirm
if [[ ! "$flash_confirm" =~ ^[Yy]$ ]]; then
    echo "✅ Skipping flashing."
    exit 0
fi

echo "⚠️ Please put your keyboard into bootloader mode..."

for i in {5..1}; do
    echo "⏳ Flashing will start in $i seconds..."
    sleep 1
done

echo "➡️ Flashing of your ZSA Voyager started"
qmk flash -kb zsa/voyager -km "$KEYMAP_NAME"

echo "✅ Flashing succeeded!"