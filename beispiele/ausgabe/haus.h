// Automatisch erzeugt mit deltaroboter-converter 0.1.0 - nicht von Hand bearbeiten.
// Quelle: haus.png (Modus: mittellinie)
// Zeichenflaeche 200 x 200 mm  |  27 Striche  |  187 Punkte  |  ca. 1.1 KB Flash
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
  {   -15,  -1979, STIFT_OBEN }, {  1662,  -1969, STIFT_UNTEN}, {  1698,  -1984, STIFT_UNTEN}, {  1723,  -2020, STIFT_UNTEN},
  {  1739,  -2097, STIFT_UNTEN}, {  1739,  -3375, STIFT_UNTEN}, {  1703,  -3467, STIFT_UNTEN}, {  1611,  -3503, STIFT_UNTEN},
  {    77,  -3503, STIFT_UNTEN}, {   -20,  -3477, STIFT_UNTEN}, {   -51,  -3426, STIFT_UNTEN}, {   -51,  -2045, STIFT_UNTEN},
  {   -41,  -2005, STIFT_UNTEN}, {   -15,  -1979, STIFT_UNTEN}, { -3119,  -2097, STIFT_OBEN }, { -3119,  -3375, STIFT_UNTEN},
  { -3155,  -3467, STIFT_UNTEN}, { -3247,  -3503, STIFT_UNTEN}, { -4781,  -3503, STIFT_UNTEN}, { -4878,  -3477, STIFT_UNTEN},
  { -4909,  -3426, STIFT_UNTEN}, { -4909,  -2045, STIFT_UNTEN}, { -4899,  -2005, STIFT_UNTEN}, { -4873,  -1979, STIFT_UNTEN},
  { -3196,  -1969, STIFT_UNTEN}, { -3160,  -1984, STIFT_UNTEN}, { -3135,  -2020, STIFT_UNTEN}, { -3119,  -2097, STIFT_UNTEN},
  { -5932,   -435, STIFT_OBEN }, { -6034,   -358, STIFT_UNTEN}, { -5952,   -164, STIFT_UNTEN}, { -5569,    251, STIFT_UNTEN},
  { -5272,    542, STIFT_UNTEN}, { -5057,    788, STIFT_UNTEN}, { -4761,   1079, STIFT_UNTEN}, { -4546,   1324, STIFT_UNTEN},
  { -4249,   1616, STIFT_UNTEN}, { -4035,   1861, STIFT_UNTEN}, { -3738,   2153, STIFT_UNTEN}, { -3012,   2935, STIFT_UNTEN},
  { -2204,   3764, STIFT_UNTEN}, { -1989,   4009, STIFT_UNTEN}, { -1734,   4265, STIFT_UNTEN}, { -1611,   4336, STIFT_UNTEN},
  { -1560,   4336, STIFT_UNTEN}, { -1468,   4290, STIFT_UNTEN}, { -1181,   4009, STIFT_UNTEN}, {  -966,   3764, STIFT_UNTEN},
  {  -670,   3472, STIFT_UNTEN}, {  -455,   3227, STIFT_UNTEN}, {  -159,   2935, STIFT_UNTEN}, {   568,   2153, STIFT_UNTEN},
  {   864,   1861, STIFT_UNTEN}, {  1079,   1616, STIFT_UNTEN}, {  1887,    787, STIFT_UNTEN}, {  2102,    542, STIFT_UNTEN},
  {  2398,    251, STIFT_UNTEN}, {  2613,      5, STIFT_UNTEN}, {  2756,   -133, STIFT_UNTEN}, {  2797,   -205, STIFT_UNTEN},
  {  2838,   -358, STIFT_UNTEN}, {  3043,   -440, STIFT_UNTEN}, {  3528,   -946, STIFT_UNTEN}, {  2838,   -358, STIFT_OBEN },
  {  2761,   -435, STIFT_UNTEN}, { -5932,   -435, STIFT_UNTEN}, { -5932,  -8003, STIFT_UNTEN}, { -5901,  -8064, STIFT_UNTEN},
  { -5778,  -8115, STIFT_UNTEN}, { -5702,  -8207, STIFT_UNTEN}, { -2608,  -8207, STIFT_UNTEN}, { -2608,  -4116, STIFT_UNTEN},
  { -2577,  -4045, STIFT_UNTEN}, { -2506,  -4014, STIFT_UNTEN}, {  -665,  -4014, STIFT_UNTEN}, {  -619,  -4030, STIFT_UNTEN},
  {  -588,  -4065, STIFT_UNTEN}, {  -562,  -4168, STIFT_UNTEN}, {  -562,  -8207, STIFT_UNTEN}, { -2608,  -8207, STIFT_UNTEN},
  {  -562,  -8207, STIFT_OBEN }, {  2531,  -8207, STIFT_UNTEN}, {  2618,  -8110, STIFT_UNTEN}, {  2720,  -8064, STIFT_UNTEN},
  {  2761,  -7926, STIFT_UNTEN}, {  2761,   -435, STIFT_UNTEN}, {  4602,   2940, STIFT_OBEN }, {  5037,   3656, STIFT_UNTEN},
  {  5267,   4147, STIFT_OBEN }, {  5068,   4301, STIFT_UNTEN}, {  4914,   4454, STIFT_UNTEN}, {  4812,   4587, STIFT_UNTEN},
  {  4725,   4730, STIFT_UNTEN}, {  4618,   4986, STIFT_UNTEN}, {  4572,   5165, STIFT_UNTEN}, {  4551,   5267, STIFT_UNTEN},
  {  4551,   5625, STIFT_UNTEN}, {  4572,   5727, STIFT_UNTEN}, {  4618,   5906, STIFT_UNTEN}, {  4669,   6034, STIFT_UNTEN},
  {  4812,   6305, STIFT_UNTEN}, {  4965,   6489, STIFT_UNTEN}, {  5093,   6617, STIFT_UNTEN}, {  5226,   6719, STIFT_UNTEN},
  {  5446,   6842, STIFT_UNTEN}, {  5625,   6914, STIFT_UNTEN}, {  5855,   6970, STIFT_UNTEN}, {  6264,   6980, STIFT_UNTEN},
  {  6366,   6960, STIFT_UNTEN}, {  6607,   6888, STIFT_UNTEN}, {  6801,   6806, STIFT_UNTEN}, {  6955,   6714, STIFT_UNTEN},
  {  7077,   6617, STIFT_UNTEN}, {  7231,   6464, STIFT_UNTEN}, {  7359,   6305, STIFT_UNTEN}, {  7476,   6095, STIFT_UNTEN},
  {  7548,   5906, STIFT_UNTEN}, {  7594,   5676, STIFT_UNTEN}, {  7619,   5420, STIFT_UNTEN}, {  7553,   4986, STIFT_UNTEN},
  {  7415,   4679, STIFT_UNTEN}, {  7231,   4428, STIFT_UNTEN}, {  7031,   4239, STIFT_UNTEN}, {  6801,   4086, STIFT_UNTEN},
  {  6545,   3978, STIFT_UNTEN}, {  6111,   3912, STIFT_UNTEN}, {  5855,   3937, STIFT_UNTEN}, {  5564,   4004, STIFT_UNTEN},
  {  5267,   4147, STIFT_UNTEN}, {  4270,   4423, STIFT_OBEN }, {  3989,   4249, STIFT_UNTEN}, {  3554,   4014, STIFT_UNTEN},
  {  3170,   5446, STIFT_OBEN }, {  4014,   5446, STIFT_UNTEN}, {  4270,   6494, STIFT_OBEN }, {  3554,   6929, STIFT_UNTEN},
  {  4602,   7977, STIFT_OBEN }, {  5037,   7261, STIFT_UNTEN}, {  6085,   7517, STIFT_OBEN }, {  6085,   8361, STIFT_UNTEN},
  {  7517,   7977, STIFT_OBEN }, {  7384,   7722, STIFT_UNTEN}, {  7108,   7261, STIFT_UNTEN}, {  7875,   6520, STIFT_OBEN },
  {  8131,   6653, STIFT_UNTEN}, {  8591,   6929, STIFT_UNTEN}, {  9000,   5446, STIFT_OBEN }, {  8156,   5446, STIFT_UNTEN},
  {  7875,   4423, STIFT_OBEN }, {  7977,   4352, STIFT_UNTEN}, {  8514,   4045, STIFT_UNTEN}, {  8591,   4014, STIFT_UNTEN},
  {  7517,   2940, STIFT_OBEN }, {  7384,   3196, STIFT_UNTEN}, {  7108,   3656, STIFT_UNTEN}, {  6085,   3375, STIFT_OBEN },
  {  6085,   2531, STIFT_UNTEN}, { -4116,   5446, STIFT_OBEN }, { -4858,   5446, STIFT_UNTEN}, { -5006,   5477, STIFT_UNTEN},
  { -5042,   5502, STIFT_UNTEN}, { -5062,   5574, STIFT_UNTEN}, { -5062,   7108, STIFT_UNTEN}, { -5523,   7159, STIFT_OBEN },
  { -6034,   7159, STIFT_UNTEN}, { -6545,   7159, STIFT_UNTEN}, { -7006,   7108, STIFT_OBEN }, { -7006,   6341, STIFT_UNTEN},
  { -8156,   6341, STIFT_UNTEN}, { -8156,   7108, STIFT_UNTEN}, { -8156,   6341, STIFT_OBEN }, { -8156,   5497, STIFT_UNTEN},
  { -7006,   5497, STIFT_OBEN }, { -7006,   6341, STIFT_UNTEN}, { -6034,   7159, STIFT_OBEN }, { -6034,   5497, STIFT_UNTEN},
  { -6034,   -358, STIFT_OBEN }, { -6213,   -440, STIFT_UNTEN}, { -6699,   -946, STIFT_UNTEN}, { -5702,  -8207, STIFT_OBEN },
  { -5814,  -8330, STIFT_UNTEN}, { -5906,  -8361, STIFT_UNTEN}, { -9000,  -8361, STIFT_UNTEN}, {  2531,  -8207, STIFT_OBEN },
  {  2659,  -8330, STIFT_UNTEN}, {  2736,  -8361, STIFT_UNTEN}, {  8310,  -8361, STIFT_UNTEN},
};

const DeltaZeichnung zeichnung_haus = {
  "haus",
  zeichnung_haus_punkte,
  sizeof(zeichnung_haus_punkte) / sizeof(zeichnung_haus_punkte[0]),
  0.00f, 10.00f,  // zZeichnen, zHeben
  0.00f, 0.00f, 30.00f,  // Startposition
  0.00f, 0.00f, 30.00f,  // Endposition
  1500.00f, 3000.00f, 600.00f  // Vorschub mm/min
};

#endif  // DELTA_ZEICHNUNG_HAUS_H
