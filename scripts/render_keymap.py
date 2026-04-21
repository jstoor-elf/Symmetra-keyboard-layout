#!/usr/bin/env python3
"""
Parse src/keymap.c and emit an internal representation of layers, ledmaps,
and combos to stdout as JSON.

Physical key index convention (0-51) follows the LED ordering:
  Left side  0-25:  rows 0-3 (6 keys each) then 2 thumbs
  Right side 26-51: rows 0-3 (6 keys each) then 2 thumbs

LAYOUT_voyager arg order is interleaved per row (L0…L5 R0…R5 per row),
then L-thumbs, R-thumbs — so we remap args to LED-canonical indices.
"""

import re
import json
import sys
from pathlib import Path

ROOT   = Path(__file__).parent.parent
KEYMAP = ROOT / "src" / "keymap.c"

# ── physical layout ───────────────────────────────────────────────────────────

def _make_positions() -> list[dict]:
    """
    52 entries in LED-canonical order.
    Left  (0-25):  row 0-3 cols 0-5, then thumb-outer(24), thumb-inner(25)
    Right (26-51): row 0-3 cols 0-5, then thumb-inner(50), thumb-outer(51)
    """
    pos = []
    for side in ("left", "right"):
        for row in range(4):
            for col in range(6):
                pos.append({"side": side, "row": row, "col": col, "thumb": False})
        if side == "left":
            pos.append({"side": "left",  "row": 4, "col": 0, "thumb": True, "thumb_pos": "outer"})
            pos.append({"side": "left",  "row": 4, "col": 1, "thumb": True, "thumb_pos": "inner"})
        else:
            pos.append({"side": "right", "row": 4, "col": 0, "thumb": True, "thumb_pos": "inner"})
            pos.append({"side": "right", "row": 4, "col": 1, "thumb": True, "thumb_pos": "outer"})
    return pos

POSITIONS = _make_positions()

def _make_kmap_to_led() -> list[int]:
    mapping = []
    for row in range(4):
        for col in range(6):
            mapping.append(row * 6 + col)
        for col in range(6):
            mapping.append(26 + row * 6 + col)
    mapping += [24, 25]
    mapping += [50, 51]
    return mapping

KMAP_TO_LED = _make_kmap_to_led()

# ── label resolution ──────────────────────────────────────────────────────────

LAYER_LABELS = {
    "ALPHA":      "Alpha ⋅ Gallium",
    "SYM_EDIT_L": "Sym Left",
    "SYM_EDIT_R": "Sym Right",
    "NUM":        "Num",
    "FUNC":       "Func",
    "NAV":        "Nav",
    "MOUSE":      "Mouse",
    "SYS":        "Sys",
}

MOD_LABELS = {
    "MOD_LSFT": "Shift", "MOD_RSFT": "Shift",
    "MOD_LCTL": "Ctrl",  "MOD_RCTL": "Ctrl",
    "MOD_LALT": "Alt",   "MOD_RALT": "Alt",
    "MOD_LGUI": "Gui",   "MOD_RGUI": "Gui",
}

