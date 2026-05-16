# Gallium 34 key QMK layout

QMK layout for my ZSA Voyager.

A 34-key layout (3×5 + 2 thumbs per side) designed to minimize finger travel. Compatible with the ZSA Voyager as well as boards like the Ferris Sweep. The switches in the top row and outer columns of the Voyager have been physically removed.

The layout is mainly inspired by [Jonas Hietala's T-34](https://www.jonashietala.se/blog/2021/06/03/the-t-34-keyboard-layout/). After finding out the hard way that home-row mods are not for me — too many timing issues — I moved to combos and the chord model instead

My alpha layout is [Gallium](https://github.com/GalileoBlues/Gallium), with Swedish letters Ä and Ö on the base layer, and Å via a combo of the two. A dedicated OS toggle lets you switch between Mac and Windows shortcuts/commands on the fly.

![Keyboard](assets/voyager.jpeg)

## Keywords

`gallium`, `win-mac`, `nordic`, `swedish`, `34-keys`, `home-row-combos`, `combos`, `vim-navigation`, `mouse-layer`, `edit-cluster`, `getreuer-select-word`, `OS-toggle`

## Layers

The keymap below shows key positions and RGB coloring for each layer. The keycaps in the outer columns and top row are rendered in a different geometry to reflect that those switches have been physically removed.   

A legend is added explaining the color-coded underlines used on action keys — each color represents a different action type: one-shot layer (OSL), one-shot modifier (OSM), momentary (MO), layer-tap (LT), and toggle (TG). 

Same-side combos are shown as overlay boxes on the affected keys, while cross-side combos use dashed routing lines converging on a central indicator box. Anchor-key combos are collected into dedicated panels below the layer views: symbols and numbers use the thumb keys as anchors, while shortcuts use the right home-row pinky.

![Keymap](assets/keymap.svg)


## Design Rationale

The core idea is **chords over sequences** — where a sequence can mean order, duration, or both. A chord expresses intent through combination — two keys pressed together mean one thing, defined by which keys, not by sequential action. A sequence expresses intent through order and duration — the firmware has to infer what you meant from when you pressed, held, and released. One-shot keys follow that model: press, release, press the next key, and trust the firmware to read the sequence as a modified keystroke. Both approaches have a timing window under the hood, but the mental model is different: a chord is "these two things together", a sequence is "this, then that".

The result is a system where every intentional action is a single atomic event — one thing — triggered by a combination of simultaneous keys: symbols and numbers via the two thumb keys, shortcuts via the right home-row pinky, and one-shot modifiers and layer access via home-row combos. Text is still a sequence of individual keypresses, but every non-text action is a chord: one intent, one event, defined by which keys you pressed together.

A consequence of this philosophy combined with a personal preference for minimizing finger movement is an extensive use of combos concentrated on the home row and the two main thumb keys. Modifiers, layer access, symbols, numbers, and shortcuts are all reachable without leaving the home position.

## Visualization Pipeline

The keymap SVG is generated directly from `src/keymap.c` by a custom Python script. It renders all layers with key labels, RGB colors, and combo indicators for same-side combos.

```bash
./scripts/visualize.sh
```

Output is written to `assets/keymap.svg`. Optional flags:

| Flag | Description |
|------|-------------|
| `--json` | Dump the internal IR as JSON to stdout instead |
| `--svg FILE` | Write SVG to a custom path instead of `assets/keymap.svg` |

**Template:** `assets/layer_template.svg` — defines key positions and geometry for the ZSA Voyager.

**Dependencies:**
- Python 3.11+
- No external Python packages required
