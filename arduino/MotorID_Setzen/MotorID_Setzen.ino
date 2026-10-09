/*
  MotorID_Setzen
  ==============
  Hilfsprogramm fuer die Inbetriebnahme: findet die Dynamixel-Motoren am
  OpenRB-150, aendert ihre ID und laesst einzelne Motoren zur Erkennung blinken.

  Board:      OpenRB-150   |   Serieller Monitor: 115200 Baud, "Newline"

  Ab Werk haben ALLE XL430 die ID 1. Deshalb:
    1. NUR EINEN Motor anschliessen (12-V-Versorgung an!)
    2. "s"        -> sucht und zeigt die ID (z. B. 1)
    3. "i 1 2"    -> aendert ID 1 auf ID 2 (bleibt dauerhaft gespeichert)
    4. naechsten Motor alleine anschliessen, usw.
  Danach alle drei anschliessen, "s" muss die IDs 1, 2, 3 zeigen.
  "l 2" laesst Motor 2 blinken -> so findet man, welcher Motor welcher ist.
*/

#include <Dynamixel2Arduino.h>

#define DXL_SERIAL Serial1     // DXL-Anschluss des OpenRB-150
const int DXL_DIR_PIN = -1;
const uint32_t BAUDRATE = 57600;  // Werkseinstellung XL430

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

char zeile[32];
uint8_t laenge = 0;

void hilfe() {
  Serial.println();
  Serial.println("=== MotorID_Setzen - Befehle ===");
  Serial.println("  s            alle Motoren suchen");
  Serial.println("  i ALT NEU    ID aendern, z.B.  i 1 2");
  Serial.println("  l ID         Motor-LED 3 s leuchten lassen, z.B.  l 2");
  Serial.println("  ?            Hilfe");
}

void suchen() {
  Serial.println("Suche Motoren (ID 0..252) ...");
  uint8_t anzahl = 0;
  for (uint16_t id = 0; id <= 252; id++) {
    if (dxl.ping((uint8_t)id)) {
      Serial.print("  gefunden: ID ");
      Serial.print(id);
      Serial.print("  (Modellnummer ");
      Serial.print(dxl.getModelNumber((uint8_t)id));
      Serial.println(", XL430-W250 = 1060)");
      anzahl++;
    }
  }
  if (anzahl == 0) {
    Serial.println("  Kein Motor gefunden - 12-V-Versorgung und Kabel pruefen.");
  } else {
    Serial.print(anzahl);
    Serial.println(" Motor(en) gefunden.");
  }
}

void idAendern(long alt, long neu) {
  if (alt < 0 || alt > 252 || neu < 0 || neu > 252) {
    Serial.println("IDs muessen zwischen 0 und 252 liegen.");
    return;
  }
  if (!dxl.ping((uint8_t)alt)) {
    Serial.print("Kein Motor mit ID ");
    Serial.println(alt);
    return;
  }
  if (alt != neu && dxl.ping((uint8_t)neu)) {
    Serial.print("ID ");
    Serial.print(neu);
    Serial.println(" ist schon vergeben - zuerst nur EINEN Motor anschliessen.");
    return;
  }
  dxl.torqueOff((uint8_t)alt);  // ID kann nur ohne Drehmoment geaendert werden
  if (dxl.setID((uint8_t)alt, (uint8_t)neu) && dxl.ping((uint8_t)neu)) {
    Serial.print("OK: ID ");
    Serial.print(alt);
    Serial.print(" -> ");
    Serial.println(neu);
  } else {
    Serial.println("ID konnte nicht geaendert werden.");
  }
}

void blinken(long id) {
  if (id < 0 || id > 252 || !dxl.ping((uint8_t)id)) {
    Serial.println("Motor nicht gefunden.");
    return;
  }
  Serial.print("Motor ID ");
  Serial.print(id);
  Serial.println(" leuchtet 3 s ...");
  dxl.ledOn((uint8_t)id);
  delay(3000);
  dxl.ledOff((uint8_t)id);
}

void befehl(char* text) {
  while (*text == ' ') text++;
  char* rest;
  switch (text[0]) {
    case 's': case 'S':
      suchen();
      break;
    case 'i': case 'I': {
      const long alt = strtol(text + 1, &rest, 10);
      const long neu = strtol(rest, &rest, 10);
      idAendern(alt, neu);
      break;
    }
    case 'l': case 'L':
      blinken(strtol(text + 1, &rest, 10));
      break;
    default:
      hilfe();
  }
}

void setup() {
  Serial.begin(115200);
  const uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000) {
  }
  dxl.begin(BAUDRATE);
  dxl.setPortProtocolVersion(2.0);
  hilfe();
  suchen();
}

void loop() {
  while (Serial.available()) {
    const char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (laenge > 0) {
        zeile[laenge] = '\0';
        laenge = 0;
        befehl(zeile);
      }
    } else if (laenge < sizeof(zeile) - 1) {
      zeile[laenge++] = c;
    }
  }
}
