# Deltaroboter-Converter

Wandelt ein **Bild (PNG/JPG/…)** oder eine **SVG-Datei** in Zeichenpfade für unseren selbstgebauten
Deltaroboter mit Stifthalter (Stift-Plotter) um. Der Roboter fährt die Pfade – wie ein 3D-Drucker
seinen G-Code – von einer **Startposition** aus ab und kehrt danach zur **Endposition** zurück.

Hardware: 3 × Dynamixel XL430-W250-T, gesteuert von einem ROBOTIS **OpenRB-150**.

```
Bild / SVG ──► Converter (Python) ──┬──► zeichnung.gcode        G-Code, wie beim 3D-Druck
                                    ├──► zeichnung.h            zum Einbinden in den Arduino-Sketch
                                    └──► zeichnung_vorschau.svg zum Kontrollieren im Browser
```

![Beispiel-Vorschau](beispiele/ausgabe/haus_vorschau.svg)

*Schwarz = gezeichnet, rot gestrichelt = Leerfahrt mit angehobenem Stift, grün = Start, blau = Ende.*

---

## Installation

Benötigt wird **Python 3.11 oder neuer** ([python.org](https://www.python.org/downloads/), beim
Installieren „Add Python to PATH“ anhaken).

```bash
pip install -r requirements.txt
```

## Benutzung

Im Repository-Ordner eine Eingabeaufforderung öffnen (im Explorer in die Adresszeile `cmd` tippen) und:

```bash
python -m deltaconvert beispiele/haus.png
```

Die Dateien landen im Ordner `ausgabe/`. Weitere Beispiele:

```bash
python -m deltaconvert logo.png -m kontur              # Umrisse zeichnen
python -m deltaconvert skizze.jpg -m mittellinie       # Linien nur einmal (Mittellinie) zeichnen
python -m deltaconvert foto.jpg -m kanten              # Kantenerkennung für Fotos
python -m deltaconvert bild.png -s 100                 # eigener Schwellwert statt automatisch
python -m deltaconvert bild.png -i                     # helle statt dunkle Bereiche zeichnen
python -m deltaconvert stern.svg -n stern -o meine_ausgabe
python -m deltaconvert --help                          # alle Optionen
```

### Welcher Modus wofür?

| Modus         | Was wird gezeichnet?                                  | Geeignet für                         |
|---------------|-------------------------------------------------------|--------------------------------------|
| `kontur`      | Umriss jeder dunklen Fläche                            | Logos, Silhouetten, ausgefüllte Formen |
| `mittellinie` | die Mitte jeder dunklen Linie, nur **einmal**          | Strichzeichnungen, Skizzen, Schrift   |
| `kanten`      | erkannte Kanten (Canny)                               | Fotos                                |
| *SVG*         | jede Form genau so, wie sie in der Datei steht         | Vektorgrafiken (Inkscape, Illustrator) |

Tipp: Bei **SVG** werden Texte ignoriert – in Inkscape vorher *Pfad → Objekt in Pfad umwandeln*.

## Konfiguration – `config/plotter.toml`

Alle Maße stehen in [`config/plotter.toml`](config/plotter.toml). **Einträge mit `PLATZHALTER`
müssen noch mit den echten Maßen des Roboters ersetzt werden.**

| Abschnitt          | Inhalt                                                                  |
|--------------------|-------------------------------------------------------------------------|
| `[zeichenflaeche]` | Größe des Papiers und freier Rand; die Zeichnung wird automatisch eingepasst |
| `[stift]`          | Z-Höhe beim Zeichnen und bei Leerfahrten                                |
| `[vorschub]`       | Geschwindigkeiten in mm/min                                              |
| `[startposition]`, `[endposition]` | wo der Roboter vor bzw. nach dem Zeichnen steht         |
| `[bild]`           | Modus, Schwellwert, Glättung (nur für Rasterbilder)                      |
| `[pfade]`          | Vereinfachung, Mindestlänge, Optimierung der Reihenfolge                 |
| `[ausrichtung]`    | Drehen (0/90/180/270°) und Spiegeln, falls das Blatt anders liegt        |

Tippfehler (z. B. `breit` statt `breite`) werden mit einer Fehlermeldung angezeigt.

### Koordinatensystem

Der Converter arbeitet in **Papier-Koordinaten** (wie ein Delta-3D-Drucker):

- Ursprung `(0, 0)` = **Mitte der Zeichenfläche**, X nach rechts, Y nach oben (Blick aufs Blatt)
- `Z = 0` = Papieroberfläche, Z nach oben positiv
- alle Werte in **mm**

Die Umrechnung in **Roboter-Koordinaten** (Abstand zur Basis, inverse Kinematik, Motorwinkel)
passiert im Arduino-Programm. Dadurch muss der Converter die Abmessungen des Roboters nicht kennen.

## Ausgabeformate

### G-Code (`.gcode`)

```gcode
G21 ; Einheit mm
G90 ; absolute Koordinaten
G0 X0.00 Y0.00 Z30.00 F3000 ; Startposition
; Strich 1
G0 X-0.15 Y-19.79 Z10.00
G1 Z0.00 F600 ; Stift absetzen
G1 X16.62 Y-19.69 F1500
...
G1 Z10.00 F600 ; Stift heben
G0 X0.00 Y0.00 Z30.00 F3000 ; Endposition
```

Kann z. B. auf [ncviewer.com](https://ncviewer.com) angeschaut werden.

### Arduino-Header (`.h`)

Enthält alle Punkte als Array (`int16`, 1/100 mm, 6 Byte pro Punkt) plus Start-/Endposition,
Z-Höhen und Vorschübe. Auf dem OpenRB-150 liegen die Daten im Flash (256 KB), nicht im RAM.
Mehrere Zeichnungen können gleichzeitig eingebunden werden.

```cpp
#include "haus.h"
...
zeichne(zeichnung_haus);
```

## In ein Arduino-Programm einbinden

Der Beispiel-Sketch [`arduino/DeltaPlotter_Beispiel`](arduino/DeltaPlotter_Beispiel) zeigt den Ablauf:
**Startposition → alle Striche → Endposition**. Lange Geraden werden in 1-mm-Stücke zerlegt,
weil ein Deltaroboter zwischen zwei weit entfernten Punkten sonst einen Bogen fährt.

1. Arduino IDE: Board **OpenRB-150** auswählen (Board-Paket von ROBOTIS, Bibliothek *Dynamixel2Arduino*).
2. `python -m deltaconvert meinbild.png` ausführen.
3. `ausgabe/meinbild.h` in den Sketch-Ordner kopieren.
4. Im Sketch `#include "meinbild.h"` und `zeichne(zeichnung_meinbild);` eintragen.

> **Stand:** Die Funktion `fahreZu()` im Sketch steuert noch keine Motoren an, sondern gibt die
> Positionen im Seriellen Monitor (115200 Baud) aus („Trockenlauf“). Umrechnung auf Roboter-
> Koordinaten, inverse Kinematik und Dynamixel-Ansteuerung folgen in einem späteren Schritt.

## Ordnerstruktur

```
deltaroboter-converter/
├── deltaconvert/            Python-Paket (der Converter)
│   ├── cli.py               Kommandozeile
│   ├── konfig.py            Laden/Prüfen von plotter.toml
│   ├── rasterbild.py        Bild → Striche (kontur / mittellinie / kanten)
│   ├── svgdatei.py          SVG → Striche
│   ├── geometrie.py         Einpassen, Glätten, Vereinfachen, Reihenfolge
│   ├── werkzeugweg.py       Punktfolge mit Stift oben/unten, Statistik
│   ├── ausgabe_gcode.py     → .gcode
│   ├── ausgabe_arduino.py   → .h
│   └── ausgabe_vorschau.py  → _vorschau.svg
├── config/plotter.toml      Maße und Einstellungen
├── arduino/DeltaPlotter_Beispiel/   Beispiel-Sketch für den OpenRB-150
├── beispiele/               Beispielbilder und erzeugte Ausgaben
└── tests/                   automatische Tests
```

## Tests

```bash
pip install pytest
python -m pytest
```

Die Tests prüfen unter anderem, dass die erzeugte `.h`-Datei kompiliert und dass der Beispiel-Sketch
im Trockenlauf bis zur Endposition fährt (dafür wird `g++` benötigt, sonst werden diese Tests übersprungen).
Bei jedem Hochladen auf GitHub laufen die Tests automatisch (GitHub Actions).

## Nächste Schritte

- [ ] echte Maße in `config/plotter.toml` eintragen (Zeichenfläche, Z-Höhen, Start/Ende, Vorschübe)
- [ ] `fahreZu()`: Papier- → Roboter-Koordinaten, inverse Kinematik, Dynamixel-Ansteuerung
- [ ] Test auf dem echten Roboter
- [ ] Ideen: Schraffur für gefüllte Flächen, G-Code über USB streamen statt einbinden

---
HTBLA Kaindorf · KOP-Projekt Deltaroboter
