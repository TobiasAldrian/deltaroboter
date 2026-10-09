#pragma once
// =====================================================================
//  Bewegungen in Papier-Koordinaten (mm, Ursprung Blattmitte, Z = 0 auf dem Papier)
// =====================================================================

#include "Konfiguration.h"
#include "DeltaTypen.h"
#include "Kinematik.h"
#include "Motoren.h"

float posX = 0.0f, posY = 0.0f, posZ = 0.0f;  // aktuelle Sollposition (Papier-KS)
bool positionBekannt = false;                  // erst nach dem ersten Anfahren true
bool abgebrochen = false;

// Liest alle wartenden Zeichen weg (z. B. das zweite Zeilenende CR/LF).
void eingabeLeeren() {
  delay(30);
  while (Serial.available()) Serial.read();
}

// Beliebige Eingabe im Seriellen Monitor bricht eine laufende Bewegung ab.
bool abbruchGewuenscht() {
  if (Serial.available()) {
    eingabeLeeren();
    abgebrochen = true;
  }
  return abgebrochen;
}

void druckePosition(float x, float y, float z, const float phi[3]) {
  Serial.print(x, 2);
  Serial.print('\t');
  Serial.print(y, 2);
  Serial.print('\t');
  Serial.print(z, 2);
  for (uint8_t i = 0; i < 3; i++) {
    Serial.print('\t');
    Serial.print(phi[i], 2);
  }
  Serial.println();
}

void meldeNichtErreichbar(float x, float y, float z) {
  Serial.print("Nicht erreichbar: X=");
  Serial.print(x, 1);
  Serial.print(" Y=");
  Serial.print(y, 1);
  Serial.print(" Z=");
  Serial.print(z, 1);
  Serial.println("  (Geometrie, PHI_MIN/PHI_MAX, Z_MIN oder Papierlage pruefen)");
}

// Setzt den Stift direkt auf (x, y, z) - ohne Zwischenpunkte.
bool setzePosition(float x, float y, float z) {
  float phi[3];
  if (!winkelFuerPapierpunkt(x, y, z, phi)) {
    meldeNichtErreichbar(x, y, z);
    return false;
  }
  if (!sendeWinkel(phi)) return false;
  if (trockenlauf) druckePosition(x, y, z, phi);
  posX = x;
  posY = y;
  posZ = z;
  return true;
}

// Faehrt auf einer GERADEN zum Ziel. Ein Deltaroboter wuerde zwischen zwei
// Punkten sonst einen Bogen fahren, deshalb kommt alle TAKT_MS ein neuer
// Zwischenpunkt (bei 600 mm/min = 10 mm/s also alle 0,2 mm).
bool fahreLinear(float x, float y, float z, float vorschubMmProMin, bool abbrechbar = true) {
  if (!positionBekannt) {
    Serial.println("Position unbekannt - zuerst 's' (Startposition anfahren).");
    return false;
  }
  float phiZiel[3];
  if (!winkelFuerPapierpunkt(x, y, z, phiZiel)) {  // Ziel pruefen, BEVOR losgefahren wird
    meldeNichtErreichbar(x, y, z);
    return false;
  }
  const float dx = x - posX, dy = y - posY, dz = z - posZ;
  const float laenge = sqrtf(dx * dx + dy * dy + dz * dz);
  const float schrittMm = (vorschubMmProMin / 60.0f) * (TAKT_MS / 1000.0f);
  int32_t schritte = (int32_t)ceilf(laenge / schrittMm);
  if (schritte < 1) schritte = 1;
  const float x0 = posX, y0 = posY, z0 = posZ;
  uint32_t takt = micros();
  for (int32_t i = 1; i <= schritte; i++) {
    if (abbrechbar && abbruchGewuenscht()) return false;
    const float t = (float)i / (float)schritte;
    if (!setzePosition(x0 + dx * t, y0 + dy * t, z0 + dz * t)) return false;
    if (!trockenlauf) {  // im Trockenlauf nicht warten
      takt += TAKT_MS * 1000UL;
      while ((int32_t)(micros() - takt) < 0) {
      }
    }
  }
  return true;
}

// Erstes Anfahren aus unbekannter Lage: langsam, direkt im Gelenkraum.
bool fahreLangsamZu(float x, float y, float z) {
  float phi[3];
  if (!winkelFuerPapierpunkt(x, y, z, phi)) {
    meldeNichtErreichbar(x, y, z);
    return false;
  }
  drehmomentEin();
  setzeProfilGeschwindigkeit(PROFIL_ANFAHREN);
  const bool ok = sendeWinkel(phi);
  if (ok) warteBisAngekommen(15000);
  setzeProfilGeschwindigkeit(PROFIL_ZEICHNEN);
  if (!ok) return false;
  if (trockenlauf) druckePosition(x, y, z, phi);
  posX = x;
  posY = y;
  posZ = z;
  positionBekannt = true;
  return true;
}

