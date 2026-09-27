/*
  DeltaPlotter_Beispiel
  =====================
  Board:  OpenRB-150   (Tools > Board > OpenRB-150)

  Faehrt eine vom Converter erzeugte Zeichnung ab:
      Startposition  ->  alle Striche  ->  Endposition

  Eigene Zeichnung einbinden:
    1. python -m deltaconvert meinbild.png      (erzeugt ausgabe/meinbild.h)
    2. meinbild.h in diesen Sketch-Ordner kopieren
    3. unten  #include "meinbild.h"  und in setup()  zeichne(zeichnung_meinbild);

  Noch offen (kommt in spaeteren Schritten):
    fahreZu() steuert noch keine Motoren an. Dort kommen spaeter hin:
      1. Papier-Koordinaten -> Roboter-Koordinaten (Versatz zur Basis)
      2. inverse Kinematik  -> drei Motorwinkel
      3. Winkel an die Dynamixel XL430 senden (Bibliothek Dynamixel2Arduino)
    Bis dahin ist das ein "Trockenlauf": Die Positionen werden nur im
    Seriellen Monitor (115200 Baud) ausgegeben, der Roboter bewegt sich nicht.

  Koordinaten: mm, Ursprung = Mitte der Zeichenflaeche, Z = 0 auf dem Papier.
*/

#include "haus.h"  // vom Converter erzeugt

// ---- Einstellungen ------------------------------------------------------
const float SCHRITT_MM = 1.0f;       // Geraden werden in Stuecke dieser Laenge zerlegt
const bool AUSGABE_SERIELL = true;   // jede Zwischenposition im Seriellen Monitor ausgeben

// ---- Aktuelle Position (Papier-Koordinaten, mm) -------------------------
float posX = 0.0f, posY = 0.0f, posZ = 0.0f;

// Bewegt den Stift direkt auf (x, y, z).
// HIER kommt spaeter die Ansteuerung der drei Motoren hinein.
void fahreZu(float x, float y, float z) {
  // TODO 1: Papier-Koordinaten -> Roboter-Koordinaten
  // TODO 2: inverse Kinematik -> Winkel fuer Motor 1, 2, 3
  // TODO 3: dxl.setGoalPosition(id, winkel, UNIT_DEGREE) fuer alle drei Motoren
  if (AUSGABE_SERIELL) {
    Serial.print(x, 2);
    Serial.print('\t');
    Serial.print(y, 2);
    Serial.print('\t');
    Serial.println(z, 2);
  }
}

// Wartet die angegebene Zeit in Mikrosekunden (auch fuer Werte > 16 ms).
void warteMikros(unsigned long us) {
  delay(us / 1000UL);
  delayMicroseconds((unsigned int)(us % 1000UL));
}

// Faehrt auf einer Geraden zum Ziel.
// Ein Deltaroboter wuerde zwischen zwei weit entfernten Punkten sonst einen
// Bogen fahren, deshalb wird die Strecke in kurze Stuecke zerlegt.
void fahreLinear(float x, float y, float z, float vorschubMmProMin) {
  const float dx = x - posX, dy = y - posY, dz = z - posZ;
  const float laenge = sqrtf(dx * dx + dy * dy + dz * dz);
  int schritte = (int)ceilf(laenge / SCHRITT_MM);
  if (schritte < 1) schritte = 1;
  const float mmProSekunde = vorschubMmProMin / 60.0f;
  const unsigned long pauseUs = (unsigned long)((laenge / schritte) / mmProSekunde * 1000000.0f);
  const float x0 = posX, y0 = posY, z0 = posZ;
  for (int i = 1; i <= schritte; i++) {
    const float t = (float)i / (float)schritte;
    fahreZu(x0 + dx * t, y0 + dy * t, z0 + dz * t);
    warteMikros(pauseUs);
  }
  posX = x;
  posY = y;
  posZ = z;
}

// Faehrt eine komplette Zeichnung ab: Startposition -> Striche -> Endposition.
void zeichne(const DeltaZeichnung& zg) {
  // Annahme: Der Roboter steht bereits auf der Startposition.
  posX = zg.startX;
  posY = zg.startY;
  posZ = zg.startZ;
  fahreZu(posX, posY, posZ);

  bool stiftUnten = false;
  for (uint32_t i = 0; i < zg.anzahl; i++) {
    const DeltaPunkt& p = zg.punkte[i];
    const float x = p.x / (float)DELTA_EINHEITEN_PRO_MM;
    const float y = p.y / (float)DELTA_EINHEITEN_PRO_MM;
    if (p.stift == STIFT_UNTEN) {
      if (!stiftUnten) {  // Stift absetzen
        fahreLinear(posX, posY, zg.zZeichnen, zg.vorschubZ);
        stiftUnten = true;
      }
      fahreLinear(x, y, zg.zZeichnen, zg.vorschubZeichnen);
    } else {
      if (stiftUnten) {  // Stift heben
        fahreLinear(posX, posY, zg.zHeben, zg.vorschubZ);
        stiftUnten = false;
      }
      fahreLinear(x, y, zg.zHeben, zg.vorschubLeerfahrt);
    }
  }
  if (stiftUnten) {
    fahreLinear(posX, posY, zg.zHeben, zg.vorschubZ);
  }
  fahreLinear(zg.endX, zg.endY, zg.endZ, zg.vorschubLeerfahrt);
}

void setup() {
  Serial.begin(115200);
  const unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000UL) {
    // max. 3 s auf den Seriellen Monitor warten
  }

  Serial.print("Zeichnung: ");
  Serial.print(zeichnung_haus.name);
  Serial.print(" (");
  Serial.print(zeichnung_haus.anzahl);
  Serial.println(" Punkte)");
  Serial.println("x\ty\tz");

  zeichne(zeichnung_haus);

  Serial.println("Fertig.");
}

void loop() {
}
