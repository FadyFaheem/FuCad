# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright (c) 2026 FuCad contributors

"""Single source of truth for the FuCad visual identity.

The palette and the geometry of the FuCad mark are defined here once and are
consumed by ``generate_brand_assets.py`` to emit every logo, icon, splash and
packaging graphic in the repository. Nothing else should hard-code brand
colours or redraw the mark.

The mark is an extruded "F": a sketch profile pushed along an axonometric
direction, the operation every parametric CAD model starts with. The front
face reads as the letter, the swept faces give it the depth and the accent
colour.
"""

import struct
from io import BytesIO
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont

# --------------------------------------------------------------------------
# Palette
# --------------------------------------------------------------------------

INK = (7, 31, 39)  # near-black teal, text on light surfaces
STEEL_DEEP = (8, 45, 56)  # darkest plate tone
STEEL = (11, 61, 74)  # primary
STEEL_LIT = (18, 82, 95)  # lightest plate tone
AMBER = (240, 162, 2)  # accent
AMBER_LIT = (255, 194, 77)  # accent highlight
PAPER = (242, 247, 248)  # near-white
MIST = (124, 152, 161)  # cool grey, secondary text

TAGLINE = "PARAMETRIC 3D MODELING"


def hex_of(rgb):
    """Return the ``#rrggbb`` form of an RGB tuple."""
    return "#{:02x}{:02x}{:02x}".format(*rgb[:3])


def rgba(rgb, alpha=255):
    """Return an RGBA tuple for an RGB colour and an 0-255 alpha."""
    return (rgb[0], rgb[1], rgb[2], alpha)


# --------------------------------------------------------------------------
# Mark geometry, expressed in a 48 unit design box with y pointing down
# --------------------------------------------------------------------------

# Clockwise profile of the letter, drawn as it faces the viewer.
F_PROFILE = (
    (11.0, 14.0),
    (31.0, 14.0),
    (31.0, 20.5),
    (17.5, 20.5),
    (17.5, 24.0),
    (27.0, 24.0),
    (27.0, 29.5),
    (17.5, 29.5),
    (17.5, 37.0),
    (11.0, 37.0),
)

# Direction the profile is swept along, up and to the right.
EXTRUDE = (4.5, -2.6)


def _visible_faces(profile, extrude):
    """Split the swept faces of an extruded profile into top and side faces.

    ``profile`` is wound clockwise in screen coordinates, so the outward normal
    of the edge ``p1 -> p2`` is ``(dy, -dx)``. A face is visible when its normal
    points towards the sweep direction; faces pointing away are hidden behind
    the solid and are never drawn.
    """
    top, side = [], []
    count = len(profile)
    for index in range(count):
        p1 = profile[index]
        p2 = profile[(index + 1) % count]
        edge = (p2[0] - p1[0], p2[1] - p1[1])
        normal = (edge[1], -edge[0])
        if normal[0] * extrude[0] + normal[1] * extrude[1] <= 0:
            continue
        quad = [
            p1,
            p2,
            (p2[0] + extrude[0], p2[1] + extrude[1]),
            (p1[0] + extrude[0], p1[1] + extrude[1]),
        ]
        (top if abs(normal[1]) > abs(normal[0]) else side).append(quad)
    return top, side


TOP_FACES, SIDE_FACES = _visible_faces(F_PROFILE, EXTRUDE)


def _bounds(polygons):
    xs = [x for polygon in polygons for x, _ in polygon]
    ys = [y for polygon in polygons for _, y in polygon]
    return min(xs), min(ys), max(xs), max(ys)


MARK_BOUNDS = _bounds([list(F_PROFILE)] + TOP_FACES + SIDE_FACES)
MARK_ASPECT = (MARK_BOUNDS[2] - MARK_BOUNDS[0]) / (MARK_BOUNDS[3] - MARK_BOUNDS[1])


def mark_polygons(box, front, top, side):
    """Return ``(points, colour)`` pairs for the mark fitted into ``box``.

    ``box`` is ``(x, y, width, height)``; the mark keeps its aspect ratio and is
    centred inside it. Faces are returned back to front, so painting them in
    order produces the correct silhouette.
    """
    x, y, width, height = box
    min_x, min_y, max_x, max_y = MARK_BOUNDS
    scale = min(width / (max_x - min_x), height / (max_y - min_y))
    offset_x = x + (width - (max_x - min_x) * scale) / 2 - min_x * scale
    offset_y = y + (height - (max_y - min_y) * scale) / 2 - min_y * scale

    def place(polygon):
        return [(px * scale + offset_x, py * scale + offset_y) for px, py in polygon]

    shapes = [(place(quad), side) for quad in SIDE_FACES]
    shapes += [(place(quad), top) for quad in TOP_FACES]
    shapes.append((place(F_PROFILE), front))
    return shapes


