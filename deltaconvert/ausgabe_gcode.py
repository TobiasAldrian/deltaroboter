"""G-Code-Ausgabe (wie beim 3D-Druck).

Verwendete Befehle:
  G21  Einheit Millimeter
  G90  absolute Koordinaten
  G0   Leerfahrt (Stift oben)
  G1   Fahrt mit Vorschub (zeichnen bzw. Stift heben/absetzen)
  F    Vorschub in mm/min – wird immer angegeben, wenn er sich ändert

Kommentare sind absichtlich ohne Umlaute, weil manche Firmware nur ASCII
verträgt.
"""

from __future__ import annotations

from .konfig import Konfig
from .werkzeugweg import STIFT_UNTEN, Statistik, Werkzeugweg


def _z(v: float) -> str:
    return f"{v:.2f}"


def _f(v: float) -> str:
    return f"{v:g}"


def erzeuge_gcode(weg: Werkzeugweg, cfg: Konfig, quelle: str, stat: Statistik, version: str) -> str:
    zz, zh = cfg.stift.z_zeichnen, cfg.stift.z_heben
    v = cfg.vorschub
    sp, ep = cfg.startposition, cfg.endposition
    zf = cfg.zeichenflaeche

    z: list[str] = []
    z.append(f"; Erzeugt mit deltaroboter-converter {version}")
    z.append(f"; Quelle: {quelle.encode('ascii', 'replace').decode()}")
    z.append(f"; Zeichenflaeche: {zf.breite:g} x {zf.hoehe:g} mm, Ursprung = Mitte, Z = 0 auf dem Papier")
    z.append(f"; Striche: {stat.striche}  Punkte: {stat.punkte}  "
             f"Zeichenweg: {stat.zeichenweg:.0f} mm  Leerweg: {stat.leerweg:.0f} mm")
    z.append(f"; Geschaetzte Dauer: {stat.dauer_text().replace('ü', 'ue')}")
    z.append("G21 ; Einheit mm")
    z.append("G90 ; absolute Koordinaten")

    f_aktuell = None

    def mit_f(befehl: str, f: float) -> str:
        nonlocal f_aktuell
        if f_aktuell != f:
            f_aktuell = f
            return f"{befehl} F{_f(f)}"
        return befehl

    z.append(mit_f(f"G0 X{sp.x:.2f} Y{sp.y:.2f} Z{_z(sp.z)}", v.leerfahrt) + " ; Startposition")
    z_aktuell = sp.z
    stift_unten = False
    strich = 0
    for (x, y), stift in zip(weg.xy, weg.stift):
        if stift == STIFT_UNTEN:
            if not stift_unten:
                z.append(mit_f(f"G1 Z{_z(zz)}", v.z) + " ; Stift absetzen")
                z_aktuell = zz
                stift_unten = True
            z.append(mit_f(f"G1 X{x:.2f} Y{y:.2f}", v.zeichnen))
        else:
            if stift_unten:
                z.append(mit_f(f"G1 Z{_z(zh)}", v.z) + " ; Stift heben")
                z_aktuell = zh
                stift_unten = False
            strich += 1
            z.append(f"; Strich {strich}")
            ziel_z = f" Z{_z(zh)}" if z_aktuell != zh else ""
            z.append(mit_f(f"G0 X{x:.2f} Y{y:.2f}{ziel_z}", v.leerfahrt))
            z_aktuell = zh
    if stift_unten:
        z.append(mit_f(f"G1 Z{_z(zh)}", v.z) + " ; Stift heben")
    z.append(mit_f(f"G0 X{ep.x:.2f} Y{ep.y:.2f} Z{_z(ep.z)}", v.leerfahrt) + " ; Endposition")
    z.append("; Ende der Zeichnung")
    return "\n".join(z) + "\n"
