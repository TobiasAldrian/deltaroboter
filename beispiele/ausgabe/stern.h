// Automatisch erzeugt mit deltaroboter-converter 0.1.0 - nicht von Hand bearbeiten.
// Quelle: stern.svg
// Zeichenflaeche 200 x 200 mm  |  4 Striche  |  155 Punkte  |  ca. 0.9 KB Flash
//
// Einbinden:  #include "stern.h"   und dann   zeichne(zeichnung_stern);

#ifndef DELTA_ZEICHNUNG_STERN_H
#define DELTA_ZEICHNUNG_STERN_H

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

const DeltaPunkt zeichnung_stern_punkte[] = {
  {     0,  -1400, STIFT_OBEN }, { -3400,  -3600, STIFT_UNTEN}, { -2200,      0, STIFT_UNTEN}, { -5200,   2400, STIFT_UNTEN},
  { -1400,   2400, STIFT_UNTEN}, {     0,   6000, STIFT_UNTEN}, {  1400,   2400, STIFT_UNTEN}, {  5200,   2400, STIFT_UNTEN},
  {  2200,      0, STIFT_UNTEN}, {  3400,  -3600, STIFT_UNTEN}, {     0,  -1400, STIFT_UNTEN}, {     0,  -5000, STIFT_OBEN },
  {  -584,  -4971, STIFT_UNTEN}, { -1175,  -4884, STIFT_UNTEN}, { -1742,  -4741, STIFT_UNTEN}, { -2293,  -4544, STIFT_UNTEN},
  { -2833,  -4289, STIFT_UNTEN}, { -3335,  -3988, STIFT_UNTEN}, { -3805,  -3640, STIFT_UNTEN}, { -4247,  -3238, STIFT_UNTEN},
  { -4640,  -2805, STIFT_UNTEN}, { -4988,  -2335, STIFT_UNTEN}, { -5295,  -1822, STIFT_UNTEN}, { -5544,  -1293, STIFT_UNTEN},
  { -5741,   -742, STIFT_UNTEN}, { -5884,   -175, STIFT_UNTEN}, { -5970,    403, STIFT_UNTEN}, { -6000,   1000, STIFT_UNTEN},
  { -5970,   1597, STIFT_UNTEN}, { -5884,   2175, STIFT_UNTEN}, { -5741,   2742, STIFT_UNTEN}, { -5544,   3293, STIFT_UNTEN},
  { -5295,   3822, STIFT_UNTEN}, { -4988,   4335, STIFT_UNTEN}, { -4640,   4805, STIFT_UNTEN}, { -4247,   5238, STIFT_UNTEN},
  { -3814,   5631, STIFT_UNTEN}, { -3335,   5988, STIFT_UNTEN}, { -2833,   6289, STIFT_UNTEN}, { -2293,   6544, STIFT_UNTEN},
  { -1730,   6745, STIFT_UNTEN}, { -1163,   6886, STIFT_UNTEN}, {  -584,   6971, STIFT_UNTEN}, {     0,   7000, STIFT_UNTEN},
  {   584,   6971, STIFT_UNTEN}, {  1163,   6886, STIFT_UNTEN}, {  1730,   6745, STIFT_UNTEN}, {  2293,   6544, STIFT_UNTEN},
  {  2822,   6295, STIFT_UNTEN}, {  3335,   5988, STIFT_UNTEN}, {  3805,   5640, STIFT_UNTEN}, {  4238,   5247, STIFT_UNTEN},
  {  4640,   4805, STIFT_UNTEN}, {  4988,   4335, STIFT_UNTEN}, {  5295,   3822, STIFT_UNTEN}, {  5544,   3293, STIFT_UNTEN},
  {  5741,   2742, STIFT_UNTEN}, {  5884,   2175, STIFT_UNTEN}, {  5971,   1584, STIFT_UNTEN}, {  6000,   1000, STIFT_UNTEN},
  {  5971,    416, STIFT_UNTEN}, {  5884,   -175, STIFT_UNTEN}, {  5741,   -742, STIFT_UNTEN}, {  5544,  -1293, STIFT_UNTEN},
  {  5295,  -1822, STIFT_UNTEN}, {  4988,  -2335, STIFT_UNTEN}, {  4640,  -2805, STIFT_UNTEN}, {  4247,  -3238, STIFT_UNTEN},
  {  3805,  -3640, STIFT_UNTEN}, {  3335,  -3988, STIFT_UNTEN}, {  2833,  -4289, STIFT_UNTEN}, {  2293,  -4544, STIFT_UNTEN},
  {  1742,  -4741, STIFT_UNTEN}, {  1175,  -4884, STIFT_UNTEN}, {   584,  -4971, STIFT_UNTEN}, {     0,  -5000, STIFT_UNTEN},
  { -8000,  -7000, STIFT_OBEN }, { -7744,  -6765, STIFT_UNTEN}, { -7487,  -6571, STIFT_UNTEN}, { -7231,  -6417, STIFT_UNTEN},
  { -6962,  -6294, STIFT_UNTEN}, { -6683,  -6205, STIFT_UNTEN}, { -6403,  -6153, STIFT_UNTEN}, { -6111,  -6134, STIFT_UNTEN},
  { -5797,  -6151, STIFT_UNTEN}, { -5540,  -6190, STIFT_UNTEN}, { -5272,  -6251, STIFT_UNTEN}, { -4677,  -6452, STIFT_UNTEN},
  { -4106,  -6702, STIFT_UNTEN}, { -2684,  -7395, STIFT_UNTEN}, { -2288,  -7562, STIFT_UNTEN}, { -1926,  -7691, STIFT_UNTEN},
  { -1530,  -7796, STIFT_UNTEN}, { -1168,  -7853, STIFT_UNTEN}, {  -819,  -7865, STIFT_UNTEN}, {  -492,  -7832, STIFT_UNTEN},
  {  -236,  -7773, STIFT_UNTEN}, {     9,  -7688, STIFT_UNTEN}, {   254,  -7571, STIFT_UNTEN}, {   499,  -7421, STIFT_UNTEN},
  {   802,  -7185, STIFT_UNTEN}, {  1471,  -6554, STIFT_UNTEN}, {  2017,  -6095, STIFT_UNTEN}, {  2585,  -5687, STIFT_UNTEN},
  {  3156,  -5346, STIFT_UNTEN}, {  3702,  -5086, STIFT_UNTEN}, {  4234,  -4901, STIFT_UNTEN}, {  4747,  -4789, STIFT_UNTEN},
  {  5250,  -4750, STIFT_UNTEN}, {  5717,  -4787, STIFT_UNTEN}, {  6156,  -4897, STIFT_UNTEN}, {  6364,  -4980, STIFT_UNTEN},
  {  6564,  -5081, STIFT_UNTEN}, {  6931,  -5332, STIFT_UNTEN}, {  7262,  -5654, STIFT_UNTEN}, {  7548,  -6035, STIFT_UNTEN},
  {  7795,  -6484, STIFT_UNTEN}, {  8000,  -7000, STIFT_UNTEN}, {  9000,  -7800, STIFT_OBEN }, {  8973,  -8051, STIFT_UNTEN},
  {  8900,  -8280, STIFT_UNTEN}, {  8783,  -8489, STIFT_UNTEN}, {  8617,  -8679, STIFT_UNTEN}, {  8447,  -8811, STIFT_UNTEN},
  {  8256,  -8910, STIFT_UNTEN}, {  8051,  -8973, STIFT_UNTEN}, {  7825,  -9000, STIFT_UNTEN}, { -7800,  -9000, STIFT_UNTEN},
  { -8039,  -8976, STIFT_UNTEN}, { -8280,  -8900, STIFT_UNTEN}, { -8489,  -8783, STIFT_UNTEN}, { -8679,  -8617, STIFT_UNTEN},
  { -8818,  -8436, STIFT_UNTEN}, { -8915,  -8245, STIFT_UNTEN}, { -8976,  -8039, STIFT_UNTEN}, { -9000,  -7825, STIFT_UNTEN},
  { -8998,   7863, STIFT_UNTEN}, { -8965,   8088, STIFT_UNTEN}, { -8890,   8303, STIFT_UNTEN}, { -8775,   8499, STIFT_UNTEN},
  { -8626,   8671, STIFT_UNTEN}, { -8447,   8811, STIFT_UNTEN}, { -8245,   8915, STIFT_UNTEN}, { -8026,   8978, STIFT_UNTEN},
  { -7800,   9000, STIFT_UNTEN}, {  7825,   9000, STIFT_UNTEN}, {  8064,   8971, STIFT_UNTEN}, {  8291,   8895, STIFT_UNTEN},
  {  8499,   8775, STIFT_UNTEN}, {  8679,   8617, STIFT_UNTEN}, {  8811,   8447, STIFT_UNTEN}, {  8910,   8256, STIFT_UNTEN},
  {  8976,   8039, STIFT_UNTEN}, {  9000,   7825, STIFT_UNTEN}, {  9000,  -7800, STIFT_UNTEN},
};

const DeltaZeichnung zeichnung_stern = {
  "stern",
  zeichnung_stern_punkte,
  sizeof(zeichnung_stern_punkte) / sizeof(zeichnung_stern_punkte[0]),
  0.00f, 10.00f,  // zZeichnen, zHeben
  0.00f, 0.00f, 30.00f,  // Startposition
  0.00f, 0.00f, 30.00f,  // Endposition
  1500.00f, 3000.00f, 600.00f  // Vorschub mm/min
};

#endif  // DELTA_ZEICHNUNG_STERN_H
