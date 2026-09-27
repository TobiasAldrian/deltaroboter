"""Ablauf der Umwandlung: Datei → Striche → Werkzeugweg.

    Eingabe (Bild/SVG)
      → Striche in Pixel-/SVG-Einheiten
      → ausrichten (y umdrehen, drehen, spiegeln)
      → in die Zeichenfläche einpassen (mm)
      → zu kurze Striche entfernen, erneut einpassen
      → vereinfachen (weniger Punkte)
      → Reihenfolge optimieren (kurze Leerfahrten)
      → Werkzeugweg
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from .geometrie import Strich, ausrichten, einpassen, laenge, reihenfolge_optimieren, vereinfachen
from .konfig import Konfig
from .rasterbild import RASTER_ENDUNGEN, bild_zu_strichen
from .svgdatei import svg_zu_strichen
from .werkzeugweg import Statistik, Werkzeugweg, berechne_statistik, erzeuge_werkzeugweg


@dataclass
class Ergebnis:
    striche: list[Strich]  # in mm, fertig sortiert
    weg: Werkzeugweg
    statistik: Statistik
    massstab: float  # mm pro Pixel bzw. SVG-Einheit


def lade_striche(eingabe: Path, cfg: Konfig) -> list[Strich]:
    endung = eingabe.suffix.lower()
    if not eingabe.is_file():
        raise FileNotFoundError(f"Eingabedatei nicht gefunden: {eingabe}")
    if endung == ".svg":
        return svg_zu_strichen(eingabe)
    if endung in RASTER_ENDUNGEN:
        return bild_zu_strichen(eingabe, cfg.bild)
    raise ValueError(
        f"Dateityp '{endung}' wird nicht unterstützt. Erlaubt: .svg, "
        + ", ".join(sorted(RASTER_ENDUNGEN))
    )


def konvertiere(eingabe: Path, cfg: Konfig) -> Ergebnis:
    roh = [s for s in lade_striche(Path(eingabe), cfg) if len(s) > 1]
    if not roh:
        raise ValueError(
            "Keine Linien gefunden. Tipp: [bild] schwellwert anpassen, 'invertieren' "
            "umschalten oder einen anderen Modus probieren."
        )
    zf, p = cfg.zeichenflaeche, cfg.pfade
    striche = ausrichten(roh, cfg.ausrichtung.drehung, cfg.ausrichtung.spiegeln)

    # Erst einpassen, damit min_laenge in mm gilt; dann ohne die kurzen
    # Störstriche noch einmal einpassen, damit die Fläche voll genutzt wird.
    mm, _ = einpassen(striche, zf.breite, zf.hoehe, zf.rand)
    behalten = [i for i, s in enumerate(mm) if laenge(s) >= p.min_laenge]
    if not behalten:
        raise ValueError("Alle Striche sind kürzer als [pfade] min_laenge – Wert verkleinern.")
    striche = [striche[i] for i in behalten]
    mm, massstab = einpassen(striche, zf.breite, zf.hoehe, zf.rand)

    mm = [vereinfachen(s, p.vereinfachen) for s in mm]
    if p.optimieren:
        sp = cfg.startposition
        mm = reihenfolge_optimieren(mm, (sp.x, sp.y))
    weg = erzeuge_werkzeugweg(mm, p.verbinden_abstand)
    return Ergebnis(striche=mm, weg=weg, statistik=berechne_statistik(weg, cfg), massstab=massstab)
