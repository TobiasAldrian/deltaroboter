# Inbetriebnahme am Deltaroboter

Schritt für Schritt vom aufgebauten Roboter bis zur ersten Zeichnung.
Arbeite die Schritte **der Reihe nach** ab. Jeder baut auf dem vorherigen auf.

Alle Werte, die du ermittelst, kommen in **`arduino/DeltaPlotter/Konfiguration.h`**.
Nach jeder Änderung den Sketch **neu hochladen**.

---

## 0. Sicherheit und Vorbereitung

- **Stift ausbauen** für alle Tests bis Schritt 6.
- Das **12-V-Netzteil** für die Motoren so platzieren, dass du es jederzeit schnell ausstecken kannst.
  Das ist dein Not-Aus.
- Im Seriellen Monitor bricht **jede Eingabe + Enter** eine laufende Bewegung ab. Mit `a` schaltest
  du das Drehmoment aus. Dann fällt die Plattform herunter, also vorher festhalten.
- Beim ersten Bewegen immer eine Hand an der Plattform bzw. am Stecker.
- Spannung: XL430 = 6,5 bis 12 V (empfohlen 11,1 bis 12 V). Die Motoren bekommen ihren Strom über den
  Versorgungseingang des OpenRB-150, **USB allein reicht nicht**. Polung beachten.

Du brauchst: Laptop mit Arduino IDE (Board **OpenRB-150**, Bibliothek **Dynamixel2Arduino**),
USB-Datenkabel, 12-V-Netzteil, Wasserwaage oder Winkelmesser, Lineal/Maßband, A4-Blatt, Klebeband.

---

## 1. Motoren nummerieren (IDs)

Ab Werk haben **alle XL430 die ID 1**. Damit man sie unterscheiden kann, brauchen sie **1, 2, 3**.

**Welcher Motor ist welcher?** Von **oben** auf den Roboter geschaut:

```
               Motor 2 (120°)
                    \
                     \
                      ●  Basismitte ───────►  Motor 1 (0°)   = x-Achse
                     /
                    /
               Motor 3 (240°)
```

Motor 1 frei wählen, dann **gegen den Uhrzeigersinn** Motor 2 und Motor 3. Am besten mit Klebeband
an den Armen beschriften.

**IDs einstellen** mit dem Hilfs-Sketch `arduino/MotorID_Setzen`:

1. `MotorID_Setzen.ino` öffnen, Board OpenRB-150, hochladen, Seriellen Monitor öffnen (115200, *Newline*).
2. **Nur Motor 2** am OpenRB anschließen, 12 V an.
3. `s` zeigt die gefundene ID (normalerweise `1`). Dann `i 1 2` stellt ID 2 ein.
4. **Nur Motor 3** anschließen: `s`, dann `i 1 3`.
5. Motor 1 behält ID 1.
6. Alle drei anschließen: `s` muss **ID 1, 2, 3** zeigen. Mit `l 2` leuchtet die LED von Motor 2.
   So prüfst du, ob die Nummerierung zur Skizze oben passt.

Eintragen in `Konfiguration.h` (nur nötig, wenn andere IDs verwendet werden):
```cpp
const uint8_t MOTOR_ID[3] = {1, 2, 3};
```

---

## 2. Geometrie nachmessen

Die Werte in `Konfiguration.h` stammen aus dem Berechnungsbericht (CAD). Am echten Roboter nachmessen.
Es zählen immer die **Drehpunkte**, nicht die Außenmaße:

| Konstante | Was messen | Bericht |
|---|---|---|
| `OBERARM_A` | Motorachse → Mitte der Kugelgelenke am Ellbogen | 223,6 mm |
| `UNTERARM_B` | Kugelmitte → Kugelmitte eines Unterarms | 460 mm |
| `RADIUS_BASIS` | Basismitte → Motorachse | 51,1 mm |
| `RADIUS_PLATTFORM` | Plattformmitte → Mitte zwischen den zwei Kugeln eines Gelenkpaars | 55,8 mm |
| `STIFT_UEBERSTAND` | Stiftspitze → Ebene der unteren Kugelmitten (senkrecht) | 43 mm |

Weicht etwas um mehr als ca. 1 mm ab, den gemessenen Wert eintragen.

---

## 3. Erster Upload: Trockenlauf

1. `arduino/DeltaPlotter/DeltaPlotter.ino` öffnen, Board **OpenRB-150**, Port wählen, hochladen.
   Klappt der Upload nicht: Reset-Taste am OpenRB **zweimal schnell** drücken (Bootloader), Port neu wählen.