inline float punktX(const DeltaPunkt& p) { return p.x / (float)DELTA_EINHEITEN_PRO_MM; }
inline float punktY(const DeltaPunkt& p) { return p.y / (float)DELTA_EINHEITEN_PRO_MM; }

// Prueft VOR dem Zeichnen, ob alle Punkte erreichbar sind.
bool pruefeZeichnung(const DeltaZeichnung& zg) {
  float phi[3];
  float phiMin = 1e9f, phiMax = -1e9f;
  if (!winkelFuerPapierpunkt(zg.startX, zg.startY, zg.startZ, phi)) {
    Serial.print("Startposition: ");
    meldeNichtErreichbar(zg.startX, zg.startY, zg.startZ);
    return false;
  }
  if (!winkelFuerPapierpunkt(zg.endX, zg.endY, zg.endZ, phi)) {
    Serial.print("Endposition: ");
    meldeNichtErreichbar(zg.endX, zg.endY, zg.endZ);
    return false;
  }
  for (uint32_t i = 0; i < zg.anzahl; i++) {
    const float x = punktX(zg.punkte[i]), y = punktY(zg.punkte[i]);
    const float hoehen[2] = {zg.zZeichnen, zg.zHeben};
    for (uint8_t h = 0; h < 2; h++) {
      if (!winkelFuerPapierpunkt(x, y, hoehen[h], phi)) {
        Serial.print("Punkt ");
        Serial.print(i);
        Serial.print(": ");
        meldeNichtErreichbar(x, y, hoehen[h]);
        return false;
      }
      for (uint8_t m = 0; m < 3; m++) {
        if (phi[m] < phiMin) phiMin = phi[m];
        if (phi[m] > phiMax) phiMax = phi[m];
      }
    }
  }
  Serial.print("Zeichnung '");
  Serial.print(zg.name);
  Serial.print("': ");
  Serial.print(zg.anzahl);
  Serial.print(" Punkte, alle erreichbar. Oberarmwinkel ");
  Serial.print(phiMin, 1);
  Serial.print(" .. ");
  Serial.print(phiMax, 1);
  Serial.println(" Grad");
  return true;
}

// Faehrt eine komplette Zeichnung ab: Startposition -> Striche -> Endposition.
bool zeichne(const DeltaZeichnung& zg) {
  if (!pruefeZeichnung(zg)) return false;
  eingabeLeeren();
  abgebrochen = false;
  Serial.println(trockenlauf ? "TROCKENLAUF (Motoren bewegen sich nicht):  x  y  z  phi1  phi2  phi3"
                             : "Zeichnen startet - beliebige Eingabe = ABBRUCH");

  // 1. Startposition (aus unbekannter Lage langsam anfahren)
  bool ok = positionBekannt ? fahreLinear(zg.startX, zg.startY, zg.startZ, zg.vorschubLeerfahrt)
                            : fahreLangsamZu(zg.startX, zg.startY, zg.startZ);

  // 2. Alle Punkte der Zeichnung
  bool stiftUnten = false;
  uint8_t letzteProzent = 0;
  for (uint32_t i = 0; ok && i < zg.anzahl; i++) {
    const DeltaPunkt& p = zg.punkte[i];
    const float x = punktX(p), y = punktY(p);
    if (p.stift == STIFT_UNTEN) {
      if (!stiftUnten) {  // Stift absetzen
        ok = fahreLinear(posX, posY, zg.zZeichnen, zg.vorschubZ);
        stiftUnten = true;
      }
      if (ok) ok = fahreLinear(x, y, zg.zZeichnen, zg.vorschubZeichnen);
    } else {
      if (stiftUnten) {  // Stift heben
        ok = fahreLinear(posX, posY, zg.zHeben, zg.vorschubZ);
        stiftUnten = false;
      }
      if (ok) ok = fahreLinear(x, y, zg.zHeben, zg.vorschubLeerfahrt);
    }
    const uint8_t prozent = (uint8_t)((i + 1) * 100UL / zg.anzahl);
    if (!trockenlauf && prozent >= letzteProzent + 10) {
      letzteProzent = prozent;
      Serial.print(prozent);
      Serial.println(" %");
    }
  }

  // 3. Stift heben und zur Endposition
  if (!ok) {
    Serial.println(abgebrochen ? "ABGEBROCHEN - Stift wird angehoben." : "FEHLER - Stift wird angehoben.");
    if (positionBekannt && posZ < zg.zHeben) fahreLinear(posX, posY, zg.zHeben, zg.vorschubZ, false);
    return false;
  }
  if (stiftUnten) ok = fahreLinear(posX, posY, zg.zHeben, zg.vorschubZ);
  if (ok) ok = fahreLinear(zg.endX, zg.endY, zg.endZ, zg.vorschubLeerfahrt);
  Serial.println(ok ? "Fertig." : "Abbruch bei der Fahrt zur Endposition.");
  return ok;
}
