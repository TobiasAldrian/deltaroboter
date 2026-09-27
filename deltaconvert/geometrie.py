"""Geometrie-Funktionen für Striche (Polylinien).

Ein *Strich* ist ein numpy-Array der Form (N, 2) mit x/y-Koordinaten. Er wird
ohne Absetzen des Stifts gezeichnet. Ein Strich ist *geschlossen*, wenn erster
und letzter Punkt gleich sind (z. B. ein Kreis oder eine Kontur).
"""

from __future__ import annotations

import numpy as np

Strich = np.ndarray


def ist_geschlossen(s: Strich, toleranz: float = 1e-9) -> bool:
    return len(s) > 2 and bool(np.all(np.abs(s[0] - s[-1]) <= toleranz))


def laenge(s: Strich) -> float:
    if len(s) < 2:
        return 0.0
    d = np.diff(s, axis=0)
    return float(np.sum(np.hypot(d[:, 0], d[:, 1])))


def glaetten(s: Strich, n: int) -> Strich:
    """Gleitender Mittelwert über ±n Punkte (entfernt Pixel-Treppenstufen).

    Bei offenen Strichen bleiben Anfangs- und Endpunkt erhalten, geschlossene
    Striche werden ringförmig geglättet und bleiben geschlossen.
    """
    if n <= 0 or len(s) < 3:
        return s
    k = 2 * n + 1
    kern = np.ones(k) / k
    if ist_geschlossen(s):
        kern_pts = s[:-1]
        if len(kern_pts) < k:
            return s
        erweitert = np.concatenate([kern_pts[-n:], kern_pts, kern_pts[:n]])
        glatt = np.column_stack(
            [np.convolve(erweitert[:, i], kern, mode="valid") for i in range(2)]
        )
        return np.vstack([glatt, glatt[:1]])
    erweitert = np.concatenate([np.repeat(s[:1], n, axis=0), s, np.repeat(s[-1:], n, axis=0)])
    glatt = np.column_stack([np.convolve(erweitert[:, i], kern, mode="valid") for i in range(2)])
    glatt[0] = s[0]
    glatt[-1] = s[-1]
    return glatt


def vereinfachen(s: Strich, toleranz: float) -> Strich:
    """Douglas-Peucker: entfernt Punkte, die weniger als `toleranz` abweichen."""
    if toleranz <= 0 or len(s) < 3:
        return s
    behalten = np.zeros(len(s), dtype=bool)
    behalten[0] = behalten[-1] = True
    stapel = [(0, len(s) - 1)]
    while stapel:
        i, j = stapel.pop()
        if j <= i + 1:
            continue
        a, b = s[i], s[j]
        mitte = s[i + 1 : j]
        ab = b - a
        l_ab = float(np.hypot(ab[0], ab[1]))
        if l_ab < 1e-12:
            d = np.hypot(mitte[:, 0] - a[0], mitte[:, 1] - a[1])
        else:
            d = np.abs(ab[0] * (mitte[:, 1] - a[1]) - ab[1] * (mitte[:, 0] - a[0])) / l_ab
        k = int(np.argmax(d))
        if d[k] > toleranz:
            m = i + 1 + k
            behalten[m] = True
            stapel.append((i, m))
            stapel.append((m, j))
    return s[behalten]


def ausrichten(striche: list[Strich], drehung: int = 0, spiegeln: bool = False) -> list[Strich]:
    """Bildkoordinaten (y nach unten) → Zeichenkoordinaten (y nach oben).

    Danach optional spiegeln (an der Y-Achse) und drehen (gegen den
    Uhrzeigersinn, 0/90/180/270 Grad).
    """
    ergebnis = []
    for s in striche:
        p = np.asarray(s, dtype=float).copy()
        p[:, 1] = -p[:, 1]
        if spiegeln:
            p[:, 0] = -p[:, 0]
        x, y = p[:, 0].copy(), p[:, 1].copy()
        if drehung == 90:
            p[:, 0], p[:, 1] = -y, x
        elif drehung == 180:
            p[:, 0], p[:, 1] = -x, -y
        elif drehung == 270:
            p[:, 0], p[:, 1] = y, -x
        ergebnis.append(p)
    return ergebnis


def einpassen(
    striche: list[Strich], breite: float, hoehe: float, rand: float
) -> tuple[list[Strich], float]:
    """Skaliert und verschiebt alle Striche gemeinsam in die Zeichenfläche.

    Das Seitenverhältnis bleibt erhalten, die Zeichnung wird mittig um den
    Ursprung (0, 0) = Mitte der Zeichenfläche platziert.
    Rückgabe: (neue Striche, Maßstab in mm pro Eingabeeinheit).
    """
    alle = np.vstack(striche)
    mn, mx = alle.min(axis=0), alle.max(axis=0)
    groesse = mx - mn
    verfuegbar = np.array([breite - 2 * rand, hoehe - 2 * rand])
    faktoren = [verfuegbar[i] / groesse[i] for i in range(2) if groesse[i] > 1e-12]
    massstab = min(faktoren) if faktoren else 1.0
    mitte = (mn + mx) / 2
    return [(s - mitte) * massstab for s in striche], float(massstab)


def reihenfolge_optimieren(
    striche: list[Strich], start: tuple[float, float], max_kandidaten: int = 64
) -> list[Strich]:
    """Sortiert die Striche so, dass die Leerfahrten möglichst kurz werden.

    Greedy "nächster Nachbar": Vom aktuellen Punkt aus wird immer der nächste
    noch nicht gezeichnete Strich gewählt. Offene Striche dürfen rückwärts
    gezeichnet werden, geschlossene dürfen an einem beliebigen Punkt beginnen.
    """
    if not striche:
        return []
    kand_pkt, kand_strich, kand_index = [], [], []
    for i, s in enumerate(striche):
        if ist_geschlossen(s):
            n = len(s) - 1
            idx = np.unique(np.linspace(0, n - 1, min(n, max_kandidaten)).round().astype(int))
        else:
            idx = np.array([0, len(s) - 1]) if len(s) > 1 else np.array([0])
        kand_pkt.append(s[idx])
        kand_strich.append(np.full(len(idx), i))
        kand_index.append(idx)
    pkt = np.vstack(kand_pkt)
    zu_strich = np.concatenate(kand_strich)
    zu_index = np.concatenate(kand_index)

    aktiv = np.ones(len(striche), dtype=bool)
    aktuell = np.asarray(start, dtype=float)
    ergebnis = []
    for _ in range(len(striche)):
        maske = aktiv[zu_strich]
        kandidaten = np.flatnonzero(maske)
        d = np.hypot(pkt[kandidaten, 0] - aktuell[0], pkt[kandidaten, 1] - aktuell[1])
        w = kandidaten[int(np.argmin(d))]
        i, k = int(zu_strich[w]), int(zu_index[w])
        s = striche[i]
        if ist_geschlossen(s):
            ring = np.roll(s[:-1], -k, axis=0)
            s = np.vstack([ring, ring[:1]])
        elif k != 0:
            s = s[::-1]
        ergebnis.append(s)
        aktiv[i] = False
        aktuell = s[-1]
    return ergebnis
