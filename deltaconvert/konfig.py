"""Laden und Prüfen der Konfiguration (config/plotter.toml).

Jeder Abschnitt der TOML-Datei entspricht einer Dataclass unten. Fehlt ein
Eintrag in der Datei, gilt der Standardwert aus der Dataclass. Tippfehler bei
Abschnitten oder Einträgen werden als Fehler gemeldet, damit falsch
geschriebene Werte nicht still ignoriert werden.
"""

from __future__ import annotations

import tomllib
from dataclasses import dataclass, field, fields
from pathlib import Path

MODI = ("kontur", "mittellinie", "kanten")


class KonfigFehler(ValueError):
    """Fehler in der Konfigurationsdatei."""


@dataclass
class Zeichenflaeche:
    breite: float = 200.0  # mm
    hoehe: float = 200.0  # mm
    rand: float = 10.0  # mm, freier Rand innerhalb der Zeichenfläche


@dataclass
class Stift:
    z_zeichnen: float = 0.0  # mm, Stift berührt das Papier
    z_heben: float = 10.0  # mm, Höhe für Leerfahrten


@dataclass
class Vorschub:
    zeichnen: float = 1500.0  # mm/min
    leerfahrt: float = 3000.0  # mm/min
    z: float = 600.0  # mm/min, Stift heben/absetzen


@dataclass
class Position:
    x: float = 0.0
    y: float = 0.0
    z: float = 30.0


@dataclass
class Bild:
    modus: str = "kontur"  # kontur | mittellinie | kanten
    schwellwert: int = 0  # 0 = automatisch (Otsu), sonst 1..255
    invertieren: bool = False
    weichzeichnen: int = 3  # Kernelgröße Gauß-Filter (0 = aus)
    pixel_glaettung: int = 2  # gleitender Mittelwert über ±n Pixelpunkte
    max_aufloesung: int = 1500  # größere Bilder werden verkleinert (px)


@dataclass
class Pfade:
    vereinfachen: float = 0.1  # mm, Toleranz Douglas-Peucker
    min_laenge: float = 1.0  # mm, kürzere Striche werden verworfen
    verbinden_abstand: float = 0.3  # mm, näher → ohne Absetzen weiterzeichnen
    optimieren: bool = True  # Reihenfolge der Striche optimieren


@dataclass
class Ausrichtung:
    drehung: int = 0  # 0 | 90 | 180 | 270 Grad, gegen den Uhrzeigersinn
    spiegeln: bool = False  # an der Y-Achse spiegeln


@dataclass
class Konfig:
    zeichenflaeche: Zeichenflaeche = field(default_factory=Zeichenflaeche)
    stift: Stift = field(default_factory=Stift)
    vorschub: Vorschub = field(default_factory=Vorschub)
    startposition: Position = field(default_factory=Position)
    endposition: Position = field(default_factory=Position)
    bild: Bild = field(default_factory=Bild)
    pfade: Pfade = field(default_factory=Pfade)
    ausrichtung: Ausrichtung = field(default_factory=Ausrichtung)


def _wert_umwandeln(abschnitt: str, schluessel: str, wert, vorlage):
    """Wandelt einen TOML-Wert in den Typ des Standardwerts um."""
    ort = f"[{abschnitt}] {schluessel}"
    if isinstance(vorlage, bool):
        if not isinstance(wert, bool):
            raise KonfigFehler(f"{ort} muss true oder false sein, nicht {wert!r}.")
        return wert
    if isinstance(vorlage, int):
        if isinstance(wert, bool) or not isinstance(wert, (int, float)) or int(wert) != wert:
            raise KonfigFehler(f"{ort} muss eine ganze Zahl sein, nicht {wert!r}.")
        return int(wert)
    if isinstance(vorlage, float):
        if isinstance(wert, bool) or not isinstance(wert, (int, float)):
            raise KonfigFehler(f"{ort} muss eine Zahl sein, nicht {wert!r}.")
        return float(wert)
    if isinstance(vorlage, str):
        if not isinstance(wert, str):
            raise KonfigFehler(f"{ort} muss ein Text sein, nicht {wert!r}.")
        return wert
    raise KonfigFehler(f"{ort}: nicht unterstützter Typ.")  # pragma: no cover