2. Seriellen Monitor öffnen: **115200 Baud**, Zeilenende **Newline**.
3. **Ohne 12 V** muss kommen: `Motoren nicht gefunden -> TROCKENLAUF`.
4. `p` meldet `alle erreichbar`. `z` gibt Positionen und Winkel aus, und am Ende kommt `Fertig.`

Damit ist klar: Programm, Board und Upload funktionieren.

---

## 4. Motoren kalibrieren (`k`)

Der Sketch muss wissen, welcher Motorwinkel **„Oberarm waagrecht“** bedeutet (`NULLPOSITION`)
und in welche Richtung der Motor beim Absenken zählt (`RICHTUNG`).

1. 12 V an, Reset drücken. Jetzt muss `Alle 3 Motoren gefunden` erscheinen.
2. `k` eingeben. Das Drehmoment geht aus, **Plattform festhalten bzw. abstützen**.
3. **Motor 1:** Oberarm genau **waagrecht** stellen (Wasserwaage auf der Linie Motorachse → Ellbogengelenk).
   Den angezeigten Wert `M1: ... Grad` notieren. Das ist `NULLPOSITION[0]`.
4. Oberarm 1 etwas **nach unten** drücken: Wird der Wert **größer**, ist `RICHTUNG[0] = 1`, wird er **kleiner**, ist es `-1`.
5. Genauso für Motor 2 und 3.
6. Beliebige Eingabe beendet die Anzeige.

Eintragen, z. B.:
```cpp
const float NULLPOSITION[3] = {183.4f, 177.9f, 181.2f};
const int8_t RICHTUNG[3]    = {-1, -1, -1};
```

**Kontrolle:** Neu hochladen und wieder `k`. Steht der Arm waagrecht, muss `phi` ungefähr 0 sein,
und nach unten gedrückt muss `phi` positiv werden.

> **Wichtig:** `NULLPOSITION` sollte zwischen etwa **90° und 270°** liegen. Der Motor kann im
> Positionsmodus nur 0 bis 360° anfahren, und der Arm braucht ca. −20° bis +70° Spielraum. Liegt der
> Wert nahe 0° oder 360°, das Horn bzw. den Oberarm um ein paar Zähne versetzt montieren.

---

## 5. Erste Bewegung ohne Stift

1. `s` fährt **langsam** zur Startposition (Blattmitte, 50 mm über dem Papier).
   Erwartet: Alle drei Oberarme stehen **knapp unter der Waagrechten** (ca. 10°), die Plattform hängt
   mittig und waagrecht. Steht ein Arm **über** der Waagrechten oder läuft er gegen einen Anschlag,
   sofort Netzteil ziehen: `RICHTUNG` bzw. `NULLPOSITION` dieses Motors prüfen.
2. **Richtungen prüfen.** Lege das Blatt so, dass die **rechte Blattkante zu Motor 1** zeigt.
   - `g 50 0 50` → die Plattform fährt 50 mm **Richtung Motor 1** (nach „rechts“ auf dem Blatt)
   - `g 0 50 50` → die Plattform fährt 50 mm nach **„oben“** auf dem Blatt, also auf die Seite von Motor 2
   - Fährt sie bei `g 0 50 50` stattdessen Richtung Motor 3, sind **Motor 2 und 3 vertauscht**:
     `MOTOR_ID = {1, 3, 2}` eintragen.
   - Soll das Blatt anders liegen: `PAPIER_DREHUNG` in Grad eintragen (gegen den Uhrzeigersinn).
3. `g 0 0 50` zurück zur Mitte.

---

## 6. Papierebene ermitteln

Jetzt wird festgelegt, wo das Papier für den Roboter liegt.

1. **Stift einbauen**, A4-Blatt mittig unter den Roboter kleben.
2. `g 0 0 10`: Der Stift steht laut Programm 10 mm über dem Papier (noch geschätzt).
3. Mehrmals `-` (jeweils 0,5 mm tiefer), bis die Stiftspitze das Papier **gerade berührt**.
   Ein zweites Blatt sollte sich darunter noch mit leichtem Widerstand verschieben lassen.
4. Die Ausgabe zeigt z. B. `Stiftspitze im Roboter-KS: z = -538.50 mm`.
   Diesen Wert als `PAPIEREBENE_Z` eintragen.
5. Hochladen und kontrollieren: `g 0 0 1` steht 1 mm über dem Papier, `g 0 0 0` berührt es.
6. **Ecken prüfen:** zu jeder Ecke `g 95 138 2` / `g -95 138 2` / `g -95 -138 2` / `g 95 -138 2`
   fahren und mit `-` absenken, bis der Stift berührt. Die angezeigte Höhe sollte überall **0 ± 0,5 mm** sein.
   - Eine Seite höher als die andere: Tisch oder Blatt schief, oder die `NULLPOSITION` eines Motors ungenau.
   - Mitte passt, alle Ecken gleich zu hoch bzw. zu tief (Schüssel-Form): Geometrie aus Schritt 2 nachmessen.
