usage: qmk c2json [-h] [-km KEYMAP] [-kb KEYBOARD] [-q] [-o OUTPUT] [--no-cpp]
                  [filename]

positional arguments:
  filename              keymap.c file

options:
  -h, --help            show this help message and exit
  -km, --keymap KEYMAP  The keymap's name
  -kb, --keyboard KEYBOARD
                        The keyboard's name
  -q, --quiet           Quiet mode, only output error messages
  -o, --output OUTPUT   File to write to
  --no-cpp              Do not use 'cpp' on keymap.c
