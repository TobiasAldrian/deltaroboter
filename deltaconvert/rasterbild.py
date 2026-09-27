"""Rasterbilder (PNG, JPG, BMP, ...) in Striche umwandeln.

Drei Modi:

* ``kontur``      – Umrisse aller dunklen Flächen (gut für Logos, Silhouetten).
* ``mittellinie`` – Mittellinie dunkler Linien, jede Linie nur einmal gezeichnet
                    (gut für Strichzeichnungen, Handskizzen, Schrift).
* ``kanten``      – Kantenerkennung (Canny), für Fotos.

Alle Striche kommen in Pixelkoordinaten zurück (x nach rechts, y nach unten).
"""

from __future__ import annotations

from pathlib import Path

import cv2
import numpy as np

from .geometrie import Strich, glaetten
from .konfig import Bild

RASTER_ENDUNGEN = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tif", ".tiff", ".webp"}


def lade_graustufen(pfad: Path, max_aufloesung: int) -> np.ndarray:
    """Lädt ein Bild als Graustufen (uint8). Transparenz wird zu Weiß.

    np.fromfile + imdecode statt cv2.imread, damit auch Pfade mit Umlauten
    unter Windows funktionieren.
    """
    daten = np.fromfile(str(pfad), dtype=np.uint8)
    bild = cv2.imdecode(daten, cv2.IMREAD_UNCHANGED)
    if bild is None:
        raise ValueError(f"Bild konnte nicht gelesen werden: {pfad}")
    if bild.dtype != np.uint8:  # z. B. 16-Bit-PNG
        bild = cv2.convertScaleAbs(bild, alpha=255.0 / max(1, int(bild.max())))
    if bild.ndim == 3 and bild.shape[2] == 4:
        alpha = bild[:, :, 3:4].astype(float) / 255.0
        farbe = bild[:, :, :3].astype(float)
        bild = (farbe * alpha + 255.0 * (1 - alpha)).astype(np.uint8)
    if bild.ndim == 3:
        bild = cv2.cvtColor(bild, cv2.COLOR_BGR2GRAY)
    h, w = bild.shape
    if max(h, w) > max_aufloesung:
        f = max_aufloesung / max(h, w)
        bild = cv2.resize(bild, (max(1, round(w * f)), max(1, round(h * f))), interpolation=cv2.INTER_AREA)
    return bild


