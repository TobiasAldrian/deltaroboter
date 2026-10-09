// Automatisch erzeugt mit deltaroboter-converter 0.1.0 - nicht von Hand bearbeiten.
// Quelle: stern.svg
// Zeichenflaeche 210 x 297 mm  |  4 Striche  |  161 Punkte  |  ca. 0.9 KB Flash
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
  {     0,  -1556, STIFT_OBEN }, { -3778,  -4000, STIFT_UNTEN}, { -2444,      0, STIFT_UNTEN}, { -5778,   2667, STIFT_UNTEN},
  { -1556,   2667, STIFT_UNTEN}, {     0,   6667, STIFT_UNTEN}, {  1556,   2667, STIFT_UNTEN}, {  5778,   2667, STIFT_UNTEN},
  {  2444,      0, STIFT_UNTEN}, {  3778,  -4000, STIFT_UNTEN}, {     0,  -1556, STIFT_UNTEN}, {     0,  -5556, STIFT_OBEN },
  {  -649,  -5524, STIFT_UNTEN}, { -1306,  -5426, STIFT_UNTEN}, { -1936,  -5268, STIFT_UNTEN}, { -2548,  -5049, STIFT_UNTEN},
  { -3148,  -4765, STIFT_UNTEN}, { -3705,  -4431, STIFT_UNTEN}, { -4227,  -4044, STIFT_UNTEN}, { -4719,  -3598, STIFT_UNTEN},
  { -5164,  -3105, STIFT_UNTEN}, { -5550,  -2582, STIFT_UNTEN}, { -5883,  -2025, STIFT_UNTEN}, { -6161,  -1437, STIFT_UNTEN},
  { -6379,   -825, STIFT_UNTEN}, { -6538,   -195, STIFT_UNTEN}, { -6635,    462, STIFT_UNTEN}, { -6667,   1111, STIFT_UNTEN},
  { -6634,   1774, STIFT_UNTEN}, { -6538,   2417, STIFT_UNTEN}, { -6379,   3047, STIFT_UNTEN}, { -6161,   3659, STIFT_UNTEN},
  { -5883,   4247, STIFT_UNTEN}, { -5542,   4816, STIFT_UNTEN}, { -5155,   5338, STIFT_UNTEN}, { -4719,   5820, STIFT_UNTEN},
  { -4238,   6257, STIFT_UNTEN}, { -3705,   6653, STIFT_UNTEN}, { -3136,   6994, STIFT_UNTEN}, { -2548,   7272, STIFT_UNTEN},
  { -1936,   7490, STIFT_UNTEN}, { -1292,   7651, STIFT_UNTEN}, {  -649,   7746, STIFT_UNTEN}, {     0,   7778, STIFT_UNTEN},
  {   649,   7746, STIFT_UNTEN}, {  1292,   7651, STIFT_UNTEN}, {  1936,   7490, STIFT_UNTEN}, {  2548,   7272, STIFT_UNTEN},
  {  3136,   6994, STIFT_UNTEN}, {  3705,   6653, STIFT_UNTEN}, {  4238,   6257, STIFT_UNTEN}, {  4719,   5820, STIFT_UNTEN},
  {  5164,   5327, STIFT_UNTEN}, {  5550,   4805, STIFT_UNTEN}, {  5883,   4247, STIFT_UNTEN}, {  6161,   3659, STIFT_UNTEN},
  {  6379,   3047, STIFT_UNTEN}, {  6538,   2417, STIFT_UNTEN}, {  6635,   1760, STIFT_UNTEN}, {  6667,   1111, STIFT_UNTEN},
  {  6635,    462, STIFT_UNTEN}, {  6538,   -195, STIFT_UNTEN}, {  6379,   -825, STIFT_UNTEN}, {  6161,  -1437, STIFT_UNTEN},
  {  5883,  -2025, STIFT_UNTEN}, {  5550,  -2582, STIFT_UNTEN}, {  5164,  -3105, STIFT_UNTEN}, {  4719,  -3598, STIFT_UNTEN},
  {  4227,  -4044, STIFT_UNTEN}, {  3705,  -4431, STIFT_UNTEN}, {  3148,  -4765, STIFT_UNTEN}, {  2548,  -5049, STIFT_UNTEN},
  {  1936,  -5268, STIFT_UNTEN}, {  1306,  -5426, STIFT_UNTEN}, {   649,  -5524, STIFT_UNTEN}, {     0,  -5556, STIFT_UNTEN},
  { -8889,  -7778, STIFT_OBEN }, { -8604,  -7517, STIFT_UNTEN}, { -8319,  -7302, STIFT_UNTEN}, { -8034,  -7130, STIFT_UNTEN},
  { -7736,  -6993, STIFT_UNTEN}, { -7425,  -6894, STIFT_UNTEN}, { -7114,  -6836, STIFT_UNTEN}, { -6790,  -6816, STIFT_UNTEN},
  { -6441,  -6834, STIFT_UNTEN}, { -6156,  -6877, STIFT_UNTEN}, { -5858,  -6946, STIFT_UNTEN}, { -5534,  -7044, STIFT_UNTEN},
  { -5197,  -7168, STIFT_UNTEN}, { -4562,  -7447, STIFT_UNTEN}, { -2982,  -8216, STIFT_UNTEN}, { -2542,  -8402, STIFT_UNTEN},
  { -2140,  -8545, STIFT_UNTEN}, { -1700,  -8663, STIFT_UNTEN}, { -1298,  -8725, STIFT_UNTEN}, {  -910,  -8739, STIFT_UNTEN},
  {  -547,  -8702, STIFT_UNTEN}, {  -262,  -8637, STIFT_UNTEN}, {    10,  -8542, STIFT_UNTEN}, {   282,  -8412, STIFT_UNTEN},
  {   554,  -8245, STIFT_UNTEN}, {   891,  -7984, STIFT_UNTEN}, {  1634,  -7282, STIFT_UNTEN}, {  2242,  -6773, STIFT_UNTEN},
  {  2872,  -6319, STIFT_UNTEN}, {  3507,  -5940, STIFT_UNTEN}, {  4114,  -5651, STIFT_UNTEN}, {  4704,  -5445, STIFT_UNTEN},
  {  5275,  -5321, STIFT_UNTEN}, {  5551,  -5289, STIFT_UNTEN}, {  5833,  -5278, STIFT_UNTEN}, {  6096,  -5288, STIFT_UNTEN},
  {  6352,  -5319, STIFT_UNTEN}, {  6600,  -5370, STIFT_UNTEN}, {  6840,  -5441, STIFT_UNTEN}, {  7071,  -5533, STIFT_UNTEN},
  {  7294,  -5646, STIFT_UNTEN}, {  7507,  -5779, STIFT_UNTEN}, {  7701,  -5925, STIFT_UNTEN}, {  7894,  -6098, STIFT_UNTEN},
  {  8068,  -6282, STIFT_UNTEN}, {  8387,  -6706, STIFT_UNTEN}, {  8661,  -7205, STIFT_UNTEN}, {  8889,  -7778, STIFT_UNTEN},
  { 10000,  -8667, STIFT_OBEN }, {  9973,  -8932, STIFT_UNTEN}, {  9889,  -9200, STIFT_UNTEN}, {  9758,  -9432, STIFT_UNTEN},
  {  9574,  -9644, STIFT_UNTEN}, {  9385,  -9790, STIFT_UNTEN}, {  9174,  -9900, STIFT_UNTEN}, {  8932,  -9973, STIFT_UNTEN},
  {  8695, -10000, STIFT_UNTEN}, { -8667, -10000, STIFT_UNTEN}, { -8932,  -9973, STIFT_UNTEN}, { -9200,  -9889, STIFT_UNTEN},
  { -9432,  -9758, STIFT_UNTEN}, { -9644,  -9574, STIFT_UNTEN}, { -9790,  -9385, STIFT_UNTEN}, { -9900,  -9174, STIFT_UNTEN},
  { -9973,  -8932, STIFT_UNTEN}, {-10000,  -8695, STIFT_UNTEN}, { -9998,   8737, STIFT_UNTEN}, { -9961,   8987, STIFT_UNTEN},
  { -9877,   9225, STIFT_UNTEN}, { -9750,   9444, STIFT_UNTEN}, { -9584,   9634, STIFT_UNTEN}, { -9385,   9790, STIFT_UNTEN},
  { -9161,   9905, STIFT_UNTEN}, { -8918,   9976, STIFT_UNTEN}, { -8667,  10000, STIFT_UNTEN}, {  8695,  10000, STIFT_UNTEN},
  {  8959,   9967, STIFT_UNTEN}, {  9212,   9883, STIFT_UNTEN}, {  9444,   9750, STIFT_UNTEN}, {  9644,   9574, STIFT_UNTEN},
  {  9790,   9385, STIFT_UNTEN}, {  9900,   9174, STIFT_UNTEN}, {  9973,   8932, STIFT_UNTEN}, { 10000,   8695, STIFT_UNTEN},
  { 10000,  -8667, STIFT_UNTEN},
};

const DeltaZeichnung zeichnung_stern = {
  "stern",
  zeichnung_stern_punkte,
  sizeof(zeichnung_stern_punkte) / sizeof(zeichnung_stern_punkte[0]),
  0.00f, 10.00f,  // zZeichnen, zHeben
  0.00f, 0.00f, 50.00f,  // Startposition
  0.00f, 0.00f, 50.00f,  // Endposition
  600.00f, 1200.00f, 300.00f  // Vorschub mm/min
};

#endif  // DELTA_ZEICHNUNG_STERN_H