# --------------------------------------------------------------------------
# Raster helpers
# --------------------------------------------------------------------------

SUPERSAMPLE = 4


def render_polygons(size, shapes, supersample=SUPERSAMPLE):
    """Draw flat coloured polygons into a transparent RGBA image."""
    width, height = size
    canvas = Image.new("RGBA", (width * supersample, height * supersample), (0, 0, 0, 0))
    draw = ImageDraw.Draw(canvas)
    for points, colour in shapes:
        draw.polygon(
            [(px * supersample, py * supersample) for px, py in points],
            fill=colour,
        )
    return canvas.resize((width, height), Image.LANCZOS)


def rounded_rect_mask(size, radius, supersample=SUPERSAMPLE):
    """Return an 8-bit mask holding an antialiased rounded rectangle."""
    width, height = size
    mask = Image.new("L", (width * supersample, height * supersample), 0)
    ImageDraw.Draw(mask).rounded_rectangle(
        (0, 0, width * supersample - 1, height * supersample - 1),
        radius=radius * supersample,
        fill=255,
    )
    return mask.resize((width, height), Image.LANCZOS)


def rounded_rect_ring(size, radius, thickness, supersample=SUPERSAMPLE):
    """Return an 8-bit mask holding the outline of a rounded rectangle."""
    width, height = size
    mask = Image.new("L", (width * supersample, height * supersample), 0)
    ImageDraw.Draw(mask).rounded_rectangle(
        (0, 0, width * supersample - 1, height * supersample - 1),
        radius=radius * supersample,
        outline=255,
        width=max(1, int(thickness * supersample)),
    )
    return mask.resize((width, height), Image.LANCZOS)


def linear_gradient(size, start, end, angle=45.0):
    """Return an RGB image holding a linear gradient at ``angle`` degrees."""
    width, height = size
    radians = np.deg2rad(angle)
    xs = np.linspace(0.0, 1.0, width, dtype=np.float32)[None, :]
    ys = np.linspace(0.0, 1.0, height, dtype=np.float32)[:, None]
    ramp = xs * np.cos(radians) + ys * np.sin(radians)
    span = ramp.max() - ramp.min()
    ramp = (ramp - ramp.min()) / (span if span else 1.0)
    ramp = ramp[:, :, None]
    colours = (
        np.array(start, dtype=np.float32) * (1.0 - ramp) + np.array(end, dtype=np.float32) * ramp
    )
    return Image.fromarray(colours.astype(np.uint8), "RGB")


def radial_glow(size, centre, radius, colour, strength):
    """Return an RGBA image holding a soft radial glow used for atmosphere."""
    width, height = size
    ys, xs = np.mgrid[0:height, 0:width].astype(np.float32)
    distance = np.hypot(xs - centre[0], ys - centre[1]) / float(radius)
    falloff = np.clip(1.0 - distance, 0.0, 1.0) ** 2
    layer = np.zeros((height, width, 4), dtype=np.float32)
    layer[:, :, 0] = colour[0]
    layer[:, :, 1] = colour[1]
    layer[:, :, 2] = colour[2]
    layer[:, :, 3] = falloff * strength
    return Image.fromarray(layer.astype(np.uint8), "RGBA")


ISO_SLOPE = 0.57735  # tan(30 degrees)


