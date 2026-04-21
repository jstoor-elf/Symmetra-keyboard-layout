# Gallium 34 key QMK layout

QMK layout for my ZSA Voyager.

A 34-key layout (3×5 + 2 thumbs per side) designed to minimize finger travel. Compatible with the ZSA Voyager as well as boards like the Ferris Sweep and other similar 34-key split keyboards. The top row and outer columns of the Voyager have been physically removed.

The layout is mainly inspired by [Jonas Hietala's T-34](https://www.jonashietala.se/blog/2021/06/03/the-t-34-keyboard-layout/). After finding out the hard way that home-row mods are not for me — too many timing issues — I moved to one-shot modifiers and combos instead.

My alpha layout is [Gallium](https://github.com/GalileoBlues/Gallium), with Swedish letters Ä and Ö on the base layer, and Å via a combo of the two.

![Keyboard](assets/voyager.jpeg)

## Keywords

`gallium`, `win-mac`, `nordic`, `swedish`, `34-keys`, `one-shot-mods`, `combos`, `vim-navigation`, `mouse-layer`, `edit-cluster`, `getreuer-select-word`, `OS-toggle`

## Layers

The keymap below shows key positions and RGB coloring for each layer. The removed outer columns and top row are rendered in a different geometry to reflect that those switches have been physically removed.

![Keymap](assets/keymap.svg)


## Visualization Pipeline

The keymap SVG is generated directly from `src/keymap.c` by a custom Python script.

```
python3 visualization/parse_keymap.py --svg visualization/keymap.svg
```

**Dependencies:**
- Python 3.11+
- No external Python packages required