LABEL_MAP: dict[str, str] = {
    # dead / unassigned
    "_DEAD_": "⦻", "_OFF_": "", "XXXXXXX": "",

    # Swedish letters
    "SE_ADIA": "Ä", "SE_OSLH": "Ö", "SE_AA": "Å",

    # Symbols
    "SE_QUES":   "?",  "U_SE_LESS": "<",  "U_SE_GRTR": ">",
    "SE_MINS":   "-",  "U_SE_PIPE": "|",  "SE_CIRC":   "^",
    "U_SE_LCBR": "{",  "U_SE_RCBR": "}",  "SE_DLR":    "$",
    "SE_ACUT":   "´",  "SE_GRV":    "`",  "SE_ASTR":   "*",
    "SE_SLSH":   "/",  "SE_EQL":    "=",  "SE_AMPR":   "&",
    "SE_LPRN":   "(",  "SE_RPRN":   ")",  "SE_SCLN":   ";",
    "SE_DQUO":   '"',  "U_SE_BSLS": "\\", "SE_TILD":   "~",
    "SE_PLUS":   "+",  "SE_LBRC":   "[",  "SE_RBRC":   "]",
    "SE_AT":     "@",  "SE_UNDS":   "_",  "SE_COLN":   ":",
    "SE_APOS":   "'",  "SE_PERC":   "%",
    "KC_HASH":   "#",  "KC_EXLM":   "!",  "KC_PERC":   "%",
    "KC_COMMA":  ",",  "KC_DOT":    ".",

    # Common keys
    "KC_BSPC": "Bspc",  "KC_DEL":  "Del",   "KC_ENT":  "Enter",
    "KC_ESC":  "Esc",   "KC_TAB":  "Tab",   "KC_SPC":  "Space",
    "CW_TOGG": "Caps\nWord",

    # Held modifiers (NAV / MOUSE layers)
    "KC_LSFT": "Shift", "KC_RSFT": "Shift",
    "KC_LCTL": "Ctrl",  "KC_RCTL": "Ctrl",
    "KC_LALT": "Alt",   "KC_RALT": "Alt",
    "KC_LGUI": "Gui",   "KC_RGUI": "Gui",

    # Navigation
    "KC_LEFT": "⬅",          "KC_DOWN": "⬇",
    "KC_UP":   "⬆",          "KC_RIGHT": "⮕",
    "U_DOC_LEFT":    "Doc ⬅",    "U_DOC_DOWN":  "Doc ⬇",
    "U_DOC_UP":      "Doc ⬆",    "U_DOC_RIGHT": "Doc ⮕",
    "U_WORD_LEFT":   "Word ⬅",   "U_WORD_RIGHT": "Word ⮕",
    "U_5_ROWS_DOWN": "5-Rows ⬇", "U_5_ROWS_UP":  "5-Rows ⬆",

    # Mouse
    "MS_BTN1": "Click L",    "MS_BTN2": "Click R",   "MS_BTN3": "Click M",
    "MS_UP":   "Mouse ⬆",    "MS_DOWN": "Mouse ⬇",
    "MS_LEFT": "Mouse ⬅",    "MS_RGHT": "Mouse ⮕",
    "MS_WHLU": "Wheel ⬆",    "MS_WHLD": "Wheel ⬇",

    # Edit / selection
    "U_SAVE":      "Save",      "U_CUT":      "Cut",
    "U_COPY":      "Copy",      "U_PASTE":    "Paste",
    "U_UNDO":      "Undo",      "U_REDO":     "Redo",
    "U_SEARCH":    "Find",      "U_MARK_ALL": "Select All",
    "U_NUM_ENTER": "Enter",
    "SELLINE":  "Select Line",  "SELWBAK": "Select ⬅Word", "SELWORD": "Select Word⮕",

    # System
    "KC_VOLD": "Vol -",      "KC_VOLU": "Vol +",    "KC_MUTE": "Mute",
    "KC_MPRV": "Prev Track", "KC_MNXT": "Next Track",  "KC_MPLY": "Play/ Pause",
    "U_OS_SEARCH":  "OS Search",    "U_SCREENSHOT": "Scrrenshot",
    "U_EMOJIS":     "Emojis",    "U_TOGGLE_OS":  "Switch OS",
}

_OSL_SYM = "○"   # one-shot layer prefix
_MO_SYM  = "▲"   # hold/momentary layer prefix
_TG_SYM  = "⇔"   # toggle layer prefix

def _layer_label(name: str) -> str:
    return LAYER_LABELS.get(name, name)

def _mod_label(mod: str) -> str:
    return MOD_LABELS.get(mod.strip(), mod.strip())

def map_key(keycode: str) -> str:
    """Resolve a raw QMK keycode token to a human-readable label."""
    if not keycode:
        return ""

    # OSM(MOD_xxx) — one-shot modifier
    m = re.fullmatch(r"OSM\((\w+)\)", keycode)
    if m:
        return f"{_OSL_SYM}\u00a0{_mod_label(m.group(1))}"

    # OSL(LAYER) — one-shot layer
    m = re.fullmatch(r"OSL\((\w+)\)", keycode)
    if m:
        return f"{_OSL_SYM}\u00a0{_layer_label(m.group(1))}"

    # LT(LAYER, key) — layer-tap
    m = re.fullmatch(r"LT\((\w+),\s*([^)]+)\)", keycode)
    if m:
        layer, tap = m.group(1), m.group(2).strip()
        return f"{map_key(tap)}\n{_MO_SYM}\u00a0{_layer_label(layer)}"

    # MT(MOD, key) — mod-tap
    m = re.fullmatch(r"MT\((\w+),\s*([^)]+)\)", keycode)
    if m:
        mod, tap = m.group(1), m.group(2).strip()
        return f"{map_key(tap)}\n{_mod_label(mod)}"

    # MO(LAYER) — momentary layer
    m = re.fullmatch(r"MO\((\w+)\)", keycode)
    if m:
        return f"{_MO_SYM}\u00a0{_layer_label(m.group(1))}"

    # TG(LAYER) — toggle layer
    m = re.fullmatch(r"TG\((\w+)\)", keycode)
    if m:
        return f"{_TG_SYM}\u00a0{_layer_label(m.group(1))}"

    # direct lookup
    if keycode in LABEL_MAP:
        return LABEL_MAP[keycode]

    # KC_xxx / U_xxx — strip prefix, title-case
    for prefix in ("KC_", "U_"):
        if keycode.startswith(prefix):
            return keycode.removeprefix(prefix).replace("_", " ").title().replace(" ", "\n")

    return keycode

