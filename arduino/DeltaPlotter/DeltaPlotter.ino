/*
  DeltaPlotter
  ============
  Deltaroboter als Stiftplotter - HTBLA Kaindorf, KOP-Projekt
  Board:       OpenRB-150   (Tools > Board > OpenRB-150)
  Bibliothek:  Dynamixel2Arduino
  Bedienung:   Serieller Monitor, 115200 Baud, Zeilenende "Newline"

  Ablauf beim Zeichnen:  Startposition -> alle Striche -> Endposition

  ZEICHNUNG WECHSELN
    1. Im Converter-Repo:  python -m deltaconvert bild.png
    2. ausgabe/bild.h in diesen Sketch-Ordner kopieren
    3. Unten bei "ZEICHNUNG" die zwei Zeilen anpassen:
         #include "bild.h"
         const DeltaZeichnung& ZEICHNUNG = zeichnung_bild;

  VOR DEM ERSTEN ECHTEN LAUF
    - Konfiguration.h: alle PLATZHALTER pruefen
    - 'k'  Nullposition und Richtung der Motoren kalibrieren
    - 'g 0 0 10', dann mehrmals '-'  -> Papierebene ermitteln
    - 'p'  pruefen, ob die ganze Zeichnung erreichbar ist
    - 'z'  zeichnen (erst mit Stift weit ueber dem Papier testen!)

  Werden die Motoren beim Start nicht gefunden, laeuft das Programm im
  TROCKENLAUF: Es rechnet alles durch und gibt Positionen und Winkel aus,
  bewegt aber nichts.
*/

#include "Konfiguration.h"
#include "DeltaTypen.h"
#include "Kinematik.h"
#include "Motoren.h"
#include "Bewegung.h"

// ============================ ZEICHNUNG =============================
#include "triangle.h"
const DeltaZeichnung& ZEICHNUNG = zeichnung_triangle;
// ====================================================================

char zeile[48];
uint8_t zeilenLaenge = 0;
uint32_t letztesZeichenMs = 0;

void hilfe() {
  Serial.println();
  Serial.println("=== DeltaPlotter - Befehle ===");
  Serial.println("  z          Zeichnung abfahren (Start -> Bild -> Ende)");
  Serial.println("  p          Zeichnung pruefen (alle Punkte erreichbar?)");
  Serial.println("  s          Startposition langsam anfahren");
  Serial.println("  g X Y Z    zu Papier-Koordinate fahren (mm), z.B.  g 0 0 10");
  Serial.println("  + / -      Stift um 0,5 mm heben / senken (Papierebene ermitteln)");
  Serial.println("  k          Kalibrieren: Drehmoment AUS, Motorwinkel anzeigen");
  Serial.println("  a          Drehmoment AUS (Roboter vorher festhalten!)");
  Serial.println("  t          Trockenlauf ein/aus");
  Serial.println("  ?          diese Hilfe");
  Serial.print("Modus: ");
  Serial.println(trockenlauf ? "TROCKENLAUF (Motoren bewegen sich nicht)" : "MOTOREN AKTIV");
  Serial.print("Zeichnung: ");
  Serial.print(ZEICHNUNG.name);
  Serial.print(" (");
  Serial.print(ZEICHNUNG.anzahl);
  Serial.println(" Punkte)");
}

// Zeigt laufend die Motorwinkel an, waehrend man die Oberarme von Hand bewegt.
void kalibrieren() {
  if (!motorenGefunden) {
    Serial.println("Keine Motoren verbunden.");
    return;
  }
  Serial.println("ACHTUNG: Drehmoment wird ausgeschaltet - Plattform festhalten!");
  delay(1500);
  drehmomentAus();
  positionBekannt = false;
  eingabeLeeren();
  Serial.println("Oberarm waagrecht stellen -> angezeigter Motorwinkel = NULLPOSITION.");
  Serial.println("Oberarm nach unten druecken: Wert wird groesser -> RICHTUNG +1, kleiner -> -1.");
  Serial.println("Beliebige Eingabe beendet die Anzeige.");
  while (!Serial.available()) {
    for (uint8_t i = 0; i < 3; i++) {
      const float grad = leseMotorGrad(i);
      Serial.print("M");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(grad, 1);
      Serial.print(" Grad (phi ");
      Serial.print(motorGradZuPhi(i, grad), 1);
      Serial.print(")   ");
    }
    Serial.println();
    delay(300);
  }
  eingabeLeeren();
  Serial.println("Kalibrierung beendet. Werte in Konfiguration.h eintragen und neu hochladen.");
}

