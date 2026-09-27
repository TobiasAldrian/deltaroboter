"""SVG-Dateien in Striche umwandeln.

Jede Form (Pfad, Rechteck, Kreis, Ellipse, Linie, Polygon, ...) wird als Linie
gezeichnet – egal ob sie in der SVG gefüllt oder nur umrandet ist. Kurven
werden in kurze Geradenstücke zerlegt, Ecken bleiben exakt erhalten.
Texte werden nicht unterstützt: In Inkscape vorher "Pfad > Objekt in Pfad
umwandeln" ausführen.
"""

from __future__ import annotations

import math
import warnings
from pathlib import Path

import numpy as np

from .geometrie import Strich


def svg_zu_strichen(pfad: Path, feinheit: float = 1 / 2000) -> list[Strich]:
    """SVG → Striche in SVG-Einheiten (y nach unten, wie im Bild).

    `feinheit`: Länge der Kurvenstücke relativ zur Diagonale der Zeichnung.
    """
    try:
        from svgelements import SVG, Close, Line, Move, Path as SvgPath, Shape, Text
    except ImportError as e:  # pragma: no cover
        raise ImportError("Für SVG-Dateien wird das Paket 'svgelements' benötigt: pip install svgelements") from e

    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        svg = SVG.parse(str(pfad), reify=True)
    formen = []
    texte = 0
    for element in svg.elements():
        if isinstance(element, Text):
            texte += 1
        elif isinstance(element, Shape):
            if element.values.get("visibility") == "hidden" or element.values.get("display") == "none":
                continue
            formen.append(element if isinstance(element, SvgPath) else SvgPath(element))
    if texte:
        print(f"Hinweis: {texte} Text-Element(e) in der SVG werden ignoriert "
              "(in Inkscape vorher in Pfade umwandeln).")

    boxen = [f.bbox() for f in formen if f.bbox() is not None]
    if not boxen:
        return []
    x0 = min(b[0] for b in boxen)
    y0 = min(b[1] for b in boxen)
    x1 = max(b[2] for b in boxen)
    y1 = max(b[3] for b in boxen)
    schritt = max(math.hypot(x1 - x0, y1 - y0) * feinheit, 1e-9)

    striche: list[Strich] = []
    for form in formen:
        aktuell: list[tuple[float, float]] = []
        for seg in form.segments():
            if isinstance(seg, Move):
                if len(aktuell) > 1:
                    striche.append(np.array(aktuell))
                aktuell = [(seg.end.x, seg.end.y)]
                continue
            if seg.start is None or seg.end is None:
                continue
            if not aktuell:
                aktuell = [(seg.start.x, seg.start.y)]
            if isinstance(seg, (Line, Close)):
                aktuell.append((seg.end.x, seg.end.y))
            else:  # Bézier-Kurven, Bögen
                n = max(2, math.ceil(seg.length(error=1e-4) / schritt))
                for t in np.linspace(0, 1, n + 1)[1:]:
                    q = seg.point(float(t))
                    aktuell.append((q.x, q.y))
        if len(aktuell) > 1:
            striche.append(np.array(aktuell))

    ergebnis = []
    for s in striche:
        # doppelte Punkte hintereinander entfernen
        behalten = np.ones(len(s), dtype=bool)
        behalten[1:] = np.any(np.abs(np.diff(s, axis=0)) > 1e-12, axis=1)
        s = s[behalten]
        if len(s) > 1:
            ergebnis.append(s.astype(float))
    return ergebnis