def _split_spaces(label: str) -> str:
    """Replace spaces with newlines so multi-word labels stack vertically."""
    return label.replace(" ", "\n")

# Patch map_key to auto-split spaces in the final label
_map_key_raw = map_key
def map_key(keycode: str) -> str:
    return _split_spaces(_map_key_raw(keycode))

# ── source helpers ────────────────────────────────────────────────────────────

def strip_line_comments(src: str) -> str:
    return re.sub(r"//[^\n]*", "", src)

def strip_block_comments(src: str) -> str:
    return re.sub(r"/\*.*?\*/", " ", src, flags=re.DOTALL)

def clean(src: str) -> str:
    return strip_block_comments(strip_line_comments(src))

def tokenize_args(s: str) -> list[str]:
    tokens, depth, cur = [], 0, []
    for ch in s:
        if ch == "(":
            depth += 1; cur.append(ch)
        elif ch == ")":
            depth -= 1; cur.append(ch)
        elif ch == "," and depth == 0:
            tokens.append("".join(cur).strip()); cur = []
        else:
            cur.append(ch)
    if cur:
        tokens.append("".join(cur).strip())
    return tokens

def _extract_body(src: str, start: int) -> str:
    depth, i = 0, src.index("{", start)
    begin = i + 1
    while i < len(src):
        if src[i] == "{":   depth += 1
        elif src[i] == "}": depth -= 1
        if depth == 0:
            return src[begin:i]
        i += 1
    raise ValueError("Unmatched brace")

def parse_key_aliases(src: str) -> dict[str, str]:
    """Parse simple single-value #defines used as key aliases (T_L_OUT etc.)."""
    aliases: dict[str, str] = {}
    for m in re.finditer(r"^#define\s+(\w+)\s+(\S[^\n]*\S|\S)\s*$", src, re.MULTILINE):
        name, value = m.group(1), m.group(2).strip()
        if name.startswith("C_") or name.startswith("LED_"):
            continue
        aliases[name] = value
    return aliases

# ── section parsers ───────────────────────────────────────────────────────────

def parse_layer_enum(src: str) -> dict[str, int]:
    m = re.search(r"enum\s+layers\s*\{([^}]+)\}", src, re.DOTALL)
    if not m:
        return {}
    return {name: i for i, name in enumerate(re.findall(r"\b([A-Z_][A-Z0-9_]*)\b", m.group(1)))}

def parse_color_defines(src: str) -> dict[str, dict]:
    colors: dict[str, dict] = {}
    for m in re.finditer(
        r"^#define\s+(C_\w+)\s+\{(\s*\d+\s*,\s*\d+\s*,\s*\d+\s*)\}.*$",
        src, re.MULTILINE,
    ):
        h, s, v = [int(x.strip()) for x in m.group(2).split(",")]
        colors[m.group(1)] = {"h": h, "s": s, "v": v}
    return colors

def parse_keymaps(src: str) -> dict[str, list[str | None]]:
    body = _extract_body(src, src.index("PROGMEM keymaps"))
    result: dict[str, list[str | None]] = {}
    for m in re.finditer(r"\[(\w+)\]\s*=\s*LAYOUT_voyager\s*\(", body):
        layer = m.group(1)
        i = m.end()
        depth = 1
        while i < len(body) and depth:
            if body[i] == "(":   depth += 1
            elif body[i] == ")": depth -= 1
            i += 1
        args = tokenize_args(body[m.end() : i - 1])
        canonical: list[str | None] = [None] * 52
        for arg_idx, led_idx in enumerate(KMAP_TO_LED):
            canonical[led_idx] = args[arg_idx] if arg_idx < len(args) else None
        result[layer] = canonical
    return result

