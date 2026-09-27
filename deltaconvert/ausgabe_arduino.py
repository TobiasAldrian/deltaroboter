"""Arduino-Header-Ausgabe (.h) zum direkten Einbinden in einen Sketch.

Die Punkte werden als int16 in 1/100 mm gespeichert (6 Byte pro Punkt).
Auf dem OpenRB-150 (SAMD21, 256 KB Flash) liegen `const`-Arrays automatisch im
Flash und belegen keinen RAM.

Die Typdefinitionen stehen hinter einem Include-Guard; so können mehrere
erzeugte Zeichnungen gleichzeitig in einen Sketch eingebunden werden.
"""

from __future__ import annotations

import re
import unicodedata

from .konfig import Konfig
from .werkzeugweg import STIFT_UNTEN, Statistik, Werkzeugweg

EINHEITEN_PRO_MM = 100
BYTES_PRO_PUNKT = 6
FLASH_WARNUNG_BYTES = 150_000  # OpenRB-150 hat 256 KB Flash, Rest braucht das Programm

TYPEN = """\
#ifndef DELTA_ZEICHNUNG_TYPEN
#define DELTA_ZEICHNUNG_TYPEN
// ---- Gemeinsame Typen aller erzeugten Zeichnungen (nicht aendern) ----
#define DELTA_EINHEITEN_PRO_MM 100  // Koordinaten in 1/100 mm

enum : uint8_t {
  STIFT_OBEN = 0,   // Leerfahrt zu diesem Punkt (Stift vorher heben)
  STIFT_UNTEN = 1   // mit abgesetztem Stift zu diesem Punkt zeichnen
};

struct DeltaPunkt {
  int16_t x;      // 1/100 mm, Ursprung = Mitte der Zeichenflaeche
  int16_t y;      // 1/100 mm
  uint8_t stift;  // STIFT_OBEN oder STIFT_UNTEN
};

struct DeltaZeichnung {
  const char* name;
  const DeltaPunkt* punkte;
  uint32_t anzahl;
  float zZeichnen, zHeben;                                  // mm
  float startX, startY, startZ;                             // mm
  float endX, endY, endZ;                                   // mm
  float vorschubZeichnen, vorschubLeerfahrt, vorschubZ;     // mm/min
};
#endif  // DELTA_ZEICHNUNG_TYPEN
"""


def c_bezeichner(name: str) -> str:
    """Macht aus einem Dateinamen einen gültigen C-Bezeichner (katze-2.png → katze_2)."""
    ersatz = {"ä": "ae", "ö": "oe", "ü": "ue", "Ä": "Ae", "Ö": "Oe", "Ü": "Ue", "ß": "ss"}
    for alt, neu in ersatz.items():
        name = name.replace(alt, neu)
    name = unicodedata.normalize("NFKD", name).encode("ascii", "ignore").decode()
    name = re.sub(r"[^0-9a-zA-Z_]+", "_", name).strip("_").lower()
    if not name:
        name = "zeichnung"
    if name[0].isdigit():
        name = "z_" + name
    return name


def _f(v: float) -> str:
    return f"{v:.2f}f"


def erzeuge_header(weg: Werkzeugweg, cfg: Konfig, name: str, quelle: str,
                   stat: Statistik, version: str) -> str:
    bez = c_bezeichner(name)
    werte = (weg.xy * EINHEITEN_PRO_MM).round().astype(int)
    if len(werte) and abs(werte).max() > 32767:
        raise ValueError("Koordinaten außerhalb von ±327 mm – Zeichenfläche verkleinern.")
    groesse = len(werte) * BYTES_PRO_PUNKT
    zf, st, v = cfg.zeichenflaeche, cfg.stift, cfg.vorschub
    sp, ep = cfg.startposition, cfg.endposition
    quelle_ascii = quelle.encode("ascii", "replace").decode()

    z: list[str] = []
    z.append(f"// Automatisch erzeugt mit deltaroboter-converter {version} - nicht von Hand bearbeiten.")
    z.append(f"// Quelle: {quelle_ascii}")
    z.append(f"// Zeichenflaeche {zf.breite:g} x {zf.hoehe:g} mm  |  {stat.striche} Striche  |  "
             f"{stat.punkte} Punkte  |  ca. {groesse / 1024:.1f} KB Flash")
    z.append("//")
    z.append("// Einbinden:  #include \"" + bez + ".h\"   und dann   zeichne(zeichnung_" + bez + ");")
    z.append("")
    z.append(f"#ifndef DELTA_ZEICHNUNG_{bez.upper()}_H")
    z.append(f"#define DELTA_ZEICHNUNG_{bez.upper()}_H")
    z.append("")
    z.append("#include <stdint.h>")
    z.append("")
    z.append(TYPEN)
    z.append(f"const DeltaPunkt zeichnung_{bez}_punkte[] = {{")
    pro_zeile = 4
    for i in range(0, len(werte), pro_zeile):
        teile = []
        for j in range(i, min(i + pro_zeile, len(werte))):
            x, y = werte[j]
            s = "STIFT_UNTEN" if weg.stift[j] == STIFT_UNTEN else "STIFT_OBEN "
            teile.append(f"{{{x:6d}, {y:6d}, {s}}}")
        z.append("  " + ", ".join(teile) + ",")
    z.append("};")
    z.append("")
    z.append(f"const DeltaZeichnung zeichnung_{bez} = {{")
    z.append(f"  \"{bez}\",")
    z.append(f"  zeichnung_{bez}_punkte,")
    z.append(f"  sizeof(zeichnung_{bez}_punkte) / sizeof(zeichnung_{bez}_punkte[0]),")
    z.append(f"  {_f(st.z_zeichnen)}, {_f(st.z_heben)},  // zZeichnen, zHeben")
    z.append(f"  {_f(sp.x)}, {_f(sp.y)}, {_f(sp.z)},  // Startposition")
    z.append(f"  {_f(ep.x)}, {_f(ep.y)}, {_f(ep.z)},  // Endposition")
    z.append(f"  {_f(v.zeichnen)}, {_f(v.leerfahrt)}, {_f(v.z)}  // Vorschub mm/min")
    z.append("};")
    z.append("")
    z.append(f"#endif  // DELTA_ZEICHNUNG_{bez.upper()}_H")
    return "\n".join(z) + "\n"


def flash_warnung(weg: Werkzeugweg) -> str | None:
    groesse = len(weg) * BYTES_PRO_PUNKT
    if groesse > FLASH_WARNUNG_BYTES:
        return (f"Die Zeichnung braucht ca. {groesse / 1024:.0f} KB Flash – das wird auf dem "
                "OpenRB-150 knapp. [pfade] vereinfachen erhöhen oder ein einfacheres Bild wählen.")
    return None
