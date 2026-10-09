#pragma once
// =====================================================================
//  KONFIGURATION DeltaPlotter
// =====================================================================
//  Laengen in mm, Winkel in Grad.
//
//  Werte mit PLATZHALTER stammen aus dem Berechnungsbericht
//  (Stand 21.09.2026) und muessen am echten Roboter geprueft bzw.
//  kalibriert werden (siehe Befehle 'k' und '+'/'-' im Seriellen Monitor).
//
//  Roboter-Koordinatensystem (wie im Berechnungsbericht):
//    Ursprung  = Mitte der Basis, in der Ebene der drei Motorachsen
//    z-Achse   = nach oben (alle Punkte unter der Basis haben z < 0)
//    x-Achse   = zeigt (von oben gesehen) zu Motor 1
//    Oberarmwinkel phi = von der Waagrechten gemessen, nach unten positiv
// =====================================================================

#include <stdint.h>

// ---- Geometrie: Abstaende der Drehpunkte --------------------------------
const float OBERARM_A        = 223.6f;  // PLATZHALTER  Motorachse -> Kugelmitten Ellbogen
const float UNTERARM_B       = 460.0f;  // PLATZHALTER  Kugelmitte -> Kugelmitte (Unterarm)
const float RADIUS_BASIS     = 51.1f;   // PLATZHALTER  Basismitte -> Motorachse (r_B)
const float RADIUS_PLATTFORM = 55.8f;   // PLATZHALTER  Plattformmitte -> Mitte Gelenkpaar (r_P)

// Lage der Motoren 1, 2, 3 um die Basismitte (von oben, gegen den Uhrzeigersinn).
// Motor 1 liegt auf der x-Achse.
const float MOTOR_THETA[3] = {0.0f, 120.0f, 240.0f};

// ---- Papier: wo liegt das Blatt im Roboter-Koordinatensystem? -----------
const float PAPIEREBENE_Z    = -540.5f; // PLATZHALTER  Papieroberflaeche (z) -> mit 'g 0 0 5' und '-' ermitteln
const float STIFT_UEBERSTAND = 43.0f;   // PLATZHALTER  Stiftspitze unter der Gelenkebene der Plattform
const float PAPIER_MITTE_X   = 0.0f;    //              Blattmitte relativ zur Basismitte
const float PAPIER_MITTE_Y   = 0.0f;
const float PAPIER_DREHUNG   = 0.0f;    //              Drehung des Blatts gegen die Roboter-x-Achse (Grad, gegen Uhrzeigersinn)

// ---- Motoren (Dynamixel XL430-W250-T am OpenRB-150) ---------------------
const uint8_t  MOTOR_ID[3]  = {1, 2, 3};  // PLATZHALTER  IDs von Motor 1, 2, 3 (im Dynamixel Wizard eingestellt)
const uint32_t DXL_BAUDRATE = 57600;      //              Werkseinstellung XL430

// Motorwinkel (0..360 Grad, so wie ihn der Motor meldet), wenn der Oberarm WAAGRECHT steht.
const float NULLPOSITION[3] = {180.0f, 180.0f, 180.0f};  // PLATZHALTER -> mit 'k' kalibrieren
// +1, wenn der Motorwinkel GROESSER wird, wenn der Oberarm nach UNTEN geht, sonst -1.
const int8_t RICHTUNG[3]    = {1, 1, 1};                 // PLATZHALTER -> mit 'k' kalibrieren

// Erlaubter Bereich des Oberarmwinkels. Auf dem A4-Blatt werden ca. -3 .. 50 Grad gebraucht.
const float PHI_MIN = -20.0f;  // PLATZHALTER  (Oberarm nach oben, Anschlag an der Basis?)
const float PHI_MAX = 70.0f;   // PLATZHALTER

// Tiefster erlaubter Stift-Z in mm (negativ = unter der Papierebene). Schuetzt Stift und Tisch.
// Solange PAPIEREBENE_Z noch nicht ermittelt ist, etwas Spielraum lassen;
// danach z. B. auf -1.0 setzen.
const float Z_MIN = -20.0f;    // PLATZHALTER

// ---- Bewegung -----------------------------------------------------------
const uint16_t TAKT_MS          = 20;     // alle 20 ms ein neuer Sollwert (50 Hz)
const float    JOG_SCHRITT_MM   = 0.5f;   // Schrittweite fuer '+' / '-'
const float    VORSCHUB_HAND    = 600.0f; // mm/min fuer Befehle 'g', '+', '-'

// Dynamixel "Profile Velocity" (Einheit 0,229 U/min, 0 = unbegrenzt)
const uint32_t PROFIL_ANFAHREN  = 20;     // langsames erstes Anfahren (ca. 27 Grad/s)
const uint32_t PROFIL_ZEICHNEN  = 0;      // beim Zeichnen folgt der Motor direkt den 50-Hz-Sollwerten