def parse_ledmaps(src: str) -> dict[str, list[str | None]]:
    body = _extract_body(src, src.index("PROGMEM ledmap"))
    result: dict[str, list[str | None]] = {}
    for m in re.finditer(r"\[(\w+)\]\s*=\s*\{", body):
        layer = m.group(1)
        i = m.end()
        depth = 1
        while i < len(body) and depth:
            if body[i] == "{":   depth += 1
            elif body[i] == "}": depth -= 1
            i += 1
        inner = body[m.end() : i - 1]
        colors = [t.strip() for t in inner.split(",") if t.strip()]
        result[layer] = colors[:52] + [None] * max(0, 52 - len(colors))
    return result

def parse_combos(src: str) -> list[dict]:
    combo_keys: dict[str, list[str]] = {}
    for m in re.finditer(
        r"const\s+uint16_t\s+PROGMEM\s+(\w+)\[\]\s*=\s*\{([^}]+)\}", src,
    ):
        keys = [k.strip() for k in m.group(2).split(",")
                if k.strip() not in ("COMBO_END", "")]
        combo_keys[m.group(1)] = keys

    combos: list[dict] = []
    kc_body = _extract_body(src, src.index("key_combos[]"))
    for m in re.finditer(r"COMBO\s*\(", kc_body):
        i, depth = m.end(), 1
        while i < len(kc_body) and depth:
            if kc_body[i] == "(":   depth += 1
            elif kc_body[i] == ")": depth -= 1
            i += 1
        args = tokenize_args(kc_body[m.end() : i - 1])
        if len(args) >= 2:
            combos.append({"keys": combo_keys.get(args[0], []), "action": args[1].strip()})
    return combos

# ── assemble IR ───────────────────────────────────────────────────────────────

def build_ir() -> dict:
    raw = KEYMAP.read_text()
    src = clean(raw)

    layer_enum  = parse_layer_enum(src)
    color_defs  = parse_color_defines(raw)
    key_aliases = parse_key_aliases(src)
    keymaps     = parse_keymaps(src)
    ledmaps     = parse_ledmaps(src)
    combos      = parse_combos(src)

    dead = {"_DEAD_", "_OFF_", "XXXXXXX", None}

    def resolve(kc: str | None) -> str | None:
        """Expand a key alias #define before label resolution."""
        if kc is None:
            return None
        return key_aliases.get(kc, kc)

    layers = []
    keycode_to_layers: dict[str, set[str]] = {}
    for name, idx in sorted(layer_enum.items(), key=lambda x: x[1]):
        keys_raw   = keymaps.get(name, [None] * 52)
        colors_raw = ledmaps.get(name, [None] * 52)

        keys = []
        for led_idx, pos in enumerate(POSITIONS):
            raw_kc    = keys_raw[led_idx]
            resolved  = resolve(raw_kc)
            if raw_kc not in dead:
                keycode_to_layers.setdefault(raw_kc, set()).add(name)
            keys.append({
                **pos,
                "led_index": led_idx,
                "keycode":   raw_kc,
                "label":     map_key(raw_kc if raw_kc in LABEL_MAP else (resolved or "")),
                "color":     colors_raw[led_idx],
            })

        layers.append({"name": name, "index": idx, "keys": keys})

    keycode_to_sides: dict[str, set[str]] = {}
    for layer in layers:
        for k in layer["keys"]:
            kc = k["keycode"]
            if kc not in dead:
                keycode_to_sides.setdefault(kc, set()).add(k["side"])

    annotated_combos = []
    for combo in combos:
        sets = [keycode_to_layers.get(k, set()) for k in combo["keys"]]
        common = set.intersection(*sets) if sets else set()

        sides: set[str] = set()
        for k in combo["keys"]:
            sides |= keycode_to_sides.get(k, set())
        side = "both" if len(sides) == 2 else (next(iter(sides)) if sides else None)

        annotated_combos.append({
            **combo,
            "action_label": map_key(resolve(combo["action"])),
            "key_labels":   [map_key(resolve(k)) for k in combo["keys"]],
            "layers":       sorted(common),
            "side":         side,
        })

    return {
        "layers":        layers,
        "combos":        annotated_combos,
        "color_palette": color_defs,
    }

# ── renderer ──────────────────────────────────────────────────────────────────

import colorsys
import copy
import xml.etree.ElementTree as ET

ET.register_namespace("",      "http://www.w3.org/2000/svg")
ET.register_namespace("xlink", "http://www.w3.org/1999/xlink")

TEMPLATE_PATH = Path(__file__).parent / "layer_template.svg"
_NS  = "http://www.w3.org/2000/svg"
_T   = lambda tag: f"{{{_NS}}}{tag}"

