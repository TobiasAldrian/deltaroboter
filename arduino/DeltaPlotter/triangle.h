// Automatisch erzeugt mit deltaroboter-converter 0.1.0 - nicht von Hand bearbeiten.
// Quelle: triangle.png (Modus: mittellinie)
// Zeichenflaeche 210 x 297 mm  |  2 Striche  |  39 Punkte  |  ca. 0.2 KB Flash
//
// Einbinden:  #include "triangle.h"   und dann   zeichne(zeichnung_triangle);

#ifndef DELTA_ZEICHNUNG_TRIANGLE_H
#define DELTA_ZEICHNUNG_TRIANGLE_H

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

const DeltaPunkt zeichnung_triangle_punkte[] = {
  {  -376,   8425, STIFT_OBEN }, { -7665,  -4467, STIFT_UNTEN}, { -7719,  -4585, STIFT_UNTEN}, { -8174,  -5368, STIFT_UNTEN},
  { -8229,  -5486, STIFT_UNTEN}, { -8683,  -6270, STIFT_UNTEN}, { -8738,  -6387, STIFT_UNTEN}, { -9193,  -7171, STIFT_UNTEN},
  { -9248,  -7288, STIFT_UNTEN}, { -9702,  -8072, STIFT_UNTEN}, { -9929,  -8503, STIFT_UNTEN}, {-10000,  -8817, STIFT_UNTEN},
  { -9976,  -8903, STIFT_UNTEN}, { -9867,  -8934, STIFT_UNTEN}, {  9882,  -8934, STIFT_UNTEN}, {  9945,  -8918, STIFT_UNTEN},
  {  9984,  -8879, STIFT_UNTEN}, { 10000,  -8817, STIFT_UNTEN}, {  9929,  -8503, STIFT_UNTEN}, {  9882,  -8386, STIFT_UNTEN},
  {  9600,  -7876, STIFT_UNTEN}, {  9146,  -7092, STIFT_UNTEN}, {  9091,  -6975, STIFT_UNTEN}, {  8636,  -6191, STIFT_UNTEN},
  {  8582,  -6074, STIFT_UNTEN}, {  8127,  -5290, STIFT_UNTEN}, {  8072,  -5172, STIFT_UNTEN}, {  7618,  -4389, STIFT_UNTEN},
  {  7563,  -4271, STIFT_UNTEN}, {   274,   8621, STIFT_UNTEN}, {   227,   8691, STIFT_UNTEN}, {   165,   8746, STIFT_UNTEN},
  {    16,   8793, STIFT_UNTEN}, {   -47,   8793, STIFT_UNTEN}, {  -180,   8723, STIFT_UNTEN}, {  -235,   8660, STIFT_UNTEN},
  {  -376,   8425, STIFT_UNTEN}, {     8,   8817, STIFT_OBEN }, {     8,   8934, STIFT_UNTEN},
};

const DeltaZeichnung zeichnung_triangle = {
  "triangle",
  zeichnung_triangle_punkte,
  sizeof(zeichnung_triangle_punkte) / sizeof(zeichnung_triangle_punkte[0]),
  0.00f, 10.00f,  // zZeichnen, zHeben
  0.00f, 0.00f, 50.00f,  // Startposition
  0.00f, 0.00f, 50.00f,  // Endposition
  600.00f, 1200.00f, 300.00f  // Vorschub mm/min
};

#endif  // DELTA_ZEICHNUNG_TRIANGLE_H
