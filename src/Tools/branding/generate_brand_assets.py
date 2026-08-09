# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright (c) 2026 FuCad contributors

"""Regenerate every FuCad brand asset from the definitions in ``brand.py``.

Run it from anywhere after changing the palette or the mark geometry::

    python src/Tools/branding/generate_brand_assets.py

The generated files are committed, so this only needs to run when the identity
itself changes.
"""

import argparse
from pathlib import Path

from PIL import Image, ImageDraw

from brand import (
    AMBER,
    AMBER_LIT,
    INK,
    MARK_ASPECT,
    MIST,
    PAPER,
    STEEL,
    STEEL_DEEP,
    STEEL_LIT,
    TAGLINE,
    draw_tracked,
    hex_of,
    isometric_grid,
    linear_gradient,
    load_font,
    mark_polygons,
    radial_glow,
    render_polygons,
    rgba,
    rounded_rect_mask,
    rounded_rect_ring,
    svg_document,
    svg_path,
    tracked_width,
    write_icns,
    write_ico,
)

WORDMARK = "FuCad"
ATTRIBUTION = "Fork of FreeCAD  \u00b7  LGPL-2.1-or-later"

ICON_SIZES = (16, 24, 32, 48, 64, 128, 256)


# --------------------------------------------------------------------------
# Icons
# --------------------------------------------------------------------------


def app_icon(size):
    """The FuCad application icon: the extruded mark on a steel plate."""
    canvas = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    plate_mask = rounded_rect_mask((size, size), size * 0.215)
    canvas.paste(linear_gradient((size, size), STEEL_LIT, STEEL_DEEP, 55.0), (0, 0), plate_mask)

    if size >= 48:
        ring = rounded_rect_ring((size, size), size * 0.215, max(1.0, size / 64.0))
        highlight = Image.new("RGBA", (size, size), rgba(PAPER, 38))
        canvas.paste(highlight, (0, 0), ring)

    inset = size * 0.175
    shapes = mark_polygons(
        (inset, inset, size - 2 * inset, size - 2 * inset),
        rgba(PAPER),
        rgba(AMBER_LIT),
        rgba(AMBER),
    )
    canvas.alpha_composite(render_polygons((size, size), shapes))
    return canvas


# A sheet of paper with a folded corner, drawn in a 64 unit box. The border is
# a second, slightly larger sheet behind the first so the icon still has an
# edge when it lands on a white file manager background.
PAGE_BORDER = ((12.0, 4.0), (41.0, 4.0), (53.0, 16.0), (53.0, 60.0), (12.0, 60.0))
PAGE = ((13.5, 5.5), (40.38, 5.5), (51.5, 16.62), (51.5, 58.5), (13.5, 58.5))
PAGE_FOLD = ((40.38, 5.5), (51.5, 16.62), (40.38, 16.62))
PROMPT_CHEVRON = (
    (18.0, 24.0),
    (23.0, 24.0),
    (33.0, 36.0),
    (23.0, 48.0),
    (18.0, 48.0),
    (28.0, 36.0),
)
PROMPT_RULE = ((34.0, 43.0), (48.0, 43.0), (48.0, 48.0), (34.0, 48.0))


def _page_shapes(scale):
    def place(polygon):
        return [(x * scale, y * scale) for x, y in polygon]

    return [
        (place(PAGE_BORDER), rgba(STEEL)),
        (place(PAGE), rgba(PAPER)),
        (place(PAGE_FOLD), rgba(AMBER)),
    ]


def document_icon(size):
    """Icon for ``.FCStd`` documents: the mark on a sheet of paper."""
    scale = size / 64.0
    shapes = _page_shapes(scale)
    shapes += mark_polygons(
        (17.0 * scale, 21.0 * scale, 31.0 * scale, 31.0 * scale),
        rgba(STEEL),
        rgba(AMBER_LIT),
        rgba(AMBER),
    )
    return render_polygons((size, size), shapes)


def script_icon(size):
    """Icon for ``.FCMacro`` scripts: a sheet of paper with a prompt."""
    scale = size / 64.0

    def place(polygon):
        return [(x * scale, y * scale) for x, y in polygon]

    shapes = _page_shapes(scale)
    shapes.append((place(PROMPT_CHEVRON), rgba(STEEL)))
    shapes.append((place(PROMPT_RULE), rgba(AMBER)))
    return render_polygons((size, size), shapes)


# --------------------------------------------------------------------------
# Shared atmosphere for the wide brand surfaces
# --------------------------------------------------------------------------