def isometric_grid(size, spacing, colour, alpha, supersample=2):
    """Return an RGBA layer of faint construction lines at +/-30 degrees."""
    width, height = size
    big_width, big_height = width * supersample, height * supersample
    layer = Image.new("RGBA", (big_width, big_height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(layer)

    run = big_height / ISO_SLOPE  # horizontal travel over the full height
    step = spacing * supersample / ISO_SLOPE
    for run_sign in (1.0, -1.0):
        start = -run if run_sign > 0 else 0.0
        while start < big_width + run:
            draw.line(
                [(start, 0), (start + run_sign * run, big_height)],
                fill=rgba(colour, alpha),
                width=supersample,
            )
            start += step
    return layer.resize((width, height), Image.LANCZOS)


def paste_shapes(base, size, shapes):
    """Render polygons and alpha-composite them onto ``base`` in place."""
    base.alpha_composite(render_polygons(size, shapes))


# --------------------------------------------------------------------------
# Type
# --------------------------------------------------------------------------

_FONT_CANDIDATES = (
    ("C:/Windows/Fonts/bahnschrift.ttf", True),
    ("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", False),
    ("/System/Library/Fonts/Supplemental/Futura.ttc", False),
    ("C:/Windows/Fonts/segoeui.ttf", False),
)


def load_font(pixels, weight="SemiBold"):
    """Load the brand typeface at ``pixels`` em size.

    Bahnschrift is a DIN derivative and carries the technical-drawing feel the
    brand is after; the other candidates are fallbacks for machines that do not
    have it so the generator still runs.
    """
    for path, variable in _FONT_CANDIDATES:
        if not Path(path).exists():
            continue
        font = ImageFont.truetype(path, pixels)
        if variable:
            try:
                font.set_variation_by_name(weight)
            except OSError:
                pass
        return font
    raise RuntimeError("no usable font found for the FuCad wordmark")


def tracked_width(font, text, tracking):
    """Return the advance width of ``text`` including letter tracking."""
    total = 0.0
    for character in text:
        total += font.getlength(character) + tracking
    return total - tracking if text else 0.0


def draw_tracked(draw, position, text, font, fill, tracking=0.0, anchor="ls"):
    """Draw ``text`` with manual letter tracking and return its advance width."""
    x, y = position
    if anchor.startswith("m"):
        x -= tracked_width(font, text, tracking) / 2
    for character in text:
        draw.text((x, y), character, font=font, fill=fill, anchor="l" + anchor[1])
        x += font.getlength(character) + tracking
    return x - position[0]


# --------------------------------------------------------------------------
# Container formats Pillow cannot write on its own
# --------------------------------------------------------------------------


def png_bytes(image):
    """Return ``image`` encoded as a PNG byte string."""
    buffer = BytesIO()
    image.save(buffer, format="PNG", optimize=True)
    return buffer.getvalue()


def _dib_entry(image):
    """Encode an image as the 32-bit DIB payload of an ICO entry.

    Windows accepts PNG payloads for large icons, but shell surfaces and NSIS
    are happiest with plain DIB data, so every size below 256 is written this
    way: a bottom-up BGRA bitmap followed by the legacy 1-bit AND mask.
    """
    width, height = image.size
    pixels = np.array(image.convert("RGBA"), dtype=np.uint8)[::-1]
    bgra = pixels[:, :, [2, 1, 0, 3]].tobytes()

    stride = ((width + 31) // 32) * 4
    mask = np.zeros((height, stride), dtype=np.uint8)
    transparent = pixels[:, :, 3] < 128
    for x in range(width):
        mask[:, x // 8] |= transparent[:, x].astype(np.uint8) << (7 - (x % 8))

    header = struct.pack("<IiiHHIIiiII", 40, width, height * 2, 1, 32, 0, len(bgra), 0, 0, 0, 0)
    return header + bgra + mask.tobytes()


def write_ico(path, images):
    """Write a multi-resolution ``.ico`` from pre-rendered images."""
    images = sorted(images, key=lambda image: image.size[0])
    payloads = [png_bytes(image) if image.size[0] >= 256 else _dib_entry(image) for image in images]

    offset = 6 + 16 * len(images)
    directory = struct.pack("<HHH", 0, 1, len(images))
    for image, payload in zip(images, payloads):
        dimension = 0 if image.size[0] >= 256 else image.size[0]
        directory += struct.pack(
            "<BBBBHHII", dimension, dimension, 0, 0, 1, 32, len(payload), offset
        )
        offset += len(payload)

    Path(path).write_bytes(directory + b"".join(payloads))


_ICNS_TYPES = (
    ("icp4", 16),
    ("icp5", 32),
    ("ic11", 32),
    ("ic12", 64),
    ("ic07", 128),
    ("ic08", 256),
    ("ic13", 256),
    ("ic09", 512),
    ("ic14", 512),
)


def write_icns(path, render):
    """Write a macOS ``.icns`` using ``render(size)`` for each needed size."""
    cache = {}
    body = b""
    for ostype, size in _ICNS_TYPES:
        if size not in cache:
            cache[size] = png_bytes(render(size))
        payload = cache[size]
        body += ostype.encode("ascii") + struct.pack(">I", len(payload) + 8) + payload
    Path(path).write_bytes(b"icns" + struct.pack(">I", len(body) + 8) + body)


# --------------------------------------------------------------------------
# SVG helpers
# --------------------------------------------------------------------------


def svg_path(points):
    """Return an SVG path definition for a closed polygon."""
    head = "M {:.3f},{:.3f}".format(*points[0])
    tail = " ".join("L {:.3f},{:.3f}".format(x, y) for x, y in points[1:])
    return f"{head} {tail} Z"


def svg_document(width, height, body, title):
    """Wrap SVG fragments in a minimal, Qt-friendly document."""
    return (
        '<?xml version="1.0" encoding="UTF-8"?>\n'
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}" version="1.1">\n'
        f"  <title>{title}</title>\n"
        f"{body}\n"
        "</svg>\n"
    )
