// Automatisch erzeugt mit deltaroboter-converter 0.1.0 - nicht von Hand bearbeiten.
// Quelle: haus.png (Modus: mittellinie)
// Zeichenflaeche 210 x 297 mm  |  27 Striche  |  205 Punkte  |  ca. 1.2 KB Flash
//
// Einbinden:  #include "haus.h"   und dann   zeichne(zeichnung_haus);

#ifndef DELTA_ZEICHNUNG_HAUS_H
#define DELTA_ZEICHNUNG_HAUS_H

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

const DeltaPunkt zeichnung_haus_punkte[] = {
  {    28,  -2188, STIFT_OBEN }, {  1847,  -2188, STIFT_UNTEN}, {  1886,  -2205, STIFT_UNTEN}, {  1915,  -2244, STIFT_UNTEN},
  {  1932,  -2330, STIFT_UNTEN}, {  1932,  -3750, STIFT_UNTEN}, {  1920,  -3807, STIFT_UNTEN}, {  1892,  -3852, STIFT_UNTEN},
  {  1847,  -3881, STIFT_UNTEN}, {  1790,  -3892, STIFT_UNTEN}, {    85,  -3892, STIFT_UNTEN}, {   -23,  -3864, STIFT_UNTEN},
  {   -57,  -3807, STIFT_UNTEN}, {   -57,  -2273, STIFT_UNTEN}, {   -45,  -2227, STIFT_UNTEN}, {   -17,  -2199, STIFT_UNTEN},
  {    28,  -2188, STIFT_UNTEN}, {  3068,   -483, STIFT_OBEN }, {  3153,   -398, STIFT_UNTEN}, {  3108,   -227, STIFT_UNTEN},
  {  3063,   -148, STIFT_UNTEN}, {  2903,      6, STIFT_UNTEN}, {  2665,    278, STIFT_UNTEN}, {  2335,    602, STIFT_UNTEN},
  {  2097,    875, STIFT_UNTEN}, {  1199,   1795, STIFT_UNTEN}, {   960,   2068, STIFT_UNTEN}, {   631,   2392, STIFT_UNTEN},
  {   392,   2665, STIFT_UNTEN}, {    63,   2989, STIFT_UNTEN}, {  -176,   3261, STIFT_UNTEN}, {  -506,   3585, STIFT_UNTEN},
  {  -744,   3858, STIFT_UNTEN}, { -1074,   4182, STIFT_UNTEN}, { -1313,   4455, STIFT_UNTEN}, { -1631,   4767, STIFT_UNTEN},
  { -1733,   4818, STIFT_UNTEN}, { -1790,   4818, STIFT_UNTEN}, { -1926,   4739, STIFT_UNTEN}, { -2210,   4455, STIFT_UNTEN},
  { -2449,   4182, STIFT_UNTEN}, { -3347,   3261, STIFT_UNTEN}, { -3585,   2989, STIFT_UNTEN}, { -3915,   2665, STIFT_UNTEN},
  { -4153,   2392, STIFT_UNTEN}, { -4483,   2068, STIFT_UNTEN}, { -4722,   1795, STIFT_UNTEN}, { -5051,   1472, STIFT_UNTEN},
  { -5290,   1199, STIFT_UNTEN}, { -5619,    875, STIFT_UNTEN}, { -5858,    602, STIFT_UNTEN}, { -6188,    278, STIFT_UNTEN},
  { -6426,      6, STIFT_UNTEN}, { -6614,   -182, STIFT_UNTEN}, { -6705,   -398, STIFT_UNTEN}, { -6591,   -483, STIFT_UNTEN},
  {  3068,   -483, STIFT_UNTEN}, {  3068,  -8807, STIFT_UNTEN}, {  3045,  -8920, STIFT_UNTEN}, {  3023,  -8960, STIFT_UNTEN},
  {  2909,  -9011, STIFT_UNTEN}, {  2813,  -9119, STIFT_UNTEN}, {  -625,  -9119, STIFT_UNTEN}, {  -625,  -4631, STIFT_UNTEN},
  {  -653,  -4517, STIFT_UNTEN}, {  -687,  -4477, STIFT_UNTEN}, {  -739,  -4460, STIFT_UNTEN}, { -2784,  -4460, STIFT_UNTEN},
  { -2864,  -4494, STIFT_UNTEN}, { -2898,  -4574, STIFT_UNTEN}, { -2898,  -9119, STIFT_UNTEN}, { -6335,  -9119, STIFT_UNTEN},
  { -6369,  -9063, STIFT_UNTEN}, { -6420,  -9017, STIFT_UNTEN}, { -6557,  -8960, STIFT_UNTEN}, { -6591,  -8892, STIFT_UNTEN},
  { -6591,   -483, STIFT_UNTEN}, { -6705,   -398, STIFT_OBEN }, { -6903,   -489, STIFT_UNTEN}, { -7443,  -1051, STIFT_UNTEN},
  { -5443,  -2227, STIFT_OBEN }, { -5415,  -2199, STIFT_UNTEN}, { -5369,  -2188, STIFT_UNTEN}, { -3551,  -2188, STIFT_UNTEN},
  { -3511,  -2205, STIFT_UNTEN}, { -3483,  -2244, STIFT_UNTEN}, { -3466,  -2330, STIFT_UNTEN}, { -3466,  -3750, STIFT_UNTEN},
  { -3477,  -3807, STIFT_UNTEN}, { -3506,  -3852, STIFT_UNTEN}, { -3551,  -3881, STIFT_UNTEN}, { -3608,  -3892, STIFT_UNTEN},
  { -5312,  -3892, STIFT_UNTEN}, { -5420,  -3864, STIFT_UNTEN}, { -5455,  -3807, STIFT_UNTEN}, { -5455,  -2273, STIFT_UNTEN},
  { -5443,  -2227, STIFT_UNTEN}, { -6335,  -9119, STIFT_OBEN }, { -6460,  -9256, STIFT_UNTEN}, { -6562,  -9290, STIFT_UNTEN},
  {-10000,  -9290, STIFT_UNTEN}, { -2898,  -9119, STIFT_OBEN }, {  -625,  -9119, STIFT_UNTEN}, {  2813,  -9119, STIFT_OBEN },
  {  2955,  -9256, STIFT_UNTEN}, {  3040,  -9290, STIFT_UNTEN}, {  9233,  -9290, STIFT_UNTEN}, {  3920,  -1051, STIFT_OBEN },
  {  3381,   -489, STIFT_UNTEN}, {  3324,   -455, STIFT_UNTEN}, {  3153,   -398, STIFT_UNTEN}, {  5114,   3267, STIFT_OBEN },
  {  5597,   4063, STIFT_UNTEN}, {  5852,   4608, STIFT_OBEN }, {  5631,   4778, STIFT_UNTEN}, {  5460,   4949, STIFT_UNTEN},
  {  5347,   5097, STIFT_UNTEN}, {  5250,   5256, STIFT_UNTEN}, {  5131,   5540, STIFT_UNTEN}, {  5080,   5739, STIFT_UNTEN},
  {  5057,   5852, STIFT_UNTEN}, {  5057,   6250, STIFT_UNTEN}, {  5080,   6364, STIFT_UNTEN}, {  5131,   6562, STIFT_UNTEN},
  {  5188,   6705, STIFT_UNTEN}, {  5347,   7006, STIFT_UNTEN}, {  5517,   7210, STIFT_UNTEN}, {  5659,   7352, STIFT_UNTEN},
  {  5807,   7466, STIFT_UNTEN}, {  6051,   7602, STIFT_UNTEN}, {  6250,   7682, STIFT_UNTEN}, {  6506,   7744, STIFT_UNTEN},
  {  6960,   7756, STIFT_UNTEN}, {  7074,   7733, STIFT_UNTEN}, {  7341,   7653, STIFT_UNTEN}, {  7557,   7562, STIFT_UNTEN},
  {  7727,   7460, STIFT_UNTEN}, {  7864,   7352, STIFT_UNTEN}, {  8034,   7182, STIFT_UNTEN}, {  8176,   7006, STIFT_UNTEN},
  {  8307,   6773, STIFT_UNTEN}, {  8386,   6562, STIFT_UNTEN}, {  8438,   6307, STIFT_UNTEN}, {  8466,   6023, STIFT_UNTEN},
  {  8392,   5540, STIFT_UNTEN}, {  8239,   5199, STIFT_UNTEN}, {  8176,   5097, STIFT_UNTEN}, {  8034,   4920, STIFT_UNTEN},
  {  7812,   4710, STIFT_UNTEN}, {  7557,   4540, STIFT_UNTEN}, {  7273,   4420, STIFT_UNTEN}, {  6790,   4347, STIFT_UNTEN},
  {  6506,   4375, STIFT_UNTEN}, {  6182,   4449, STIFT_UNTEN}, {  6051,   4500, STIFT_UNTEN}, {  5852,   4608, STIFT_UNTEN},
  {  4744,   4915, STIFT_OBEN }, {  4432,   4722, STIFT_UNTEN}, {  4034,   4494, STIFT_UNTEN}, {  3949,   4460, STIFT_UNTEN},
  {  3523,   6051, STIFT_OBEN }, {  4460,   6051, STIFT_UNTEN}, {  4744,   7216, STIFT_OBEN }, {  3949,   7699, STIFT_UNTEN},
  {  5114,   8864, STIFT_OBEN }, {  5597,   8068, STIFT_UNTEN}, {  6761,   8352, STIFT_OBEN }, {  6761,   9290, STIFT_UNTEN},
  {  8352,   8864, STIFT_OBEN }, {  8205,   8580, STIFT_UNTEN}, {  7898,   8068, STIFT_UNTEN}, {  8750,   7244, STIFT_OBEN },
  {  9034,   7392, STIFT_UNTEN}, {  9545,   7699, STIFT_UNTEN}, { 10000,   6051, STIFT_OBEN }, {  9063,   6051, STIFT_UNTEN},
  {  8750,   4915, STIFT_OBEN }, {  8864,   4835, STIFT_UNTEN}, {  9460,   4494, STIFT_UNTEN}, {  9545,   4460, STIFT_UNTEN},
  {  8352,   3267, STIFT_OBEN }, {  8318,   3352, STIFT_UNTEN}, {  7977,   3949, STIFT_UNTEN}, {  7898,   4063, STIFT_UNTEN},
  {  6761,   3750, STIFT_OBEN }, {  6761,   2813, STIFT_UNTEN}, { -4574,   6051, STIFT_OBEN }, { -5398,   6051, STIFT_UNTEN},
  { -5563,   6085, STIFT_UNTEN}, { -5602,   6114, STIFT_UNTEN}, { -5625,   6193, STIFT_UNTEN}, { -5625,   7898, STIFT_UNTEN},
  { -6136,   7955, STIFT_OBEN }, { -6705,   7955, STIFT_UNTEN}, { -7273,   7955, STIFT_UNTEN}, { -7784,   7898, STIFT_OBEN },
  { -7784,   7045, STIFT_UNTEN}, { -9063,   7045, STIFT_UNTEN}, { -9063,   7898, STIFT_UNTEN}, { -9063,   7045, STIFT_OBEN },
  { -9063,   6108, STIFT_UNTEN}, { -7784,   6108, STIFT_OBEN }, { -7784,   7045, STIFT_UNTEN}, { -6705,   7955, STIFT_OBEN },
  { -6705,   6108, STIFT_UNTEN},
};

const DeltaZeichnung zeichnung_haus = {
  "haus",
  zeichnung_haus_punkte,
  sizeof(zeichnung_haus_punkte) / sizeof(zeichnung_haus_punkte[0]),
  0.00f, 10.00f,  // zZeichnen, zHeben
  0.00f, 0.00f, 50.00f,  // Startposition
  0.00f, 0.00f, 50.00f,  // Endposition
  600.00f, 1200.00f, 300.00f  // Vorschub mm/min
};

#endif  // DELTA_ZEICHNUNG_HAUS_H
