// Typen fuer die vom Converter erzeugten Zeichnungen (.h).
// Identisch mit dem Block in jeder erzeugten Datei - der gemeinsame
// Include-Guard sorgt dafuer, dass sie nur einmal definiert werden.
#pragma once
#include <stdint.h>

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
