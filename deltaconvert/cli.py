"""Kommandozeile:  python -m deltaconvert BILD [Optionen]"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from . import __version__
from .ausgabe_arduino import c_bezeichner, erzeuge_header, flash_warnung
from .ausgabe_gcode import erzeuge_gcode
from .ausgabe_vorschau import erzeuge_vorschau
from .konfig import MODI, KonfigFehler, lade_konfig, pruefen
from .konverter import konvertiere

FORMATE = ("gcode", "header", "vorschau")
REPO_ORDNER = Path(__file__).resolve().parent.parent


def standard_konfig() -> Path | None:
    """config/plotter.toml im aktuellen Ordner oder im Repository-Ordner."""
    for kandidat in (Path.cwd() / "config" / "plotter.toml", REPO_ORDNER / "config" / "plotter.toml"):
        if kandidat.is_file():
            return kandidat
    return None


def parser_erstellen() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="deltaconvert",
        description="Wandelt ein Bild (PNG/JPG/...) oder eine SVG-Datei in Zeichenpfade für den "
                    "Deltaroboter um und erzeugt G-Code, eine Arduino-Header-Datei und eine Vorschau.",
    )
    p.add_argument("eingabe", type=Path, help="Bild- oder SVG-Datei")
    p.add_argument("-c", "--konfig", type=Path, help="Konfigurationsdatei (Standard: config/plotter.toml)")
    p.add_argument("-o", "--ausgabe", type=Path, default=Path("ausgabe"), help="Ausgabeordner (Standard: ausgabe)")
    p.add_argument("-n", "--name", help="Name der Zeichnung (Standard: Dateiname ohne Endung)")
    p.add_argument("-m", "--modus", choices=MODI, help="überschreibt [bild] modus")
    p.add_argument("-s", "--schwellwert", type=int, help="überschreibt [bild] schwellwert (0 = automatisch)")
    p.add_argument("-i", "--invertieren", action="store_true", help="helle statt dunkle Bereiche zeichnen")
    p.add_argument("-f", "--formate", default=",".join(FORMATE),
                   help=f"welche Dateien erzeugt werden, kommagetrennt (Standard: {','.join(FORMATE)})")
    p.add_argument("--version", action="version", version=f"deltaroboter-converter {__version__}")
    return p


def main(argv: list[str] | None = None) -> int:
    args = parser_erstellen().parse_args(argv)
    try:
        formate = [f.strip() for f in args.formate.split(",") if f.strip()]
        falsch = [f for f in formate if f not in FORMATE]
        if falsch or not formate:
            raise ValueError(f"Unbekanntes Format: {', '.join(falsch) or '(leer)'}. Erlaubt: {', '.join(FORMATE)}")

        konfig_pfad = args.konfig or standard_konfig()
        cfg = lade_konfig(konfig_pfad)
        if args.modus:
            cfg.bild.modus = args.modus
        if args.schwellwert is not None:
            cfg.bild.schwellwert = args.schwellwert
        if args.invertieren:
            cfg.bild.invertieren = not cfg.bild.invertieren
        pruefen(cfg)

        name = c_bezeichner(args.name or args.eingabe.stem)
        if args.eingabe.suffix.lower() == ".svg":
            quelle = args.eingabe.name
        else:
            quelle = f"{args.eingabe.name} (Modus: {cfg.bild.modus})"
        print(f"Konfiguration: {konfig_pfad or 'Standardwerte'}")
        print(f"Eingabe:       {quelle}")
        erg = konvertiere(args.eingabe, cfg)
        st = erg.statistik

        args.ausgabe.mkdir(parents=True, exist_ok=True)
        erzeugt = []
        if "gcode" in formate:
            ziel = args.ausgabe / f"{name}.gcode"
            ziel.write_text(erzeuge_gcode(erg.weg, cfg, quelle, st, __version__), encoding="utf-8")
            erzeugt.append(ziel)
        if "header" in formate:
            ziel = args.ausgabe / f"{name}.h"
            ziel.write_text(erzeuge_header(erg.weg, cfg, name, quelle, st, __version__), encoding="utf-8")
            erzeugt.append(ziel)
        if "vorschau" in formate:
            ziel = args.ausgabe / f"{name}_vorschau.svg"
            ziel.write_text(erzeuge_vorschau(erg.weg, cfg, quelle, st), encoding="utf-8")
            erzeugt.append(ziel)

        print(f"Striche:       {st.striche}   Punkte: {st.punkte}")
        print(f"Zeichenweg:    {st.zeichenweg / 1000:.2f} m   Leerweg: {st.leerweg / 1000:.2f} m")
        print(f"Dauer (ca.):   {st.dauer_text()}")
        print("Erzeugt:")
        for d in erzeugt:
            print(f"  {d}")
        warnung = flash_warnung(erg.weg)
        if warnung and "header" in formate:
            print(f"WARNUNG: {warnung}")
        return 0
    except (KonfigFehler, ValueError, FileNotFoundError, ImportError) as e:
        print(f"Fehler: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":  # pragma: no cover
    sys.exit(main())
