#!/bin/bash
set -euo pipefail

# ---- CONFIG ----
KEYMAP_NAME="my_keymap"
QMK_DIR=~/qmk_firmware
KEYBOARD_PATH="keyboards/zsa/voyager/keymaps"
LINK_DIR="$QMK_DIR/$KEYBOARD_PATH/$KEYMAP_NAME"

# ---- ARG CHECK ----
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <path-to-keymap-root>"
    echo "Example: $0 ~/Source/Symmetra-keyboard-layout"
    exit 1
fi

SRC_ROOT="$1"
SRC_DIR="$SRC_ROOT/src"

if [[ ! -d "$SRC_DIR" ]]; then
    echo "❌ Source directory not found: $SRC_DIR"
    exit 1
fi

# ---- CLEANUP ----
[ -L "$LINK_DIR" ] && rm "$LINK_DIR"
trap 'rm -f "$LINK_DIR"' EXIT

# ---- CONFIRM COMPILE ----
read -p "Do you want to compile the keymap? (y/n): " confirm
if [[ ! "$confirm" =~ ^[Yy]$ ]]; then
    echo "Compilation canceled by user."
    exit 0
fi

# ---- SYMLINK ----
ln -s "$SRC_DIR" "$LINK_DIR"

# ---- COMPILE ----
qmk compile -kb zsa/voyager -km "$KEYMAP_NAME"
echo "✅ Compilation succeeded!"

# ---- FLASH ----
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