def _steel_backdrop(size, spacing, glow_at=(0.28, 0.42)):
    """Deep steel gradient with construction lines and a warm accent glow."""
    width, height = size
    canvas = linear_gradient(size, STEEL_LIT, (5, 26, 33), 52.0).convert("RGBA")
    canvas.alpha_composite(isometric_grid(size, spacing, PAPER, 13))
    canvas.alpha_composite(
        radial_glow(size, (width * glow_at[0], height * glow_at[1]), width * 0.55, AMBER, 24)
    )
    return canvas


def _ghost_mark(canvas, centre, height, alpha=(16, 26, 10)):
    """Composite an oversized, barely-there mark used as a watermark.

    ``centre`` and ``height`` are fractions of the canvas, so the watermark
    keeps the same weight whatever surface it lands on.
    """
    canvas_width, canvas_height = canvas.size
    tall = canvas_height * height
    wide = tall * MARK_ASPECT
    box = (canvas_width * centre[0] - wide / 2, canvas_height * centre[1] - tall / 2, wide, tall)
    shapes = mark_polygons(box, rgba(PAPER, alpha[0]), rgba(PAPER, alpha[1]), rgba(PAPER, alpha[2]))
    canvas.alpha_composite(render_polygons(canvas.size, shapes))


def _lockup(canvas, origin, scale, wordmark_size, tagline_size, ink=PAPER, subtle=MIST):
    """Draw the plate icon, the wordmark, the accent rule and the tagline."""
    draw = ImageDraw.Draw(canvas)
    plate_size = int(round(wordmark_size * 1.45))
    plate = app_icon(plate_size)
    plate_y = int(round(origin[1] - wordmark_size * 0.98))
    canvas.alpha_composite(plate, (int(round(origin[0])), plate_y))

    text_x = origin[0] + plate_size + wordmark_size * 0.42
    word_font = load_font(int(round(wordmark_size)), "SemiBold")
    tracking = wordmark_size * 0.012
    baseline = origin[1] + wordmark_size * 0.06
    width = draw_tracked(draw, (text_x, baseline), WORDMARK, word_font, rgba(ink), tracking)

    rule_y = baseline + wordmark_size * 0.24
    draw.rectangle(
        (text_x, rule_y, text_x + width * 0.62, rule_y + max(2.0, 3.0 * scale)),
        fill=rgba(AMBER),
    )

    tag_font = load_font(int(round(tagline_size)), "Regular")
    draw_tracked(
        draw,
        (text_x + 1.0, rule_y + tagline_size * 2.05),
        TAGLINE,
        tag_font,
        rgba(subtle),
        tagline_size * 0.26,
    )


def splash(scale=1):
    """Startup splash screen; the version string is painted on it at runtime."""
    size = (640 * scale, 400 * scale)
    canvas = _steel_backdrop(size, 30 * scale)
    _ghost_mark(canvas, (0.77, 0.50), 0.86)

    _lockup(canvas, (52 * scale, 150 * scale), scale, 72 * scale, 14 * scale)

    draw = ImageDraw.Draw(canvas)
    footer = load_font(int(round(12 * scale)), "Regular")
    draw_tracked(
        draw,
        (52 * scale, 382 * scale),
        ATTRIBUTION,
        footer,
        rgba(MIST, 205),
        0.9 * scale,
    )
    return canvas


def about_image(development):
    """Banner shown at the top of the About dialog."""
    size = (552, 189)
    canvas = _steel_backdrop(size, 22, glow_at=(0.22, 0.5))
    _ghost_mark(canvas, (0.86, 0.50), 1.05)

    _lockup(canvas, (32, 96), 1.0, 52, 12)

    draw = ImageDraw.Draw(canvas)
    footer = load_font(11, "Regular")
    draw_tracked(draw, (34, 170), ATTRIBUTION, footer, rgba(MIST, 205), 0.8)

    if development:
        chip_font = load_font(12, "SemiBold")
        label = "DEVELOPMENT BUILD"
        tracking = 1.6
        width = tracked_width(chip_font, label, tracking)
        left, top = size[0] - width - 46, 22
        draw.rounded_rectangle(
            (left, top, left + width + 24, top + 26), radius=13, fill=rgba(AMBER)
        )
        draw_tracked(draw, (left + 12, top + 18), label, chip_font, rgba(INK), tracking)

    return canvas


def readme_banner():
    """Wide lockup used as the project hero image in the README."""
    size = (1200, 300)
    canvas = _steel_backdrop(size, 34, glow_at=(0.24, 0.5))
    _ghost_mark(canvas, (0.84, 0.50), 1.15)
    _lockup(canvas, (88, 158), 1.0, 96, 19)
    return canvas


# --------------------------------------------------------------------------
# Packaging graphics
# --------------------------------------------------------------------------