def binarisieren(grau: np.ndarray, schwellwert: int, invertieren: bool, weichzeichnen: int) -> np.ndarray:
    """Schwarz/Weiß-Maske: 255 = zu zeichnen (dunkel), 0 = Hintergrund."""
    if weichzeichnen > 0:
        k = weichzeichnen if weichzeichnen % 2 == 1 else weichzeichnen + 1
        grau = cv2.GaussianBlur(grau, (k, k), 0)
    if schwellwert == 0:
        _, maske = cv2.threshold(grau, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    else:
        _, maske = cv2.threshold(grau, schwellwert, 255, cv2.THRESH_BINARY_INV)
    if invertieren:
        maske = 255 - maske
    return maske


def konturen(maske: np.ndarray) -> list[Strich]:
    """Umrisse aller Flächen der Maske als geschlossene Striche."""
    gefunden, _ = cv2.findContours(maske, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    striche = []
    for k in gefunden:
        p = k.reshape(-1, 2).astype(float)
        if len(p) < 3:
            continue
        striche.append(np.vstack([p, p[:1]]))
    return striche


def verduennen(maske: np.ndarray) -> np.ndarray:
    """Zhang-Suen-Verdünnung: reduziert Linien auf 1 Pixel Breite (Skelett)."""
    img = np.pad((maske > 0).astype(np.uint8), 1)
    geaendert = True
    while geaendert:
        geaendert = False
        for schritt in (0, 1):
            p2 = img[:-2, 1:-1]
            p3 = img[:-2, 2:]
            p4 = img[1:-1, 2:]
            p5 = img[2:, 2:]
            p6 = img[2:, 1:-1]
            p7 = img[2:, :-2]
            p8 = img[1:-1, :-2]
            p9 = img[:-2, :-2]
            nachbarn = [p2, p3, p4, p5, p6, p7, p8, p9]
            b = sum(n.astype(np.int16) for n in nachbarn)
            folge = nachbarn + [p2]
            a = sum(((folge[i] == 0) & (folge[i + 1] == 1)).astype(np.int16) for i in range(8))
            if schritt == 0:
                bed = ((p2 * p4 * p6) == 0) & ((p4 * p6 * p8) == 0)
            else:
                bed = ((p2 * p4 * p8) == 0) & ((p2 * p6 * p8) == 0)
            entfernen = (img[1:-1, 1:-1] == 1) & (b >= 2) & (b <= 6) & (a == 1) & bed
            if entfernen.any():
                img[1:-1, 1:-1][entfernen] = 0
                geaendert = True
    return img[1:-1, 1:-1] * 255


# Nachbarn (dy, dx): zuerst die 4 direkten, dann die 4 diagonalen
_VIER = ((-1, 0), (0, 1), (1, 0), (0, -1))
_DIAG = ((-1, 1), (1, 1), (1, -1), (-1, -1))


def skelett_verfolgen(skelett: np.ndarray) -> list[Strich]:
    """Zerlegt ein 1-Pixel-Skelett in Striche (Polylinien).

    Ein diagonaler Nachbar zählt nur, wenn es keinen direkten Umweg über einen
    gemeinsamen 4er-Nachbarn gibt – so entstehen an Treppenstufen keine
    Schein-Verzweigungen. Knoten sind Endpunkte (1 Nachbar) und Verzweigungen
    (≥3 Nachbarn); zwischen Knoten wird entlang der Linie gelaufen, übrig
    gebliebene Ringe (z. B. Kreise) werden am Schluss verfolgt.
    """
    sk = np.pad(skelett > 0, 1)
    h, w = sk.shape
    ys, xs = np.nonzero(sk)
    if len(ys) == 0:
        return []
    gesetzt = set((ys * w + xs).tolist())

    def nachbarn(p: int) -> list[int]:
        y, x = divmod(p, w)
        n = [(y + dy) * w + (x + dx) for dy, dx in _VIER if sk[y + dy, x + dx]]
        for dy, dx in _DIAG:
            if sk[y + dy, x + dx] and not sk[y + dy, x] and not sk[y, x + dx]:
                n.append((y + dy) * w + (x + dx))
        return n

    adj = {p: nachbarn(p) for p in gesetzt}
    knoten = {p for p, n in adj.items() if len(n) != 2}
    besucht: set[tuple[int, int]] = set()

    def kante(a: int, b: int) -> tuple[int, int]:
        return (a, b) if a < b else (b, a)

    def laufen(p0: int, p1: int) -> list[int]:
        pfad = [p0, p1]
        besucht.add(kante(p0, p1))
        vorher, aktuell = p0, p1
        while aktuell not in knoten and aktuell != p0:
            weiter = None
            for q in adj[aktuell]:
                if q != vorher and kante(aktuell, q) not in besucht:
                    weiter = q
                    break
            if weiter is None:
                break
            besucht.add(kante(aktuell, weiter))
            pfad.append(weiter)
            vorher, aktuell = aktuell, weiter
        return pfad

    pfade = []
    for p in sorted(knoten):
        for q in adj[p]:
            if kante(p, q) not in besucht:
                pfade.append(laufen(p, q))
    for p in sorted(gesetzt - knoten):
        for q in adj[p]:
            if kante(p, q) not in besucht:
                pfade.append(laufen(p, q))

    striche = []
    for pfad in pfade:
        arr = np.array([divmod(p, w) for p in pfad], dtype=float)  # (y, x)
        striche.append(arr[:, ::-1] - 1.0)  # → (x, y), Padding entfernen
    return striche


def kanten_maske(grau: np.ndarray, weichzeichnen: int) -> np.ndarray:
    """Canny-Kantenerkennung mit automatischen Schwellen (aus dem Median)."""
    if weichzeichnen > 0:
        k = weichzeichnen if weichzeichnen % 2 == 1 else weichzeichnen + 1
        grau = cv2.GaussianBlur(grau, (k, k), 0)
    median = float(np.median(grau))
    unten = int(max(0, 0.67 * median))
    oben = int(min(255, 1.33 * median))
    if oben <= unten:
        unten, oben = 50, 150
    return cv2.Canny(grau, unten, oben)


def bild_zu_strichen(pfad: Path, cfg: Bild) -> list[Strich]:
    """Hauptfunktion: Bilddatei → Striche in Pixelkoordinaten."""
    grau = lade_graustufen(pfad, cfg.max_aufloesung)
    if cfg.modus == "kanten":
        striche = skelett_verfolgen(verduennen(kanten_maske(grau, cfg.weichzeichnen)))
    else:
        maske = binarisieren(grau, cfg.schwellwert, cfg.invertieren, cfg.weichzeichnen)
        if cfg.modus == "kontur":
            striche = konturen(maske)
        else:  # mittellinie
            striche = skelett_verfolgen(verduennen(maske))
    return [glaetten(s, cfg.pixel_glaettung) for s in striche]