7. Danach in `Konfiguration.h` die Untergrenze scharf stellen: `const float Z_MIN = -1.0f;`

---

## 7. Testquadrat mit `g` zeichnen

Prüft Maßstab und Rechtwinkligkeit, noch ohne Converter:

```
g -50 -50 5
g -50 -50 0
g 50 -50 0
g 50 50 0
g -50 50 0
g -50 -50 0
g -50 -50 5
```

Nachmessen: alle Seiten **100 mm**, beide Diagonalen **141,4 mm**.
Abweichungen über 1 bis 2 mm zeigen, dass Geometrie oder Kalibrierung noch nicht stimmen.

---

## 8. Erste Zeichnung

1. **Testlauf in der Luft.** In `config/plotter.toml` vorübergehend eintragen:
   ```toml
   [stift]
   z_zeichnen = 20.0
   z_heben    = 30.0
   ```
2. Header erzeugen und direkt in den Sketch schreiben:
   ```bash
   python -m deltaconvert beispiele/triangle.png -m mittellinie -f header -o arduino/DeltaPlotter
   ```
3. Hochladen, `p` (prüfen), `z`: Der Roboter zeichnet 20 mm über dem Papier, nur zum Zuschauen.
4. Passt alles: in `plotter.toml` wieder `z_zeichnen = 0.0` und `z_heben = 5.0` bis `10.0` setzen,
   Header neu erzeugen, hochladen, **`z`** zeichnet echt.

Eigenes Bild: genauso mit `python -m deltaconvert meinbild.png -f header -o arduino/DeltaPlotter`, dann in
`DeltaPlotter.ino` die zwei Zeilen bei `ZEICHNUNG` anpassen.

---

## 9. Feinabstimmen

| Problem | Abhilfe |
|---|---|
| Linien zittern, Ecken schwingen nach | `[vorschub] zeichnen` senken |
| Stift setzt stellenweise aus | `z_zeichnen = -0.5` (nur, wenn die Halterung etwas nachgibt) |
| Stift kratzt / drückt zu stark | `z_zeichnen` erhöhen oder `PAPIEREBENE_Z` nachjustieren |
| Quadrat wird zum Trapez / verzerrt | Kalibrierung (Schritt 4) und Geometrie (Schritt 2) prüfen |
| Zeichnung zu groß/klein im Vergleich zur Vorschau | Geometrie prüfen, v. a. `OBERARM_A`, `UNTERARM_B` |
| Leerfahrten dauern lange | `z_heben` kleiner (3 bis 5 mm), `leerfahrt` erhöhen |

Läuft alles sauber, die Vorschübe schrittweise steigern, z. B. 600 → 1000 → 1500 mm/min.

---

## 10. Werte sichern

Die ermittelten Werte gehören zum Roboter, also **committen und pushen** (GitHub Desktop):

```
Kalibrierung: Motor-IDs, Nullpositionen und Papierebene eingetragen
```

---

## Fehlersuche

| Meldung / Verhalten | Ursache und Lösung |
|---|---|
| `Motoren nicht gefunden -> TROCKENLAUF` | 12 V an? Kabel steckt? Mit `MotorID_Setzen` → `s` prüfen, ob die IDs 1, 2, 3 da sind |
| `Motor 2 (ID 2) antwortet nicht` | Diesen Motor bzw. dieses Kabel prüfen. ID richtig? |
| `Sollwert ausserhalb 0..360 Grad` | `NULLPOSITION` oder `RICHTUNG` falsch, oder `NULLPOSITION` liegt zu nah an 0°/360° (Schritt 4) |
| `Nicht erreichbar: ...` | Punkt liegt außerhalb des Arbeitsraums. `PAPIEREBENE_Z`, Geometrie, `PHI_MIN`/`PHI_MAX`, `Z_MIN` prüfen |
| `Position unbekannt - zuerst 's'` | Nach dem Einschalten bzw. nach `k`/`a` zuerst `s` |
| Motor-LED blinkt, Motor reagiert nicht mehr | Überlast-Schutz des XL430 hat ausgelöst: 12 V kurz aus- und wieder einschalten, Ursache suchen (Anschlag, Klemmen, zu schnell) |
| Upload schlägt fehl / kein Port | USB-**Daten**kabel verwenden, Reset 2× schnell drücken. Linux: `sudo usermod -a -G dialout $USER`, neu anmelden |