_CANVAS_W  = 928
_MARGIN    = 30
_LAYER_H   = 450   # matches template height
_LAYER_GAP = 30

_BG        = "#212123"
_KEY_BG    = "#141416"
_BORDER    = "#30363d"
_DIM       = "#6e7681"   # dead / off keys

def _contrast_color(h: str) -> str:
    h = h.lstrip("#")
    r, g, b = (int(h[i:i+2], 16) / 255 for i in (0, 2, 4))
    luminance = 0.299 * r + 0.587 * g + 0.114 * b
    return "#dededc" if luminance < 0.5 else "#1a1a1c"

_LABEL_CLR = _contrast_color("#ffffff")

_CSS = f"""\
svg.keymap {{
    font-family: SFMono-Regular, Consolas, Liberation Mono, Menlo, monospace;
    font-size: 13px;
}}
.key      {{ fill: {_KEY_BG}; stroke: {_BORDER}; stroke-width: 1; }}
.key.dead {{ fill: {_KEY_BG}; stroke: {_BORDER}; stroke-width: 1; }}
text {{ text-anchor: middle; dominant-baseline: middle; }}
text.label {{ font-size: 15px; font-weight: bold; fill: #ffffff;
              stroke: #1a1a1c; stroke-width: 5px; paint-order: stroke fill;
              text-anchor: start; dominant-baseline: auto; }}
"""

def _hsv_to_hex(h: int, s: int, v: int) -> str | None:
    """QMK HSV (0-255 each) → CSS hex, boosted to at least 75% brightness for legibility."""
    if v == 0:
        return None
    r, g, b = colorsys.hsv_to_rgb(h / 255.0, s / 255.0, max(v, 192) / 255.0)
    return f"#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}"

def _make_text(label: str, color: str) -> ET.Element:
    lines  = label.split("\n")
    shrink = any(len(l) > 7 for l in lines)
    el = ET.Element(_T("text"))
    el.set("x", "0"); el.set("y", "0")
    el.set("fill", color)
    if shrink:
        el.set("style", "font-size:10px")
    if len(lines) == 1:
        el.text = lines[0]
    else:
        for i, line in enumerate(lines):
            ts = ET.SubElement(el, _T("tspan"))
            ts.set("x", "0"); ts.set("dy", "-.6em" if i == 0 else "1.2em")
            ts.text = line
    return el

def _fill_layer(template_root: ET.Element, layer: dict, palette: dict) -> ET.Element:
    root = copy.deepcopy(template_root)

    label_el = root.find(f".//{_T('text')}[@id='layer-label']")
    if label_el is not None:
        label_el.text = LAYER_LABELS.get(layer["name"], layer["name"])

    for key in layer["keys"]:
        led_idx = key["led_index"]
        group   = root.find(f".//{_T('g')}[@id='key-{led_idx}']")
        if group is None:
            continue

        kc      = key.get("keycode")
        label   = key.get("label", "")
        color   = key.get("color")
        is_dead = kc == "_DEAD_"

        if not label:
            continue

        text_color = _DIM
        if not is_dead and color and color in palette:
            c = palette[color]
            h = _hsv_to_hex(c["h"], c["s"], c["v"])
            if h:
                text_color = h

        group.append(_make_text(label, text_color))

    return root

def render_svg(ir: dict) -> str:
    layers  = ir["layers"]
    palette = ir["color_palette"]
    template_root = ET.parse(TEMPLATE_PATH).getroot()

    total_h = len(layers) * (_LAYER_H + _LAYER_GAP) + 40
    parts = [
        f'<svg width="{_CANVAS_W}" height="{total_h}" viewBox="0 0 {_CANVAS_W} {total_h}" '
        f'class="keymap" xmlns="http://www.w3.org/2000/svg">',
        f'<style>{_CSS}</style>',
    ]

    for i, layer in enumerate(layers):
        y    = i * (_LAYER_H + _LAYER_GAP)
        root = _fill_layer(template_root, layer, palette)
        parts.append(f'<g transform="translate({_MARGIN},{y})">')
        for child in root:
            parts.append(ET.tostring(child, encoding="unicode"))
        parts.append("</g>")

    parts.append("</svg>")
    return "\n".join(parts)

# ── entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument("--svg", metavar="FILE", help="write SVG to FILE instead of JSON to stdout")
    args = p.parse_args()

    ir = build_ir()
    if args.svg:
        Path(args.svg).write_text(render_svg(ir))
        print(f"wrote {args.svg}", file=sys.stderr)
    else:
        json.dump(ir, sys.stdout, indent=2)
        print()
