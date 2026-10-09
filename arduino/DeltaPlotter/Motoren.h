#pragma once
// =====================================================================
//  Ansteuerung der drei Dynamixel XL430-W250-T ueber das OpenRB-150
// =====================================================================

#include <Dynamixel2Arduino.h>
#include "Konfiguration.h"

#define DXL_SERIAL Serial1       // DXL-Anschluss des OpenRB-150
const int DXL_DIR_PIN = -1;      // OpenRB-150 braucht keinen DIR-Pin

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

bool motorenGefunden = false;    // haben beim Start alle drei Motoren geantwortet?
bool trockenlauf = true;         // true = Motoren werden NICHT angesteuert, nur Ausgabe
bool drehmomentIstAn = false;

// ---- SyncWrite: alle drei Sollpositionen in EINEM Paket senden ----------
const uint16_t ADR_GOAL_POSITION = 116;  // Control Table XL430: Goal Position, 4 Byte
const uint16_t LEN_GOAL_POSITION = 4;

struct SyncDaten {
  int32_t sollPosition;
} __attribute__((packed));

SyncDaten syncDaten[3];
DYNAMIXEL::InfoSyncWriteInst_t syncInfo;
DYNAMIXEL::XELInfoSyncWrite_t syncXel[3];

// ---- Umrechnungen -------------------------------------------------------
// XL430: 4096 Schritte pro Umdrehung (0,088 Grad pro Schritt)
inline int32_t gradZuRoh(float grad) { return (int32_t)lroundf(grad * 4096.0f / 360.0f); }
inline float rohZuGrad(float roh) { return roh * 360.0f / 4096.0f; }

// Oberarmwinkel phi -> Motorwinkel (und zurueck), mit Kalibrierung aus Konfiguration.h
inline float phiZuMotorGrad(int i, float phi) { return NULLPOSITION[i] + RICHTUNG[i] * phi; }
inline float motorGradZuPhi(int i, float grad) { return (grad - NULLPOSITION[i]) * RICHTUNG[i]; }

// Sucht die Motoren und stellt sie auf Positionsregelung. Das Drehmoment bleibt AUS.
bool motorenStarten() {
  dxl.begin(DXL_BAUDRATE);
  dxl.setPortProtocolVersion(2.0);
  for (uint8_t i = 0; i < 3; i++) {
    if (!dxl.ping(MOTOR_ID[i])) {
      Serial.print("Motor ");
      Serial.print(i + 1);
      Serial.print(" (ID ");
      Serial.print(MOTOR_ID[i]);
      Serial.println(") antwortet nicht.");
      return false;
    }
  }
  for (uint8_t i = 0; i < 3; i++) {
    dxl.torqueOff(MOTOR_ID[i]);
    dxl.setOperatingMode(MOTOR_ID[i], OP_POSITION);
    dxl.writeControlTableItem(ControlTableItem::PROFILE_ACCELERATION, MOTOR_ID[i], 0);
    dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, MOTOR_ID[i], PROFIL_ZEICHNEN);
  }
  syncInfo.packet.p_buf = nullptr;
  syncInfo.packet.is_completed = false;
  syncInfo.addr = ADR_GOAL_POSITION;
  syncInfo.addr_length = LEN_GOAL_POSITION;
  syncInfo.p_xels = syncXel;
  syncInfo.xel_count = 0;
  for (uint8_t i = 0; i < 3; i++) {
    syncXel[i].id = MOTOR_ID[i];
    syncXel[i].p_data = (uint8_t*)&syncDaten[i].sollPosition;
    syncInfo.xel_count++;
  }
  syncInfo.is_info_changed = true;
  drehmomentIstAn = false;
  return true;
}

// Motorwinkel (Grad, 0..360) wie vom Motor gemeldet
inline float leseMotorGrad(uint8_t i) {
  return dxl.getPresentPosition(MOTOR_ID[i], UNIT_DEGREE);
}

// Schaltet das Drehmoment ein, OHNE dass die Motoren springen:
// vorher wird die Sollposition auf die aktuelle Position gesetzt.
void drehmomentEin() {
  if (trockenlauf || drehmomentIstAn) return;
  for (uint8_t i = 0; i < 3; i++) {
    const int32_t ist = (int32_t)dxl.getPresentPosition(MOTOR_ID[i]);
    syncDaten[i].sollPosition = ist;
    dxl.setGoalPosition(MOTOR_ID[i], ist);
    dxl.torqueOn(MOTOR_ID[i]);
  }
  drehmomentIstAn = true;
}

void drehmomentAus() {
  if (!motorenGefunden) return;
  for (uint8_t i = 0; i < 3; i++) dxl.torqueOff(MOTOR_ID[i]);
  drehmomentIstAn = false;
}

void setzeProfilGeschwindigkeit(uint32_t wert) {
  if (trockenlauf) return;
  for (uint8_t i = 0; i < 3; i++) {
    dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, MOTOR_ID[i], wert);
  }
}

// Schickt die drei Oberarmwinkel (Grad) gleichzeitig an die Motoren.
bool sendeWinkel(const float phi[3]) {
  for (uint8_t i = 0; i < 3; i++) {
    const int32_t roh = gradZuRoh(phiZuMotorGrad(i, phi[i]));
    if (roh < 0 || roh > 4095) {
      Serial.print("Motor ");
      Serial.print(i + 1);
      Serial.println(": Sollwert ausserhalb 0..360 Grad - NULLPOSITION/RICHTUNG pruefen!");
      return false;
    }
    syncDaten[i].sollPosition = roh;
  }
  if (trockenlauf) return true;
  syncInfo.is_info_changed = true;
  if (!dxl.syncWrite(&syncInfo)) {
    Serial.print("SyncWrite fehlgeschlagen, Fehlercode ");
    Serial.println(dxl.getLastLibErrCode());
    return false;
  }
  return true;
}

// Wartet, bis alle Motoren ihre Sollposition (fast) erreicht haben.
bool warteBisAngekommen(uint32_t timeoutMs) {
  if (trockenlauf) return true;
  const uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    bool alleDa = true;
    for (uint8_t i = 0; i < 3; i++) {
      const float ist = dxl.getPresentPosition(MOTOR_ID[i]);
      if (fabsf(ist - (float)syncDaten[i].sollPosition) > 20.0f) alleDa = false;  // 20 Schritte = 1,8 Grad
    }
    if (alleDa) return true;
    delay(20);
  }
  Serial.println("Warnung: Motoren haben das Ziel nicht ganz erreicht (Zeitlimit).");
  return false;
}