void zeigeZ() {
  Serial.print("Stift Z = ");
  Serial.print(posZ, 2);
  Serial.print(" mm ueber Papier  |  Stiftspitze im Roboter-KS: z = ");
  Serial.print(PAPIEREBENE_Z + posZ, 2);
  Serial.println(" mm  (beruehrt der Stift das Papier -> diesen Wert als PAPIEREBENE_Z eintragen)");
}

// Liest drei Zahlen "X Y Z" - gibt false zurueck, wenn eine fehlt.
bool leseDreiZahlen(char* text, float& x, float& y, float& z) {
  char* ende;
  x = strtod(text, &ende);
  if (ende == text) return false;
  text = ende;
  y = strtod(text, &ende);
  if (ende == text) return false;
  text = ende;
  z = strtod(text, &ende);
  return ende != text;
}

void befehlAusfuehren(char* text) {
  while (*text == ' ') text++;
  char befehl = text[0];
  if (befehl >= 'A' && befehl <= 'Z') befehl += 'a' - 'A';  // Grossbuchstaben erlauben
  switch (befehl) {
    case 'z':
      zeichne(ZEICHNUNG);
      break;
    case 'p':
      pruefeZeichnung(ZEICHNUNG);
      break;
    case 's':
      if (fahreLangsamZu(ZEICHNUNG.startX, ZEICHNUNG.startY, ZEICHNUNG.startZ)) {
        Serial.println("Startposition erreicht.");
      }
      break;
    case 'g': {
      float x, y, z;
      if (!leseDreiZahlen(text + 1, x, y, z)) {
        Serial.println("Bitte so eingeben:  g X Y Z   (z.B.  g 0 0 10)");
        break;
      }
      eingabeLeeren();
      abgebrochen = false;
      const bool ok = positionBekannt ? fahreLinear(x, y, z, VORSCHUB_HAND) : fahreLangsamZu(x, y, z);
      if (ok) zeigeZ();
      else if (abgebrochen) Serial.println("Abgebrochen.");
      break;
    }
    case '+':
    case '-': {
      abgebrochen = false;
      const float dz = (befehl == '+') ? JOG_SCHRITT_MM : -JOG_SCHRITT_MM;
      if (fahreLinear(posX, posY, posZ + dz, VORSCHUB_HAND, false)) zeigeZ();
      break;
    }
    case 'k':
      kalibrieren();
      break;
    case 'a':
      drehmomentAus();
      positionBekannt = false;
      Serial.println("Drehmoment aus.");
      break;
    case 't':
      if (!motorenGefunden) {
        Serial.println("Keine Motoren gefunden - nur Trockenlauf moeglich.");
        break;
      }
      trockenlauf = !trockenlauf;
      positionBekannt = false;
      if (trockenlauf) drehmomentAus();
      Serial.println(trockenlauf ? "Trockenlauf EIN" : "Motoren AKTIV - naechster Befehl faehrt langsam an.");
      break;
    case '?':
    case 'h':
      hilfe();
      break;
    default:
      Serial.print("Unbekannter Befehl: ");
      Serial.println(text);
      Serial.println("'?' zeigt die Hilfe.");
  }
}

void setup() {
  Serial.begin(115200);
  const uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000) {
    // max. 3 s auf den Seriellen Monitor warten
  }
  Serial.println();
  Serial.println("DeltaPlotter startet ...");
  motorenGefunden = motorenStarten();
  trockenlauf = !motorenGefunden;
  Serial.println(motorenGefunden ? "Alle 3 Motoren gefunden (Drehmoment noch AUS)."
                                 : "Motoren nicht gefunden -> TROCKENLAUF.");
  hilfe();
}

void loop() {
  while (Serial.available()) {
    const char c = Serial.read();
    letztesZeichenMs = millis();
    if (c == '\n' || c == '\r') {
      if (zeilenLaenge > 0) {
        zeile[zeilenLaenge] = '\0';
        zeilenLaenge = 0;
        befehlAusfuehren(zeile);
      }
    } else if (zeilenLaenge < sizeof(zeile) - 1) {
      zeile[zeilenLaenge++] = c;
    }
  }
  // Funktioniert auch mit "Kein Zeilenende" im Seriellen Monitor
  if (zeilenLaenge > 0 && millis() - letztesZeichenMs > 200) {
    zeile[zeilenLaenge] = '\0';
    zeilenLaenge = 0;
    befehlAusfuehren(zeile);
  }
}
