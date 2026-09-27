"""Werkzeugweg: die fertige Punktfolge, die der Roboter abfährt.

Der Werkzeugweg ist die gemeinsame Grundlage für alle Ausgaben (G-Code,
Arduino-Header, Vorschau), damit alle drei garantiert dasselbe beschreiben.

Jeder Punkt hat einen Stift-Zustand:

* ``STIFT_OBEN``  (0) – Leerfahrt zu diesem Punkt (Stift vorher anheben)
* ``STIFT_UNTEN`` (1) – mit abgesetztem Stift zu diesem Punkt zeichnen

Vor dem ersten Punkt steht der Roboter auf der Startposition, nach dem letzten
fährt er (mit angehobenem Stift) zur Endposition.
"""

from __future__ import annotations

import math
from dataclasses import dataclass

import numpy as np

from .geometrie import Strich
from .konfig import Konfig

STIFT_OBEN = 0
STIFT_UNTEN = 1
NACHKOMMASTELLEN = 2  # Auflösung 0,01 mm


@dataclass
class Werkzeugweg:
    xy: np.ndarray  # (N, 2) in mm, auf 0,01 mm gerundet
    stift: np.ndarray  # (N,) STIFT_OBEN / STIFT_UNTEN

    def __len__(self) -> int:
        return len(self.stift)


def erzeuge_werkzeugweg(striche: list[Strich], verbinden_abstand: float) -> Werkzeugweg:
    punkte: list[tuple[float, float]] = []
    stift: list[int] = []
    letzter = None
    for s in striche:
        s = np.round(np.asarray(s, dtype=float), NACHKOMMASTELLEN)
        behalten = np.ones(len(s), dtype=bool)
        behalten[1:] = np.any(np.diff(s, axis=0) != 0, axis=1)
        s = s[behalten]
        if len(s) < 2:
            continue
        anfang = s[0]
        if letzter is not None and math.hypot(*(anfang - letzter)) <= verbinden_abstand:
            # so nah am vorigen Strichende: ohne Absetzen weiterzeichnen
            if np.any(anfang != letzter):
                punkte.append(tuple(anfang))
                stift.append(STIFT_UNTEN)
        else:
            punkte.append(tuple(anfang))
            stift.append(STIFT_OBEN)
        for p in s[1:]:
            punkte.append(tuple(p))
            stift.append(STIFT_UNTEN)
        letzter = s[-1]
    return Werkzeugweg(
        xy=np.array(punkte, dtype=float).reshape(-1, 2),
        stift=np.array(stift, dtype=np.uint8),
    )


@dataclass
class Statistik:
    striche: int  # Anzahl Absetzvorgänge
    punkte: int
    zeichenweg: float  # mm
    leerweg: float  # mm (inkl. Fahrt von Start / zur Endposition)
    dauer_s: float  # geschätzte Zeichendauer in Sekunden

    def dauer_text(self) -> str:
        m, s = divmod(round(self.dauer_s), 60)
        return f"{m} min {s:02d} s"


def berechne_statistik(weg: Werkzeugweg, cfg: Konfig) -> Statistik:
    zz, zh = cfg.stift.z_zeichnen, cfg.stift.z_heben
    v = cfg.vorschub
    sp, ep = cfg.startposition, cfg.endposition
    zeichen = leer = 0.0
    dauer = 0.0
    striche = 0
    if len(weg):
        d = np.diff(weg.xy, axis=0)
        seg = np.hypot(d[:, 0], d[:, 1])
        zeichen = float(seg[weg.stift[1:] == STIFT_UNTEN].sum())
        leer = float(seg[weg.stift[1:] == STIFT_OBEN].sum())
        striche = int(np.sum(weg.stift == STIFT_OBEN))
        x0, y0 = weg.xy[0]
        xn, yn = weg.xy[-1]
        leer_start = math.dist((sp.x, sp.y, sp.z), (x0, y0, zh))
        leer_ende = math.dist((xn, yn, zh), (ep.x, ep.y, ep.z))
        leer += leer_start + leer_ende
        dauer = (zeichen / v.zeichnen + leer / v.leerfahrt + 2 * striche * (zh - zz) / v.z) * 60
    return Statistik(striche=striche, punkte=len(weg), zeichenweg=zeichen, leerweg=leer, dauer_s=dauer)
