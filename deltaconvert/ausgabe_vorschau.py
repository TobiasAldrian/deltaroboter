"""SVG-Vorschau des Werkzeugwegs (im Browser öffnen).

Schwarz = gezeichnete Linien, rot gestrichelt = Leerfahrten (Stift oben),
grün = Startposition, blau = Endposition. Die Datei ist maßstabsgetreu (mm)
und kann zur Kontrolle auch 1:1 ausgedruckt werden.
"""

from __future__ import annotations

from html import escape

from .konfig import Konfig
from .werkzeugweg import STIFT_UNTEN, Statistik, Werkzeugweg


def erzeuge_vorschau(weg: Werkzeugweg, cfg: Konfig, titel: str, stat: Statistik,
                     strichbreite: float = 0.5) -> str:
    zf = cfg.zeichenflaeche
    sp, ep = cfg.startposition, cfg.endposition
    rand_aussen = 10.0
    info_h = 14.0
    b = zf.breite + 2 * rand_aussen
    h = zf.hoehe + 2 * rand_aussen + info_h
    x0 = -zf.breite / 2 - rand_aussen
    y0 = -zf.hoehe / 2 - rand_aussen - info_h

    def pt(x: float, y: float) -> str:
        return f"{x:.2f},{-y:.2f}"  # SVG: y nach unten → spiegeln

    zeichnen: list[str] = []
    leer: list[str] = []
    if len(weg):
        leer.append(f"M{pt(sp.x, sp.y)} L{pt(*weg.xy[0])}")
        pfad: list[str] = []
        vorher = weg.xy[0]
        for (x, y), stift in zip(weg.xy[1:], weg.stift[1:]):
            if stift == STIFT_UNTEN:
                if not pfad:
                    pfad.append(f"M{pt(*vorher)}")
                pfad.append(f"L{pt(x, y)}")
            else:
                if pfad:
                    zeichnen.append(" ".join(pfad))
                    pfad = []
                leer.append(f"M{pt(*vorher)} L{pt(x, y)}")
            vorher = (x, y)
        if pfad:
            zeichnen.append(" ".join(pfad))
        leer.append(f"M{pt(*weg.xy[-1])} L{pt(ep.x, ep.y)}")

    innen_b = zf.breite - 2 * zf.rand
    innen_h = zf.hoehe - 2 * zf.rand
    info = (f"{escape(titel)} | {stat.striche} Striche, {stat.punkte} Punkte | "
            f"Zeichenweg {stat.zeichenweg / 1000:.2f} m, Leerweg {stat.leerweg / 1000:.2f} m | "
            f"ca. {stat.dauer_text()}")
    s = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{b:g}mm" height="{h:g}mm" '
        f'viewBox="{x0:g} {y0:g} {b:g} {h:g}">',
        f'<rect x="{x0:g}" y="{y0:g}" width="{b:g}" height="{h:g}" fill="#f4f4f4"/>',
        f'<rect x="{-zf.breite / 2:g}" y="{-zf.hoehe / 2:g}" width="{zf.breite:g}" '
        f'height="{zf.hoehe:g}" fill="#ffffff" stroke="#999" stroke-width="0.3"/>',
        f'<rect x="{-innen_b / 2:g}" y="{-innen_h / 2:g}" width="{innen_b:g}" height="{innen_h:g}" '
        f'fill="none" stroke="#ccc" stroke-width="0.2" stroke-dasharray="2 2"/>',
        '<path d="M-4,0 H4 M0,-4 V4" stroke="#bbb" stroke-width="0.2"/>',
        f'<path d="{" ".join(leer)}" fill="none" stroke="#e5484d" stroke-width="0.25" '
        f'stroke-dasharray="1.5 1.5" opacity="0.8"/>',
        f'<path d="{" ".join(zeichnen)}" fill="none" stroke="#111" stroke-width="{strichbreite:g}" '
        f'stroke-linecap="round" stroke-linejoin="round"/>',
        f'<circle cx="{sp.x:.2f}" cy="{-sp.y:.2f}" r="2" fill="#2da44e"/>',
        f'<circle cx="{ep.x:.2f}" cy="{-ep.y:.2f}" r="1.2" fill="#0969da"/>',
        f'<text x="{x0 + 4:g}" y="{y0 + 9:g}" font-family="sans-serif" font-size="4.2" '
        f'fill="#333">{info}</text>',
        "</svg>",
    ]
    return "\n".join(s) + "\n"
