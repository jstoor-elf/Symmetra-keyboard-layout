#!/bin/bash
set -euo pipefail

KEYMAP_NAME="my_keymap"

# Ask if user wants to flash
read -p "Do you really want to flash the keyboard now? (y/n): " flash_confirm
if [[ ! "$flash_confirm" =~ ^[Yy]$ ]]; then
    echo "Skipping flashing."
    exit 0
fi

# Ask if keyboard is in bootloader
read -p "Is your keyboard in bootloader mode? (y/n): " ready_confirm
if [[ ! "$ready_confirm" =~ ^[Yy]$ ]]; then
    echo "Flashing canceled. Keyboard not in bootloader mode."
    exit 0
fi

qmk flash -kb zsa/voyager -km "$KEYMAP_NAME"

if [ $? -eq 0 ]; then
    echo "✅ Flashing succeeded!"
else
    echo "❌ Flashing failed."
fi