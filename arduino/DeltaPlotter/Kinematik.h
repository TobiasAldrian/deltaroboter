#pragma once
// =====================================================================
//  Kinematik des Deltaroboters
// =====================================================================
//  Formeln wie im Berechnungsbericht, Abschnitt "Inverse Kinematik":
//    d_i  = y cos(theta_i) - x sin(theta_i)          Abstand zur Armebene
//    e_i  = r_P - r_B + x cos(theta_i) + y sin(theta_i)
//    b_i' = sqrt(b^2 - d_i^2)                        projizierter Unterarm
//    c_i' = sqrt(e_i^2 + z^2)
//    alpha_i = acos(e_i / c_i')
//    beta_i  = acos((a^2 + c_i'^2 - b_i'^2) / (2 a c_i'))
//    phi_i   = alpha_i - beta_i                      (Ellbogen nach aussen)
// =====================================================================

#include <math.h>
#include "Konfiguration.h"

const float RAD_PRO_GRAD = 0.017453292519943f;

// Plattformposition (Gelenkebene) im Roboter-KS -> drei Oberarmwinkel in Grad.
// Gibt false zurueck, wenn die Position geometrisch nicht erreichbar ist.
inline bool inverseKinematik(float x, float y, float z, float phi[3]) {
  if (z >= -1.0f) return false;  // Plattform muss unter der Basis liegen
  for (int i = 0; i < 3; i++) {
    const float th = MOTOR_THETA[i] * RAD_PRO_GRAD;
    const float c = cosf(th);
    const float s = sinf(th);
    const float d = y * c - x * s;
    const float e = RADIUS_PLATTFORM - RADIUS_BASIS + x * c + y * s;
    if (fabsf(d) >= UNTERARM_B) return false;
    const float bStrich = sqrtf(UNTERARM_B * UNTERARM_B - d * d);
    const float cStrich = sqrtf(e * e + z * z);
    const float cosBeta = (OBERARM_A * OBERARM_A + cStrich * cStrich - bStrich * bStrich)
                          / (2.0f * OBERARM_A * cStrich);
    if (cosBeta < -1.0f || cosBeta > 1.0f) return false;
    const float alpha = acosf(e / cStrich);
    const float beta = acosf(cosBeta);
    phi[i] = (alpha - beta) / RAD_PRO_GRAD;
  }
  return true;
}

// Papier-Koordinaten (vom Converter: Ursprung Blattmitte, Z = 0 auf dem Papier)
// -> Plattformposition im Roboter-KS.
inline void papierZuRoboter(float x, float y, float z, float& xR, float& yR, float& zR) {
  const float dr = PAPIER_DREHUNG * RAD_PRO_GRAD;
  const float c = cosf(dr);
  const float s = sinf(dr);
  xR = PAPIER_MITTE_X + c * x - s * y;
  yR = PAPIER_MITTE_Y + s * x + c * y;
  zR = PAPIEREBENE_Z + z + STIFT_UEBERSTAND;  // Stiftspitze liegt STIFT_UEBERSTAND unter der Gelenkebene
}

// Papier-Koordinaten -> Oberarmwinkel, inklusive Pruefung von Z_MIN und der erlaubten Winkel.
inline bool winkelFuerPapierpunkt(float x, float y, float z, float phi[3]) {
  if (z < Z_MIN) return false;
  float xR, yR, zR;
  papierZuRoboter(x, y, z, xR, yR, zR);
  if (!inverseKinematik(xR, yR, zR, phi)) return false;
  for (int i = 0; i < 3; i++) {
    if (phi[i] < PHI_MIN || phi[i] > PHI_MAX) return false;
  }
  return true;
}
