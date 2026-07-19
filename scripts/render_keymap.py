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
    "ALPHA":    "Alpha",
    "NUM":      "NumWord",
    "FUNC":     "Func",
    "NAV":      "Nav",
    "MOUSE":    "Mouse",
    "SYS":      "Sys",
    "MOD":      "Mod",
    "SHORTCUT": "Shrtc",
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
    "U_SE_BRC_PAIR": "[]", "U_SE_PRN_PAIR": "()",
    "U_SE_CBR_PAIR": "{}", "U_SE_ABK_PAIR": "<>",
    "KC_HASH":   "#",  "KC_EXLM":   "!",  "KC_PERC":   "%",
    "KC_COMMA":  ",",  "KC_DOT":    ".",

    # Common keys
    "KC_BSPC": "⌫",  "KC_DEL":  "Del",   "KC_ENT":  "Enter",
    "KC_ESC":  "Esc",   "KC_TAB":  "Tab",   "KC_SPC":  "Space",
    "CW_TOGG": "CapsWord", "QK_REP": "↻",

    # Held modifiers (NAV / MOUSE layers)
    "KC_LSFT": "Shift", "KC_RSFT": "Shift",
    "KC_LCTL": "Ctrl",  "KC_RCTL": "Ctrl",
    "KC_LALT": "Alt",   "KC_RALT": "Alt",
    "KC_LGUI": "Gui",   "KC_RGUI": "Gui",

    # Navigation
    "KC_LEFT": "←",          "KC_DOWN": "↓",
    "KC_UP":   "↑",          "KC_RIGHT": "→",
    "U_DOC_LEFT":    "Doc ←",    "U_DOC_DOWN":  "Doc ↓",
    "U_DOC_UP":      "Doc ↑",    "U_DOC_RIGHT": "Doc →",
    "U_WORD_LEFT":   "Word ←",   "U_WORD_RIGHT": "Word →",
    "U_5_ROWS_DOWN": "5-Rows ↓", "U_5_ROWS_UP":  "5-Rows ↑",

    # Mouse
    "MS_BTN1": "Click L",    "MS_BTN2": "Click R",   "MS_BTN3": "Click M",
    "MS_UP":   "Mouse ↑",    "MS_DOWN": "Mouse ↓",
    "MS_LEFT": "Mouse ←",    "MS_RGHT": "Mouse →",
    "MS_WHLU": "Wheel ↑",    "MS_WHLD": "Wheel ↓",

    # Edit / selection
    "U_SAVE":      "Save",      "U_CUT":      "Cut",
    "U_COPY":      "Copy",      "U_PASTE":    "Paste",
    "U_UNDO":      "Undo",      "U_REDO":     "Redo",
    "U_SEARCH":    "Find",      "U_MARK_ALL": "Select\nAll",
    "U_NUM_ENTER": "Enter", "U_NUM_SPACE": "Space",
    "U_NUM_TGL": "", "U_NUM_ENT_ANC": "",
    "U_FUNC_DEAC_L": "", "U_FUNC_DEAC_R": "", "U_SYS_DEAC_L": "", "U_SYS_DEAC_R": "",
    "SELLINE":  "Select Line",  "SELWBAK": "Select ←Word", "SELWORD": "Select Word→",

    # System
    "KC_VOLD": "Vol -",      "KC_VOLU": "Vol +",    "KC_MUTE": "Mute",
    "KC_MPRV": "Prev Track", "KC_MNXT": "Next Track",  "KC_MPLY": "Play/ Pause",
    "RM_VALD": "RGB -",      "RM_VALU": "RGB +",    "U_RGB_TOG": "RGB Toggle",
    "U_OS_SEARCH":  "OS Search",    "U_SCREENSHOT": "Screenshot",
    "U_EMOJIS":     "Emojis",       "U_TOGGLE_OS":  "Switch OS",
    "U_LOCK_SCREEN": "Lock\nScreen", "KC_CAPS": "Caps\nLock",
    "U_FIND_PREV":  "Find\nPrev", "U_FIND_NEXT":  "Find\nNext",
    "U_REPLACE":    "Replace",
}

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
        return _mod_label(m.group(1))

    # OSL(LAYER) — one-shot layer
    m = re.fullmatch(r"OSL\((\w+)\)", keycode)
    if m:
        return _layer_label(m.group(1))

    # LT(LAYER, key) — layer-tap
    m = re.fullmatch(r"LT\((\w+),\s*([^)]+)\)", keycode)
    if m:
        layer, tap = m.group(1), m.group(2).strip()
        return f"{map_key(tap)}\n{_layer_label(layer)}"

    # MT(MOD, key) — mod-tap
    m = re.fullmatch(r"MT\((\w+),\s*([^)]+)\)", keycode)
    if m:
        mod, tap = m.group(1), m.group(2).strip()
        return f"{map_key(tap)}\n{_mod_label(mod)}"

    # MO(LAYER) — momentary layer
    m = re.fullmatch(r"MO\((\w+)\)", keycode)
    if m:
        return _layer_label(m.group(1))

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
    keycode_to_led: dict[str, int] = {}
    for name, idx in sorted(layer_enum.items(), key=lambda x: x[1]):
        keys_raw   = keymaps.get(name, [None] * 52)
        colors_raw = ledmaps.get(name, [None] * 52)

        keys = []
        for led_idx, pos in enumerate(POSITIONS):
            raw_kc    = keys_raw[led_idx]
            resolved  = resolve(raw_kc)
            if raw_kc not in dead:
                keycode_to_layers.setdefault(raw_kc, set()).add(name)
                if raw_kc not in keycode_to_led:
                    keycode_to_led[raw_kc] = led_idx
            keys.append({
                **pos,
                "led_index":       led_idx,
                "keycode":         raw_kc,
                "resolved_keycode": resolved or raw_kc,
                "label":           map_key(raw_kc if raw_kc in LABEL_MAP else (resolved or "")),
                "color":           colors_raw[led_idx],
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

        led_indices = [keycode_to_led[k] for k in combo["keys"] if k in keycode_to_led]
        resolved_action = resolve(combo["action"]) or combo["action"]
        annotated_combos.append({
            **combo,
            "resolved_action": resolved_action,
            "action_label":    map_key(resolved_action),
            "key_labels":      [map_key(resolve(k)) for k in combo["keys"]],
            "layers":          sorted(common),
            "side":            side,
            "led_indices":     led_indices,
        })

    _inner_thumbs = {25, 50}
    _all_thumbs   = {24, 25, 50, 51}
    for c in annotated_combos:
        idxs = c.get("led_indices", [])
        c["thumb_key_combo"] = (
            len(idxs) == 2
            and bool(set(idxs) & _inner_thumbs)
            and not all(i in _all_thumbs for i in idxs)
        )
        c["thumb_3key_combo"] = (
            len(idxs) == 3
            and bool(set(idxs) & _inner_thumbs)
            and not all(i in _all_thumbs for i in idxs)
        )

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

TEMPLATE_PATH = ROOT / "assets" / "layer_template.svg"
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

_ACTION_COLORS: dict[str, str] = {
    "osl":     "#7dd4a0",  # pastel green   — one-shot layer
    "osm":     "#f4d06d",  # pastel yellow  — one-shot modifier
    "hold":    "#f4a96d",  # pastel orange  — momentary (MO)
    "lt":      "#7eb8f7",  # pastel blue    — layer-tap (LT)
    "toggle":  "#b99af5",  # pastel lavender — toggle

    "numenter": "#b99af5", # pastel lavender — num enter (exits layer)
}

_LEGEND_H = 160  # height reserved above the first layer for the legend

_COMBO_SPACE_LED   = 25
_COMBO_ENTER_LED   = 50
_COMBO_SC_LED    = 51          # right outer thumb – Sys / shortcut anchor
_COMBO_SPACE_COLOR = "#559e82"   # muted mint green – Space thumb
_COMBO_ENTER_COLOR = "#b87090"   # muted pink – Enter thumb
_COMBO_SC_COLOR  = "#7090c8"   # muted blue – Sys shortcut anchor


def _classify_thumb_combos(combos: list[dict]) -> tuple[list, list, list, list]:
    """Group thumb+key combos → (space_sym, enter_sym, space_num, enter_num)."""
    space_sym, enter_sym, space_num, enter_num = [], [], [], []
    for c in combos:
        if not c.get("thumb_key_combo"):
            continue
        idxs = c["led_indices"]
        if _COMBO_SPACE_LED in idxs:
            target = next(i for i in idxs if i != _COMBO_SPACE_LED)
            (space_sym if target >= 26 else space_num).append(c)
        else:
            target = next(i for i in idxs if i != _COMBO_ENTER_LED)
            (enter_sym if target < 26 else enter_num).append(c)
    return space_sym, enter_sym, space_num, enter_num


def _classify_shortcut_combos(combos: list[dict]) -> list:
    """Group Fn (left outer thumb) + left-side key combos for the Shortcut panel."""
    result = []
    for c in combos:
        if c.get("thumb_key_combo") or c.get("thumb_3key_combo"):
            continue
        idxs = c.get("led_indices", [])
        if len(idxs) == 2 and _COMBO_SC_LED in idxs:
            target = next(i for i in idxs if i != _COMBO_SC_LED)
            if target < 26 and not POSITIONS[target]["thumb"]:
                result.append(c)
    return result


def _classify_thumb_3key_combos(combos: list[dict]) -> tuple[list, list, list, list]:
    """Group 3-key thumb combos → (space_sym, enter_sym, space_num, enter_num)."""
    space_sym, enter_sym, space_num, enter_num = [], [], [], []
    for c in combos:
        if not c.get("thumb_3key_combo"):
            continue
        idxs = c["led_indices"]
        if _COMBO_SPACE_LED in idxs:
            non_thumb = [i for i in idxs if i != _COMBO_SPACE_LED]
            (space_sym if all(i >= 26 for i in non_thumb) else space_num).append(c)
        elif _COMBO_ENTER_LED in idxs:
            non_thumb = [i for i in idxs if i != _COMBO_ENTER_LED]
            (enter_sym if all(i < 26 for i in non_thumb) else enter_num).append(c)
    return space_sym, enter_sym, space_num, enter_num


def _3key_thumb_combo_overlay(combo: dict, key_centers: dict[int, tuple[float, float]],
                               keys_y_off: float, color: str, thumb_led: int) -> str:
    non_thumb = [i for i in combo.get("led_indices", []) if i != thumb_led]
    positions = [key_centers[i] for i in non_thumb if i in key_centers]
    if len(positions) < 2:
        return ""
    xs = [p[0] for p in positions]
    ys = [p[1] + keys_y_off for p in positions]
    cx_c = (min(xs) + max(xs)) / 2
    cy_c = (min(ys) + max(ys)) / 2
    _indent  = _COMBO_INDENT + 10
    _half    = _COMBO_HALF_MIN - 4
    _min_h   = 24
    x1 = min(min(xs) + _indent, cx_c - _half)
    x2 = max(max(xs) - _indent, cx_c + _half)
    y1 = min(min(ys) + _indent, cy_c - _half)
    y2 = max(max(ys) - _indent, cy_c + _half)
    if y2 - y1 < _min_h:
        y1 = cy_c - _min_h / 2
        y2 = cy_c + _min_h / 2
    w, h = x2 - x1, y2 - y1
    cx, cy = (x1 + x2) / 2, (y1 + y2) / 2
    label = combo["action_label"].replace("\n", " ")
    label = label.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    sz = "7px" if len(label) > 6 else "10px"
    return "\n".join([
        f'<rect x="{x1:.1f}" y="{y1:.1f}" width="{w:.1f}" height="{h:.1f}" '
        f'rx="4" fill="#141416" fill-opacity="0.88" stroke="#a0a0a0" stroke-width="0.5"/>',
        f'<text x="{cx:.1f}" y="{cy:.1f}" text-anchor="middle" dominant-baseline="middle" '
        f'fill="#a0a0a0" stroke="#a0a0a0" stroke-width="0.5" paint-order="stroke fill" '
        f'style="font-size:{sz};font-weight:400;">{label}</text>',
    ])


def _render_dual_combo_panel(template_root: ET.Element, title: str,
                             space_combos: list[dict], enter_combos: list[dict],
                             alpha_keys: list[dict],
                             shortcut_combos: list[dict] | None = None) -> ET.Element:
    """Two-thumb combo panel: Space targets in blue, Enter targets in coral."""
    root = copy.deepcopy(template_root)

    label_el = root.find(f".//{_T('text')}[@id='layer-label']")
    if label_el is not None:
        label_el.text = title

    dead_kcs = {"_DEAD_", "_OFF_", "XXXXXXX", None}

    target_color: dict[int, str] = {}
    for c in space_combos:
        t_led = next((i for i in c["led_indices"] if i != _COMBO_SPACE_LED), None)
        if t_led is not None:
            target_color[t_led] = _COMBO_SPACE_COLOR
    for c in enter_combos:
        t_led = next((i for i in c["led_indices"] if i != _COMBO_ENTER_LED), None)
        if t_led is not None:
            target_color[t_led] = _COMBO_ENTER_COLOR
    for c in (shortcut_combos or []):
        t_led = next((i for i in c["led_indices"] if i != _COMBO_SC_LED), None)
        if t_led is not None:
            target_color[t_led] = _COMBO_SC_COLOR

    for key in alpha_keys:
        led_idx = key["led_index"]
        group   = root.find(f".//{_T('g')}[@id='key-{led_idx}']")
        if group is None:
            continue
        if key.get("keycode") in dead_kcs:
            continue

        # None => use the key's own label (so the Space anchor reads "Space / Nav",
        # matching the keypair panel); a string => render that literal symbol.
        _anchor_leds = {_COMBO_SPACE_LED: None, _COMBO_ENTER_LED: None}
        if shortcut_combos:
            _anchor_leds[_COMBO_SC_LED] = None
        if led_idx in _anchor_leds:
            anchor_lbl = _anchor_leds[led_idx] or key.get("label", "")
            group.append(_make_text(anchor_lbl, "#36363a"))

        elif led_idx not in target_color:
            raw_lbl = key.get("label", "")
            display = raw_lbl.strip()
            if "\n" not in display and 0 < len(display) <= 5:
                t = ET.SubElement(group, _T("text"))
                t.set("x", "0"); t.set("y", "0")
                t.set("text-anchor", "middle"); t.set("dominant-baseline", "middle")
                t.set("fill", "#36363a"); t.set("style", "font-size:13px;")
                t.text = display

    def _strip_action_prefix(lbl: str) -> str:
        lbl = lbl.replace("\n", "").strip()
        for sym in (_TG_SYM,):
            if lbl.startswith(sym):
                return lbl[len(sym):].strip("  ")
        return lbl
    led_to_alpha_label = {k["led_index"]: _strip_action_prefix(k.get("label", ""))
                          for k in alpha_keys}

    def _anchor_prefix(led: int) -> str:
        """First line of the anchor key's own label, so the trigger text follows
        whatever is actually bound to that thumb (Space/Nav -> 'Space')."""
        lbl = next((k.get("label", "") for k in alpha_keys
                    if k["led_index"] == led), "")
        return lbl.split("\n")[0].strip() or "?"

    for c, thumb_prefix, thumb_led in (
        [(c, _anchor_prefix(_COMBO_SPACE_LED), _COMBO_SPACE_LED) for c in space_combos] +
        [(c, _anchor_prefix(_COMBO_ENTER_LED), _COMBO_ENTER_LED) for c in enter_combos] +
        [(c, _anchor_prefix(_COMBO_SC_LED),  _COMBO_SC_LED)  for c in (shortcut_combos or [])]
    ):
        target = next((i for i in c["led_indices"] if i != thumb_led), None)
        if target is None:
            continue
        group = root.find(f".//{_T('g')}[@id='key-{target}']")
        if group is None:
            continue
        sym_raw = c["action_label"].strip()
        if not sym_raw:
            continue
        sym_lines = sym_raw.split("\n")
        sym_flat  = sym_raw.replace("\n", " ")
        key_lbl = led_to_alpha_label.get(target, "?")
        trigger = f"{thumb_prefix}+{key_lbl}"
        sym_sz = "10px" if len(sym_flat) > 3 else "12px"
        t = ET.SubElement(group, _T("text"))
        t.set("x", "0"); t.set("y", "0")
        t.set("text-anchor", "middle"); t.set("dominant-baseline", "middle")
        ts1 = ET.SubElement(t, _T("tspan"))
        ts1.set("x", "0")
        ts1.set("dy", "-0.9em" if len(sym_lines) > 1 else "-0.55em")
        ts1.set("fill", "#585858"); ts1.set("style", "font-size:9px;")
        # A glyph anchor (the Repeat symbol) renders optically smaller than letters
        # at 9px, so bump just the prefix when it isn't alphanumeric.
        if not thumb_prefix.isalnum():
            big = ET.SubElement(ts1, _T("tspan"))
            big.set("style", "font-size:12px;")
            big.text = thumb_prefix
            big.tail = f"+{key_lbl}"
        else:
            ts1.text = trigger
        for line in sym_lines:
            ts = ET.SubElement(t, _T("tspan"))
            ts.set("x", "0"); ts.set("dy", "1.0em")
            ts.set("fill", "#c8c8c8"); ts.set("style", f"font-size:{sym_sz};")
            ts.text = line

    return root


def _render_combo_bg_panel(template_root: ET.Element, alpha_keys: list[dict],
                           title: str, highlight_leds: set[int] | None = None,
                           highlight_color: str = "#cc5f5f") -> ET.Element:
    """Greyed-out alpha letters as a background for a combo panel (same dim grey as
    the unused letters in the Sym/Num panels) so the combo overlays read clearly on
    top. Keys in ``highlight_leds`` are drawn in ``highlight_color`` instead, to flag
    which keys participate in the combos. Overlays are added separately by the caller."""
    root = copy.deepcopy(template_root)

    label_el = root.find(f".//{_T('text')}[@id='layer-label']")
    if label_el is not None:
        label_el.text = title

    highlight_leds = highlight_leds or set()
    dead_kcs = {"_DEAD_", "_OFF_", "XXXXXXX", None}
    for key in alpha_keys:
        led_idx = key["led_index"]
        group   = root.find(f".//{_T('g')}[@id='key-{led_idx}']")
        if group is None or key.get("keycode") in dead_kcs:
            continue
        label = key.get("label", "")
        if not label:
            continue
        color = highlight_color if led_idx in highlight_leds else "#36363a"
        group.append(_make_text(label, color))

    return root


def _is_gapped_same_side(combo: dict) -> bool:
    """True for a 2-key same-side, same-row combo whose keys are non-adjacent
    (a key is skipped between them, e.g. R·T·S). These overlap the intervening
    key's own combos, so they get their own panel instead of the Same-Side one."""
    idxs = combo.get("led_indices", [])
    if len(idxs) != 2:
        return False
    p0, p1 = POSITIONS[idxs[0]], POSITIONS[idxs[1]]
    if p0["thumb"] or p1["thumb"]:
        return False
    if p0["side"] != p1["side"] or p0["row"] != p1["row"]:
        return False
    return abs(p0["col"] - p1["col"]) >= 2


def _gapped_combo_overlay(combo: dict, key_centers: dict[int, tuple[float, float]],
                          keys_y_off: float) -> str:
    """Box centered between the two gapped keys, labelled `KEY1+KEY2=OUT`."""
    indices = combo.get("led_indices", [])
    positions = [key_centers[i] for i in indices if i in key_centers]
    if len(positions) < 2:
        return ""
    xs = [p[0] for p in positions]
    ys = [p[1] + keys_y_off for p in positions]
    cx = (min(xs) + max(xs)) / 2
    cy = (min(ys) + max(ys)) / 2

    label = combo["action_label"].replace("\n", " ")
    label = label.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

    # Span the box across the skipped key between the two triggers, grazing just
    # the inner edge of each trigger key (inset from their centers toward the gap).
    _INSET = 10
    x1 = min(xs) + _INSET
    x2 = max(xs) - _INSET
    W  = x2 - x1
    H  = _CROSSSIDE_BOX_H
    y1 = cy - H / 2
    color  = "#a0a0a0"
    return "\n".join([
        f'<rect x="{x1:.1f}" y="{y1:.1f}" width="{W:.1f}" height="{H}" '
        f'rx="4" fill="#141416" fill-opacity="0.88" stroke="{color}" stroke-width="0.5"/>',
        f'<text x="{cx:.1f}" y="{cy:.1f}" text-anchor="middle" dominant-baseline="middle" '
        f'fill="{color}" stroke="{color}" stroke-width="0.5" paint-order="stroke fill" '
        f'style="font-size:10px;font-weight:400;">{label}</text>',
    ])


def _parse_action_chip(kc: str | None) -> tuple[str, str, str | None] | None:
    """Return (chip_style, display_name, tap_kc) for action keys, else None.
    style: 'osl' | 'osm' | 'hold' | 'toggle'; tap_kc set only for LT."""
    if not kc:
        return None
    m = re.fullmatch(r"OSL\((\w+)\)", kc)
    if m: return ("osl",    _layer_label(m.group(1)), None)
    m = re.fullmatch(r"MO\((\w+)\)", kc)
    if m: return ("hold",   _layer_label(m.group(1)), None)
    m = re.fullmatch(r"TG\((\w+)\)", kc)
    if m: return ("toggle", _layer_label(m.group(1)), None)
    m = re.fullmatch(r"OSM\((\w+)\)", kc)
    if m: return ("osm",    _mod_label(m.group(1)),   None)
    m = re.fullmatch(r"U_OS_(R(?:SFT|CTL|ALT|GUI))", kc)
    if m: return ("osm",    _mod_label("MOD_" + m.group(1)), None)
    m = re.fullmatch(r"LT\((\w+),\s*([^)]+)\)", kc)
    if m: return ("lt",     _layer_label(m.group(1)), m.group(2).strip())
    if kc == "U_NUM_ENTER":  return ("numenter", "Enter",      None)
    if kc == "U_NUM_SPACE":  return ("toggle",   "Space",      None)
    if kc == "U_OS_SEARCH":  return None
    if kc == "U_SCREENSHOT": return None
    if kc == "U_EMOJIS":     return None
    if kc == "U_TOGGLE_OS":  return None
    if kc == "U_LOCK_SCREEN": return None
    return None


def _make_action_chip(name: str, color: str, y_center: float, style: str) -> ET.Element:
    """Chip inside a key group; shape encodes the action type:
    osl    = rounded rect        (one-shot layer)
    osm    = sharp rect          (one-shot modifier)
    hold   = right-pointing tag  (MO / LT hold)
    toggle = double-chevron      (TG)
    """
    yc = y_center
    g = ET.Element(_T("g"))

    t = ET.SubElement(g, _T("text"))
    t.set("x", "0"); t.set("y", f"{yc:.1f}")
    t.set("text-anchor", "middle"); t.set("dominant-baseline", "middle")
    t.set("fill", color); t.set("style", "font-size:13px")
    lines = name.split("\n")
    if len(lines) == 1:
        t.set("dominant-baseline", "middle")
        t.text = name
        uline_y = yc + 9
    else:
        for i, line in enumerate(lines):
            ts = ET.SubElement(t, _T("tspan"))
            ts.set("x", "0"); ts.set("dy", "-.6em" if i == 0 else "1.2em")
            ts.text = line
        uline_y = yc + 16

    u = ET.SubElement(g, _T("line"))
    u.set("x1", "-18"); u.set("x2", "18")
    u.set("y1", f"{uline_y:.1f}"); u.set("y2", f"{uline_y:.1f}")
    u.set("stroke", _ACTION_COLORS.get(style, color)); u.set("stroke-width", "2.5"); u.set("stroke-linecap", "square")

    return g

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

        resolved_kc = key.get("resolved_keycode")
        action = _parse_action_chip(resolved_kc)

        if action:
            style, display_name, tap_kc = action
            if tap_kc:
                tap_el = _make_text(map_key(tap_kc), text_color)
                tap_el.set("y", "-7")
                group.append(tap_el)
                group.append(_make_action_chip(display_name, text_color, y_center=14.0, style=style))
            else:
                group.append(_make_action_chip(display_name, text_color, y_center=2.0, style=style))
        else:
            group.append(_make_text(label, text_color))

    return root

_COMBO_INDENT   = 10   # inset from each key's inner edge toward its center
_COMBO_HALF_MIN = 10   # minimum half-size, keeps box visible for same-row combos

def _extract_key_centers(template_root: ET.Element) -> tuple[dict[int, tuple[float, float]], float]:
    """Returns ({led_index: (cx, cy_in_keys_group)}, keys_group_y_offset)."""
    centers: dict[int, tuple[float, float]] = {}
    keys_g = template_root.find(f".//{_T('g')}[@id='keys']")
    if keys_g is None:
        return centers, 56.0
    m = re.search(r"translate\([^,]*,([^)]+)\)", keys_g.get("transform", ""))
    y_off = float(m.group(1)) if m else 56.0
    for child in keys_g:
        gid = child.get("id", "")
        if not gid.startswith("key-"):
            continue
        try:
            idx = int(gid[4:])
        except ValueError:
            continue
        m2 = re.search(r"translate\(([^,]+),([^)]+)\)", child.get("transform", ""))
        if m2:
            centers[idx] = (float(m2.group(1)), float(m2.group(2)))
    return centers, y_off


def _combo_overlay(combo: dict, key_centers: dict[int, tuple[float, float]], keys_y_off: float) -> str:
    indices = combo.get("led_indices", [])
    if len(indices) < 2:
        return ""
    positions = [key_centers[i] for i in indices if i in key_centers]
    if len(positions) < 2:
        return ""

    xs = [p[0] for p in positions]
    ys = [p[1] + keys_y_off for p in positions]

    cx_c = (min(xs) + max(xs)) / 2
    cy_c = (min(ys) + max(ys)) / 2

    # shrink inward from each key's inner edge; clamp to minimum box size
    x1 = min(min(xs) + _COMBO_INDENT, cx_c - _COMBO_HALF_MIN)
    x2 = max(max(xs) - _COMBO_INDENT, cx_c + _COMBO_HALF_MIN)
    y1 = min(min(ys) + _COMBO_INDENT, cy_c - _COMBO_HALF_MIN)
    y2 = max(max(ys) - _COMBO_INDENT, cy_c + _COMBO_HALF_MIN)

    w, h   = x2 - x1, y2 - y1
    cx, cy = (x1 + x2) / 2, (y1 + y2) / 2

    parsed = _parse_action_chip(combo.get("resolved_action") or combo.get("action"))
    if parsed:
        chip_style, display_name, _ = parsed
        label = display_name
    else:
        chip_style = None
        label = combo["action_label"].replace("\n", " ")
    label = label.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

    _ULINE_STYLES = {"osl", "osm", "hold", "lt", "toggle", "numenter"}
    color       = "#a0a0a0"
    uline_color = _ACTION_COLORS.get(chip_style, color) if chip_style in _ULINE_STYLES else None
    sz = "7px" if len(label) > 6 else "10px"
    parts = [
        f'<rect x="{x1:.1f}" y="{y1:.1f}" width="{w:.1f}" height="{h:.1f}" '
        f'rx="4" fill="#141416" fill-opacity="0.88" stroke="{color}" stroke-width="0.5"/>',
        f'<text x="{cx:.1f}" y="{cy:.1f}" text-anchor="middle" dominant-baseline="middle" '
        f'fill="{color}" stroke="{color}" stroke-width="0.5" paint-order="stroke fill" '
        f'style="font-size:{sz};font-weight:400;">{label}</text>',
    ]
    if uline_color:
        uy  = cy + 7
        uhw = w / 2 - 6
        parts.append(
            f'<line x1="{cx - uhw:.1f}" y1="{uy:.1f}" x2="{cx + uhw:.1f}" y2="{uy:.1f}" '
            f'stroke="{uline_color}" stroke-width="2.0" stroke-linecap="square"/>'
        )
    return "\n".join(parts)


_THUMB_LEDS      = {24, 25, 50, 51}
_CROSSSIDE_BOX_W = 46
_CROSSSIDE_BOX_H = 20
_CROSSSIDE_BOX_PAD = 12  # horizontal padding inside box (total)
_CHAR_W_10PX     = 6.0   # estimated character width at 10px
# y-route levels (in layer coords) for cross-side non-thumb combos, top-down
_NONTHUMB_ROUTE_YS = [15, 42]
_ROUTE_X_LEFT  =   5.0   # x for left-side outer vertical leg
_ROUTE_X_RIGHT = 893.0   # x for right-side outer vertical leg

def _crossside_box_w(label: str) -> float:
    """Expand box width if label is wider than the default; otherwise keep default."""
    needed = len(label) * _CHAR_W_10PX + _CROSSSIDE_BOX_PAD
    return max(_CROSSSIDE_BOX_W, needed)

# Offset from each thumb key's center to its inner-top corner (the corner facing the gap).
# Derived from the key's rotation and rect half-sizes.
_THUMB_INNER_CORNER: dict[int, tuple[float, float]] = {
    24: ( 35.52,  -9.52),  # left  outer, 52×52, rot  30°
    25: ( 41.02, -19.04),  # left  inner, 52×74, rot  30°
    50: (-41.02, -19.04),  # right inner, 52×74, rot -30°
    51: (-35.52,  -9.52),  # right outer, 52×52, rot -30°
}

def _crossside_thumb_overlay(combo: dict, key_centers: dict[int, tuple[float, float]], keys_y_off: float) -> str:
    indices = combo.get("led_indices", [])
    if len(indices) < 2 or not all(i in _THUMB_LEDS for i in indices):
        return ""

    corners = []
    for i in indices:
        if i not in key_centers or i not in _THUMB_INNER_CORNER:
            return ""
        kx, ky = key_centers[i]
        dx, dy = _THUMB_INNER_CORNER[i]
        corners.append((kx + dx, ky + keys_y_off + dy))

    cx = sum(p[0] for p in corners) / len(corners)
    cy = sum(p[1] for p in corners) / len(corners)

    # Determine label first so box width can adapt
    parsed = _parse_action_chip(combo.get("resolved_action") or combo.get("action"))
    if parsed:
        chip_style, display_name, _ = parsed
        if display_name == LAYER_LABELS.get("NUM", "Num"):
            chip_style = "toggle"
        label = display_name
    else:
        chip_style = "osl"
        label = combo["action_label"].replace("\n", " ")
    label = label.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

    W, H   = _crossside_box_w(label), _CROSSSIDE_BOX_H
    hw, hh = W / 2, H / 2
    bx1, bx2 = cx - hw, cx + hw
    by1       = cy - hh

    color       = "#a0a0a0"
    uline_color = _ACTION_COLORS.get(chip_style, color)
    parts = []

    for kx, ky in corners:
        ex = bx1 if kx < cx else bx2
        parts.append(
            f'<line x1="{kx:.1f}" y1="{ky:.1f}" x2="{ex:.1f}" y2="{cy:.1f}" '
            f'stroke="{color}" stroke-width="0.5" stroke-dasharray="3,2"/>'
        )

    parts.append(
        f'<rect x="{bx1:.1f}" y="{by1:.1f}" width="{W:.1f}" height="{H}" '
        f'rx="4" fill="#141416" fill-opacity="0.88" stroke="{color}" stroke-width="0.5"/>'
    )
    parts.append(
        f'<text x="{cx:.1f}" y="{cy:.1f}" text-anchor="middle" dominant-baseline="middle" '
        f'fill="{color}" stroke="{color}" stroke-width="0.5" paint-order="stroke fill" '
        f'style="font-size:10px;font-weight:400;">{label}</text>'
    )
    uy = cy + 7
    uhw = W / 2 - 6
    parts.append(
        f'<line x1="{cx - uhw:.1f}" y1="{uy:.1f}" x2="{cx + uhw:.1f}" y2="{uy:.1f}" '
        f'stroke="{uline_color}" stroke-width="2.0" stroke-linecap="square"/>'
    )
    return "\n".join(parts)


def _crossside_nonthumb_overlay(combo: dict, key_centers: dict[int, tuple[float, float]],
                                keys_y_off: float, y_route: float, lines_only: bool = False) -> str:
    indices = combo.get("led_indices", [])
    if len(indices) < 2:
        return ""
    positions = [(key_centers[i][0], key_centers[i][1] + keys_y_off)
                 for i in indices if i in key_centers]
    if len(positions) < 2:
        return ""

    cx = sum(p[0] for p in positions) / len(positions)

    parsed = _parse_action_chip(combo.get("resolved_action") or combo.get("action"))
    if parsed:
        chip_style, label, _ = parsed
    else:
        chip_style = "osl"
        label = combo["action_label"].replace("\n", " ")
    label = label.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

    W, H = _crossside_box_w(label), _CROSSSIDE_BOX_H
    hw, hh = W / 2, H / 2
    bx1, bx2 = cx - hw, cx + hw
    by1 = y_route - hh

    color       = "#a0a0a0"
    uline_color = _ACTION_COLORS.get(chip_style, color)
    parts = []

    dash = f'stroke="{color}" stroke-width="0.5" stroke-dasharray="3,2"'
    for kx, ky in positions:
        hx = bx1 if kx < cx else bx2
        ky_start = ky - 13
        kx_start = kx + 13 if kx < cx else kx - 13
        parts.append(f'<line x1="{kx_start:.1f}" y1="{ky_start:.1f}" x2="{kx_start:.1f}" y2="{y_route:.1f}" {dash}/>')
        parts.append(f'<line x1="{kx_start:.1f}" y1="{y_route:.1f}" x2="{hx:.1f}" y2="{y_route:.1f}" {dash}/>')

    if lines_only:
        return "\n".join(parts)

    parts.append(
        f'<rect x="{bx1:.1f}" y="{by1:.1f}" width="{W:.1f}" height="{H}" '
        f'rx="4" fill="#141416" fill-opacity="0.88" stroke="{color}" stroke-width="0.5"/>'
    )
    parts.append(
        f'<text x="{cx:.1f}" y="{y_route:.1f}" text-anchor="middle" dominant-baseline="middle" '
        f'fill="{color}" stroke="{color}" stroke-width="0.5" paint-order="stroke fill" '
        f'style="font-size:10px;font-weight:400;">{label}</text>'
    )
    uy = y_route + 7
    uhw = W / 2 - 6
    parts.append(
        f'<line x1="{cx - uhw:.1f}" y1="{uy:.1f}" x2="{cx + uhw:.1f}" y2="{uy:.1f}" '
        f'stroke="{uline_color}" stroke-width="2.0" stroke-linecap="square"/>'
    )
    return "\n".join(parts)


def _render_legend(canvas_w: int, margin: int) -> str:
    items = [
        ("osl",    "One-shot layer (OSL)"),
        ("osm",    "One-shot modifier (OSM)"),
        ("hold",   "Momentary (MO)"),
        ("lt",     "Layer-tap (LT)"),
        ("toggle", "Toggle (TG)"),
    ]
    pad_x    = 12
    pad_y    = 10
    row_h    = 18
    swatch   = 24
    gap      = 10
    title_h  = 20
    box_h    = pad_y * 2 + title_h + row_h * len(items)

    parts = [
        f'<g transform="translate({margin},12)">',
        f'<rect x="0" y="0" width="210" height="{box_h}" rx="4" '
        f'fill="#141416" fill-opacity="0.8" stroke="#404040" stroke-width="0.5"/>',
        f'<text x="{pad_x}" y="{pad_y + title_h / 2:.1f}" '
        f'style="text-anchor:start;dominant-baseline:middle;font-size:11px;font-weight:bold;" '
        f'fill="#ffffff">Legend</text>',
    ]
    for i, (style, label) in enumerate(items):
        color = _ACTION_COLORS[style]
        y = pad_y + title_h + i * row_h + row_h / 2
        parts.append(
            f'<line x1="{pad_x}" y1="{y:.1f}" x2="{pad_x + swatch}" y2="{y:.1f}" '
            f'stroke="{color}" stroke-width="2.5" stroke-linecap="square"/>'
        )
        parts.append(
            f'<text x="{pad_x + swatch + gap}" y="{y:.1f}" '
            f'style="text-anchor:start;dominant-baseline:middle;font-size:10px;" '
            f'fill="#c0c0c0">{label}</text>'
        )
    parts.append('</g>')
    return "\n".join(parts)


_RENDER_ORDER = ["ALPHA", "NAV", "MOUSE", "SYS", "FUNC", "NUM"]

def render_svg(ir: dict) -> str:
    layers  = sorted(ir["layers"],
                     key=lambda l: _RENDER_ORDER.index(l["name"])
                     if l["name"] in _RENDER_ORDER else len(_RENDER_ORDER))
    combos  = ir["combos"]
    palette = ir["color_palette"]
    template_root = ET.parse(TEMPLATE_PATH).getroot()
    key_centers, keys_y_off = _extract_key_centers(template_root)

    shortcut_combos    = _classify_shortcut_combos(combos)
    _shortcut_ids      = {id(c) for c in shortcut_combos}
    same_side_combos   = [c for c in combos if c.get("side") in ("left", "right")
                          and not c.get("thumb_key_combo")
                          and id(c) not in _shortcut_ids
                          and not _is_gapped_same_side(c)]
    gapped_combos      = [c for c in combos if c.get("side") in ("left", "right")
                          and not c.get("thumb_key_combo")
                          and id(c) not in _shortcut_ids
                          and _is_gapped_same_side(c)]
    crossside_thumb_combos = [c for c in combos if c.get("side") == "both"
                              and all(i in _THUMB_LEDS for i in c.get("led_indices", []))]
    # Non-thumb cross-side combos sorted by highest key (smallest template y) first
    _raw_nonthumb = [c for c in combos if c.get("side") == "both"
                     and not all(i in _THUMB_LEDS for i in c.get("led_indices", []))
                     and not c.get("thumb_key_combo")
                     and id(c) not in _shortcut_ids
                     and not c.get("thumb_3key_combo")]
    crossside_nonthumb_combos = sorted(
        _raw_nonthumb,
        key=lambda c: min(key_centers[i][1] for i in c["led_indices"] if i in key_centers)
    )

    alpha_layer = next((l for l in layers if l["name"] == "ALPHA"), None)
    space_sym, enter_sym, space_num, enter_num = (
        _classify_thumb_combos(combos) if alpha_layer else ([], [], [], [])
    )
    space_sym_3k, enter_sym_3k, space_num_3k, enter_num_3k = (
        _classify_thumb_3key_combos(combos) if alpha_layer else ([], [], [], [])
    )
    combo_panels = [
        ("Alpha · Sym Thumb Combos", space_sym,   enter_sym,   None),
        ("Alpha · Num Thumb Combos", space_num,   enter_num,   None),
        ("Alpha · Shortcut Combos",  [],          [],          shortcut_combos),
    ]
    combo_panels_3k = [
        (space_sym_3k, enter_sym_3k),
        (space_num_3k, enter_num_3k),
        ([], []),
    ]

    n_combo_panels = (len(combo_panels) + 3) if alpha_layer else 0
    total_h = (len(layers) + n_combo_panels) * (_LAYER_H + _LAYER_GAP) + 40 + _LEGEND_H
    parts = [
        f'<svg width="{_CANVAS_W}" height="{total_h}" viewBox="0 0 {_CANVAS_W} {total_h}" '
        f'class="keymap" xmlns="http://www.w3.org/2000/svg">',
        f'<style>{_CSS}</style>',
        _render_legend(_CANVAS_W, _MARGIN),
    ]

    panel_offset = 0
    for i, layer in enumerate(layers):
        y    = _LEGEND_H + (i + panel_offset) * (_LAYER_H + _LAYER_GAP)
        root = _fill_layer(template_root, layer, palette)
        parts.append(f'<g transform="translate({_MARGIN},{y})">')
        is_alpha = layer["name"] == "ALPHA"
        layer_nonthumb_idx = 0
        for combo in crossside_nonthumb_combos:
            if layer["name"] in combo["layers"] and not is_alpha:
                y_route = _NONTHUMB_ROUTE_YS[layer_nonthumb_idx % len(_NONTHUMB_ROUTE_YS)]
                layer_nonthumb_idx += 1
                frag = _crossside_nonthumb_overlay(combo, key_centers, keys_y_off, y_route, lines_only=True)
                if frag:
                    parts.append(frag)
        for child in root:
            parts.append(ET.tostring(child, encoding="unicode"))
        for combo in same_side_combos:
            if layer["name"] in combo["layers"] and not is_alpha:
                frag = _combo_overlay(combo, key_centers, keys_y_off)
                if frag:
                    parts.append(frag)
        for combo in crossside_thumb_combos:
            if layer["name"] in combo["layers"] and not is_alpha:
                frag = _crossside_thumb_overlay(combo, key_centers, keys_y_off)
                if frag:
                    parts.append(frag)
        layer_nonthumb_idx = 0
        for combo in crossside_nonthumb_combos:
            if layer["name"] in combo["layers"] and not is_alpha:
                y_route = _NONTHUMB_ROUTE_YS[layer_nonthumb_idx % len(_NONTHUMB_ROUTE_YS)]
                layer_nonthumb_idx += 1
                frag = _crossside_nonthumb_overlay(combo, key_centers, keys_y_off, y_route)
                if frag:
                    parts.append(frag)
        parts.append("</g>")

    if alpha_layer:
        # Same-Side Combos — neighboring-key rolls (one hand), over greyed alpha letters.
        panel_offset += 1
        yp = _LEGEND_H + (len(layers) - 1 + panel_offset) * (_LAYER_H + _LAYER_GAP)
        ss_root = _render_combo_bg_panel(template_root, alpha_layer["keys"], "Alpha · Same-Side Combos")
        parts.append(f'<g transform="translate({_MARGIN},{yp})">')
        for child in ss_root:
            parts.append(ET.tostring(child, encoding="unicode"))
        for combo in same_side_combos:
            if alpha_layer["name"] in combo["layers"]:
                frag = _combo_overlay(combo, key_centers, keys_y_off)
                if frag:
                    parts.append(frag)
        parts.append("</g>")

        # Gapped Same-Side Combos — same row, non-adjacent keys (a key skipped
        # between them). Box labels the trigger + output; the two trigger keys are
        # reddened so it's clear which keys form the combo across the gap.
        panel_offset += 1
        yp = _LEGEND_H + (len(layers) - 1 + panel_offset) * (_LAYER_H + _LAYER_GAP)
        gap_leds = {i for c in gapped_combos for i in c["led_indices"]}
        gp_root = _render_combo_bg_panel(template_root, alpha_layer["keys"],
                                         "Alpha · Gapped Same-Side Combos",
                                         highlight_leds=gap_leds,
                                         highlight_color="#c8c8c8")
        parts.append(f'<g transform="translate({_MARGIN},{yp})">')
        for child in gp_root:
            parts.append(ET.tostring(child, encoding="unicode"))
        for combo in gapped_combos:
            if alpha_layer["name"] in combo["layers"]:
                frag = _gapped_combo_overlay(combo, key_centers, keys_y_off)
                if frag:
                    parts.append(frag)
        parts.append("</g>")

        # Crossover Combos — two-thumb (NumWord/Func) and cross-hand combos, whose
        # routing lines would otherwise add noise over the same-side boxes.
        panel_offset += 1
        yp = _LEGEND_H + (len(layers) - 1 + panel_offset) * (_LAYER_H + _LAYER_GAP)
        xo_root = _render_combo_bg_panel(template_root, alpha_layer["keys"], "Alpha · Crossover Combos")
        parts.append(f'<g transform="translate({_MARGIN},{yp})">')
        xo_nonthumb_idx = 0
        for combo in crossside_nonthumb_combos:
            if alpha_layer["name"] in combo["layers"]:
                y_route = _NONTHUMB_ROUTE_YS[xo_nonthumb_idx % len(_NONTHUMB_ROUTE_YS)]
                xo_nonthumb_idx += 1
                frag = _crossside_nonthumb_overlay(combo, key_centers, keys_y_off, y_route, lines_only=True)
                if frag:
                    parts.append(frag)
        for child in xo_root:
            parts.append(ET.tostring(child, encoding="unicode"))
        for combo in crossside_thumb_combos:
            if alpha_layer["name"] in combo["layers"]:
                frag = _crossside_thumb_overlay(combo, key_centers, keys_y_off)
                if frag:
                    parts.append(frag)
        xo_nonthumb_idx = 0
        for combo in crossside_nonthumb_combos:
            if alpha_layer["name"] in combo["layers"]:
                y_route = _NONTHUMB_ROUTE_YS[xo_nonthumb_idx % len(_NONTHUMB_ROUTE_YS)]
                xo_nonthumb_idx += 1
                frag = _crossside_nonthumb_overlay(combo, key_centers, keys_y_off, y_route)
                if frag:
                    parts.append(frag)
        parts.append("</g>")

        for panel_idx, (title, space_c, enter_c, shortcut_c) in enumerate(combo_panels):
            panel_offset += 1
            yp = _LEGEND_H + (len(layers) - 1 + panel_offset) * (_LAYER_H + _LAYER_GAP)
            panel_root = _render_dual_combo_panel(
                template_root, title, space_c, enter_c, alpha_layer["keys"],
                shortcut_combos=shortcut_c
            )
            parts.append(f'<g transform="translate({_MARGIN},{yp})">')
            for child in panel_root:
                parts.append(ET.tostring(child, encoding="unicode"))
            space_3k, enter_3k = combo_panels_3k[panel_idx]
            for c in space_3k:
                frag = _3key_thumb_combo_overlay(c, key_centers, keys_y_off, _COMBO_SPACE_COLOR, _COMBO_SPACE_LED)
                if frag:
                    parts.append(frag)
            for c in enter_3k:
                frag = _3key_thumb_combo_overlay(c, key_centers, keys_y_off, _COMBO_ENTER_COLOR, _COMBO_ENTER_LED)
                if frag:
                    parts.append(frag)
            parts.append("</g>")

    parts.append("</svg>")
    return "\n".join(parts)

# ── entry point ───────────────────────────────────────────────────────────────

SVG_OUT = ROOT / "assets" / "keymap.svg"

if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument("--json", action="store_true", help="dump IR as JSON to stdout instead")
    p.add_argument("--svg", metavar="FILE", help="write SVG to FILE instead of the default assets/keymap.svg")
    args = p.parse_args()

    ir = build_ir()
    if args.json:
        json.dump(ir, sys.stdout, indent=2)
        print()
    else:
        out = Path(args.svg) if args.svg else SVG_OUT
        out.write_text(render_svg(ir))
        print(f"wrote {out}", file=sys.stderr)