def konfig_aus_dict(daten: dict) -> Konfig:
    """Erzeugt eine geprüfte Konfig aus einem (TOML-)Dictionary."""
    cfg = Konfig()
    abschnitte = {f.name for f in fields(cfg)}
    for abschnitt, werte in daten.items():
        if abschnitt not in abschnitte:
            raise KonfigFehler(
                f"Unbekannter Abschnitt [{abschnitt}]. Erlaubt: "
                + ", ".join(f"[{a}]" for a in sorted(abschnitte))
            )
        if not isinstance(werte, dict):
            raise KonfigFehler(f"[{abschnitt}] muss ein Abschnitt mit Einträgen sein.")
        ziel = getattr(cfg, abschnitt)
        erlaubt = {f.name for f in fields(ziel)}
        for schluessel, wert in werte.items():
            if schluessel not in erlaubt:
                raise KonfigFehler(
                    f"Unbekannter Eintrag '{schluessel}' in [{abschnitt}]. "
                    f"Erlaubt: {', '.join(sorted(erlaubt))}"
                )
            vorlage = getattr(ziel, schluessel)
            setattr(ziel, schluessel, _wert_umwandeln(abschnitt, schluessel, wert, vorlage))
    pruefen(cfg)
    return cfg


def lade_konfig(pfad: Path | None) -> Konfig:
    """Lädt die Konfiguration aus einer TOML-Datei (oder Standardwerte bei None)."""
    if pfad is None:
        cfg = Konfig()
        pruefen(cfg)
        return cfg
    pfad = Path(pfad)
    if not pfad.is_file():
        raise KonfigFehler(f"Konfigurationsdatei nicht gefunden: {pfad}")
    try:
        with open(pfad, "rb") as f:
            daten = tomllib.load(f)
    except tomllib.TOMLDecodeError as e:
        raise KonfigFehler(f"{pfad}: Syntaxfehler in der TOML-Datei: {e}") from e
    return konfig_aus_dict(daten)


def pruefen(cfg: Konfig) -> None:
    """Prüft die Werte auf Plausibilität und meldet Fehler verständlich."""
    zf = cfg.zeichenflaeche
    if zf.breite <= 0 or zf.hoehe <= 0:
        raise KonfigFehler("[zeichenflaeche] breite und hoehe müssen größer als 0 sein.")
    if zf.rand < 0:
        raise KonfigFehler("[zeichenflaeche] rand darf nicht negativ sein.")
    if zf.breite - 2 * zf.rand <= 0 or zf.hoehe - 2 * zf.rand <= 0:
        raise KonfigFehler("[zeichenflaeche] rand ist zu groß – es bleibt keine Fläche zum Zeichnen.")
    if max(zf.breite, zf.hoehe) / 2 > 327:
        raise KonfigFehler(
            "[zeichenflaeche] maximal 654 mm Kantenlänge (Grenze des Arduino-Formats: ±327 mm)."
        )
    if cfg.stift.z_heben <= cfg.stift.z_zeichnen:
        raise KonfigFehler("[stift] z_heben muss größer als z_zeichnen sein.")
    for name in ("zeichnen", "leerfahrt", "z"):
        if getattr(cfg.vorschub, name) <= 0:
            raise KonfigFehler(f"[vorschub] {name} muss größer als 0 sein.")
    b = cfg.bild
    if b.modus not in MODI:
        raise KonfigFehler(f"[bild] modus muss einer von {', '.join(MODI)} sein, nicht '{b.modus}'.")
    if not 0 <= b.schwellwert <= 255:
        raise KonfigFehler("[bild] schwellwert muss zwischen 0 und 255 liegen (0 = automatisch).")
    if b.weichzeichnen < 0 or b.pixel_glaettung < 0:
        raise KonfigFehler("[bild] weichzeichnen und pixel_glaettung dürfen nicht negativ sein.")
    if b.max_aufloesung < 100:
        raise KonfigFehler("[bild] max_aufloesung muss mindestens 100 Pixel sein.")
    p = cfg.pfade
    if p.vereinfachen < 0 or p.min_laenge < 0 or p.verbinden_abstand < 0:
        raise KonfigFehler("[pfade] Werte dürfen nicht negativ sein.")
    if cfg.ausrichtung.drehung not in (0, 90, 180, 270):
        raise KonfigFehler("[ausrichtung] drehung muss 0, 90, 180 oder 270 sein.")