def installer_banner():
    """Left-hand panel of the Windows installer wizard."""
    size = (164, 314)
    canvas = _steel_backdrop(size, 24, glow_at=(0.5, 0.28))
    _ghost_mark(canvas, (0.50, 0.82), 0.52)

    plate = app_icon(76)
    canvas.alpha_composite(plate, ((size[0] - 76) // 2, 52))

    draw = ImageDraw.Draw(canvas)
    word_font = load_font(34, "SemiBold")
    tracking = 0.6
    width = tracked_width(word_font, WORDMARK, tracking)
    left = (size[0] - width) / 2
    draw_tracked(draw, (left, 172), WORDMARK, word_font, rgba(PAPER), tracking)
    draw.rectangle((left, 184, left + width, 187), fill=rgba(AMBER))

    tag_font = load_font(9, "Regular")
    for index, line in enumerate(("PARAMETRIC 3D", "MODELING")):
        line_width = tracked_width(tag_font, line, 2.0)
        draw_tracked(
            draw,
            ((size[0] - line_width) / 2, 208 + index * 14),
            line,
            tag_font,
            rgba(MIST),
            2.0,
        )
    return canvas.convert("RGB")


def installer_header():
    """Small logo strip shown in the installer header, on a light background."""
    size = (150, 57)
    canvas = Image.new("RGBA", size, rgba(PAPER))
    canvas.alpha_composite(app_icon(40), (104, 9))

    draw = ImageDraw.Draw(canvas)
    word_font = load_font(21, "SemiBold")
    tracking = 0.4
    width = tracked_width(word_font, WORDMARK, tracking)
    draw_tracked(draw, (96 - width, 35), WORDMARK, word_font, rgba(INK), tracking)
    draw.rectangle((96 - width, 41, 96, 43), fill=rgba(AMBER))
    return canvas.convert("RGB")


def dmg_background():
    """Backdrop for the macOS disk image, sized around the Finder icon layout."""
    size = (640, 347)
    canvas = linear_gradient(size, (255, 255, 255), (226, 235, 238), 70.0).convert("RGBA")
    canvas.alpha_composite(isometric_grid(size, 26, STEEL, 16))

    canvas.alpha_composite(app_icon(52), (34, 26))
    draw = ImageDraw.Draw(canvas)
    word_font = load_font(30, "SemiBold")
    width = draw_tracked(draw, (100, 62), WORDMARK, word_font, rgba(INK), 0.5)
    tag_font = load_font(11, "Regular")
    draw_tracked(draw, (102, 82), TAGLINE, tag_font, rgba(STEEL), 2.4)

    # Finder places the application at x=250 and the Applications link at x=475,
    # so the arrow points across the gap between them.
    arrow_y = 150
    draw.rectangle((330, arrow_y - 4, 388, arrow_y + 4), fill=rgba(AMBER))
    draw.polygon(
        [(384, arrow_y - 18), (410, arrow_y), (384, arrow_y + 18)],
        fill=rgba(AMBER),
    )

    hint_font = load_font(13, "Regular")
    hint = "DRAG FUCAD INTO YOUR APPLICATIONS FOLDER"
    hint_width = tracked_width(hint_font, hint, 2.2)
    draw_tracked(draw, ((size[0] - hint_width) / 2, 296), hint, hint_font, rgba(STEEL), 2.2)
    return canvas.convert("RGB")


# --------------------------------------------------------------------------
# Vector masters
# --------------------------------------------------------------------------


def app_icon_svg():
    """Vector master of the application icon."""
    inset = 48 * 0.175
    shapes = mark_polygons((inset, inset, 48 - 2 * inset, 48 - 2 * inset), PAPER, AMBER_LIT, AMBER)
    faces = "\n".join(
        f'  <path d="{svg_path(points)}" fill="{hex_of(colour)}"/>' for points, colour in shapes
    )
    body = (
        "  <defs>\n"
        '    <linearGradient id="plate" x1="0" y1="0" x2="1" y2="1">\n'
        f'      <stop offset="0" stop-color="{hex_of(STEEL_LIT)}"/>\n'
        f'      <stop offset="1" stop-color="{hex_of(STEEL_DEEP)}"/>\n'
        "    </linearGradient>\n"
        "  </defs>\n"
        '  <rect x="0" y="0" width="48" height="48" rx="10.3" ry="10.3" fill="url(#plate)"/>\n'
        '  <rect x="0.5" y="0.5" width="47" height="47" rx="9.8" ry="9.8" fill="none"'
        f' stroke="{hex_of(PAPER)}" stroke-opacity="0.15" stroke-width="1"/>\n'
        f"{faces}"
    )
    return svg_document(48, 48, body, "FuCad")


def document_icon_svg():
    """Vector master of the ``.FCStd`` document icon."""
    shapes = [(PAGE_BORDER, STEEL), (PAGE, PAPER), (PAGE_FOLD, AMBER)]
    shapes += mark_polygons((17.0, 21.0, 31.0, 31.0), STEEL, AMBER_LIT, AMBER)
    body = "\n".join(
        f'  <path d="{svg_path(points)}" fill="{hex_of(colour)}"/>' for points, colour in shapes
    )
    return svg_document(64, 64, body, "FuCad document")


def background_svg(colour, opacity):
    """Vector master of the watermark shown behind an empty workspace."""
    width, height = 98.0, 102.0
    shapes = mark_polygons((0.0, 0.0, width, height), colour, colour, colour)
    faces = "\n".join(f'    <path d="{svg_path(points)}"/>' for points, _ in shapes)
    body = f'  <g fill="{hex_of(colour)}" opacity="{opacity}">\n{faces}\n  </g>'
    return svg_document(round(width), round(height), body, "FuCad")


# --------------------------------------------------------------------------
# Output
# --------------------------------------------------------------------------


class Writer:
    """Writes generated assets below ``repo`` and reports what it produced."""

    def __init__(self, repo):
        self.repo = repo
        self.count = 0

    def _prepare(self, name):
        path = self.repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        return path

    def _done(self, path):
        self.count += 1
        print(f"  {path.relative_to(self.repo).as_posix()}")

    def image(self, name, image):
        path = self._prepare(name)
        if path.suffix == ".bmp":
            image.convert("RGB").save(path, format="BMP")
        else:
            image.save(path, format="PNG", optimize=True)
        self._done(path)

    def text(self, name, content):
        path = self._prepare(name)
        path.write_text(content, encoding="utf-8", newline="\n")
        self._done(path)

    def ico(self, name, images):
        path = self._prepare(name)
        write_ico(path, images)
        self._done(path)

    def icns(self, name, render):
        path = self._prepare(name)
        write_icns(path, render)
        self._done(path)


def generate(repo):
    out = Writer(repo)
    icons = "src/Gui/Icons"
    stylesheets = "src/Gui/Stylesheets"

    print("application icons")
    out.text(f"{icons}/fucad.svg", app_icon_svg())
    out.text(f"{icons}/fucad-doc.svg", document_icon_svg())
    for size in (16, 32, 48, 64):
        out.image(f"{icons}/fucad-icon-{size}.png", app_icon(size))
    out.image(f"{icons}/fucad-doc.png", document_icon(64))

    print("splash and about")
    out.image(f"{icons}/fucadsplash.png", splash(1))
    out.image(f"{icons}/fucadsplash_2x.png", splash(2))
    out.image(f"{icons}/fucadabout.png", about_image(False))
    out.image(f"{icons}/fucadaboutdev.png", about_image(True))

    print("workspace watermarks")
    watermarks = (
        ("background_fucad", STEEL, 0.20),
        ("background_fucad_dark", PAPER, 0.14),
        ("background_fucad_light", INK, 0.16),
    )
    for name, colour, opacity in watermarks:
        for folder in ("images_classic", "images_dark-light"):
            out.text(f"{stylesheets}/{folder}/{name}.svg", background_svg(colour, opacity))
        shapes = [
            (points, rgba(colour, round(opacity * 255)))
            for points, _ in mark_polygons((0.0, 0.0, 98.0, 102.0), colour, colour, colour)
        ]
        out.image(f"{stylesheets}/images_classic/{name}.png", render_polygons((98, 102), shapes))

    print("windows")
    windows_icons = [app_icon(size) for size in ICON_SIZES]
    out.ico("src/Main/icon.ico", windows_icons)
    out.ico("package/WindowsInstaller/icons/FuCad.ico", windows_icons)
    out.image("package/WindowsInstaller/graphics/banner.bmp", installer_banner())
    out.image("package/WindowsInstaller/graphics/header.bmp", installer_header())
    out.ico("src/Doc/sphinx/_static/favicon.ico", [app_icon(size) for size in (16, 32, 48)])

    print("macos")
    for resources in (
        "src/MacAppBundle/FuCad.app/Contents/Resources",
        "package/rattler-build/osx/resources",
    ):
        out.icns(f"{resources}/fucad.icns", app_icon)
        out.icns(f"{resources}/fucad-doc.icns", document_icon)
        out.icns(f"{resources}/fucad-script.icns", script_icon)
    out.image("src/MacAppBundle/DiskImage/background.png", dmg_background())

    print("project")
    out.image(".github/images/fucad-banner.png", readme_banner())

    print(f"\n{out.count} files written")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--repo",
        type=Path,
        default=Path(__file__).resolve().parents[3],
        help="repository root (defaults to the checkout this script lives in)",
    )
    arguments = parser.parse_args()
    generate(arguments.repo.resolve())


if __name__ == "__main__":
    main()
