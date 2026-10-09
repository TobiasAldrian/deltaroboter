# Inbetriebnahme am Deltaroboter – Schritt für Schritt

Diese Anleitung führt dich vom fertig aufgebauten Roboter bis zur ersten Zeichnung.
Jeder Schritt hat dieselbe Form:

- **Ziel**: wozu der Schritt da ist
- **Du brauchst**: was bereitliegen muss
- **So geht's**: jede Eingabe und jeder Klick
- **Das solltest du sehen**: die Ausgabe, wie sie aussehen muss
- **Ergebnis eintragen**: was du mit dem Ergebnis machst
- **✅ Fertig, wenn**: woran du erkennst, dass du weitermachen kannst

Arbeite die Schritte **genau in dieser Reihenfolge** ab. Sieht etwas anders aus als hier beschrieben:
**aufhören**, die Ausgabe aus dem Seriellen Monitor kopieren und nachfragen.

| Schritt | Inhalt | Motoren | Stift |
|---|---|---|---|
| [1](#schritt-1--hardware-prüfen) | Hardware prüfen | aus | raus |
| [2](#schritt-2--software-test-ohne-motoren-trockenlauf) | Software-Test ohne Motoren (Trockenlauf) | aus | raus |
| [3](#schritt-3--motor-ids-einstellen) | Motor-IDs einstellen | an | raus |
| [4](#schritt-4--geometrie-messen-und-eintragen) | Geometrie messen und eintragen | egal | raus |
| [5](#schritt-5--motoren-kalibrieren) | Motoren kalibrieren | an | raus |
| [6](#schritt-6--erste-bewegung) | Erste Bewegung | an | raus |
| [7](#schritt-7--papierebene-einstellen) | Papierebene einstellen | an | **rein** |
| [8](#schritt-8--testquadrat-zeichnen) | Testquadrat zeichnen | an | rein |
| [9](#schritt-9--erste-zeichnung) | Erste Zeichnung | an | rein |
| [10](#schritt-10--feinabstimmen) | Feinabstimmen | an | rein |
| [11](#schritt-11--werte-auf-github-sichern) | Werte auf GitHub sichern | – | – |

Am Ende gibt es eine [Fehlersuche](#fehlersuche) und ein [Messprotokoll](#messprotokoll) zum Ausfüllen.

---

# Teil A – Grundlagen

Diese Handgriffe brauchst du in fast jedem Schritt. Später steht dort nur noch z. B. „hochladen (A4)“.

## A1 – Welches Programm wofür?

| Programm | Wofür |
|---|---|
| **Arduino IDE** | Sketch öffnen, Werte in `Konfiguration.h` ändern, hochladen, Befehle an den Roboter schicken (Serieller Monitor) |
| **VS Code** | Converter ausführen (Bild → `.h`), `config/plotter.toml` ändern |
| **GitHub Desktop** | Änderungen holen (Pull) und sichern (Commit + Push) |

## A2 – Sketch in der Arduino IDE öffnen

1. Arduino IDE starten.
2. Menü **File → Open…**
3. In deinen Repo-Ordner gehen → `arduino` → `DeltaPlotter` → Datei **`DeltaPlotter.ino`** → **Öffnen**.
4. Es öffnet sich ein Fenster mit diesen Reitern (Tabs) oben:
   `DeltaPlotter.ino` · `Bewegung.h` · `DeltaTypen.h` · `Kinematik.h` · `Konfiguration.h` · `Motoren.h` · `triangle.h`

## A3 – Board und Port auswählen

1. OpenRB-150 mit einem **USB-Datenkabel** an den Laptop anstecken. Am Board leuchtet die grüne LED (PWR).
2. Oben in der Werkzeugleiste das Auswahlfeld anklicken (dort steht z. B. „Arduino Nano“ oder „Select Board“).
3. **Select other board and port…** anklicken.
4. Links unter **BOARDS** `OpenRB` eintippen und **OpenRB-150** anklicken.
5. Rechts unter **PORTS** den Port anklicken, z. B. **COM5**.
   Welcher es ist: USB-Kabel kurz abstecken. Der Port, der dabei verschwindet, ist der richtige.
6. **OK**. Im Auswahlfeld steht jetzt „OpenRB-150“.

## A4 – Hochladen

> ⚠️ **Beim Hochladen startet das Board neu und schaltet dabei kurz den Strom der Motoren ab.
> Die Plattform fällt herunter.** Sobald Motoren angeschlossen sind und 12 V anliegen:
> vor jedem Hochladen die Plattform festhalten oder auf eine Unterlage legen.

1. Links oben den runden **Pfeil-Knopf (→, „Upload“)** klicken.
2. Unten im Bereich **Output** steht zuerst „Compiling sketch…“, dann laufen Upload-Meldungen durch.
3. Fertig, wenn unten rechts **„Done uploading.“** erscheint. Das erste Mal dauert es bis zu einer Minute.
4. Danach weiß der Roboter nicht mehr, wo er steht. Vor dem nächsten Bewegungsbefehl also zuerst `s` eingeben.

Wenn das Hochladen mit einer Fehlermeldung abbricht: Die **Reset-Taste am OpenRB zweimal schnell drücken**
(Bootloader-Modus), den Port neu auswählen (A3, Punkt 5, die Nummer kann sich ändern) und nochmal hochladen.

## A5 – Seriellen Monitor öffnen und Befehle schicken

Über den Seriellen Monitor „redest“ du mit dem Roboter.

1. Rechts oben das **Lupen-Symbol (Serial Monitor)** klicken (oder `Strg + Umschalt + M`).
2. Unten öffnet sich der Reiter **Serial Monitor**. Rechts darin zwei Auswahlfelder einstellen:
   - **New Line**
   - **115200 baud**
3. Befehl schicken: in die Eingabezeile klicken (dort steht grau „Message (Enter to send message to 'OpenRB-150' on 'COM5')“),
   den Befehl tippen, z. B. `?`, und **Enter** drücken.
4. Die Antwort erscheint im großen Feld darunter.

Tipp: Die Meldungen direkt nach dem Start verpasst man oft, weil der Monitor noch nicht offen war.
Dann einfach `?` eingeben, und der Roboter zeigt Befehle und Modus.

## A6 – Einen Wert in `Konfiguration.h` ändern

1. In der Arduino IDE den Reiter **`Konfiguration.h`** anklicken.
2. Die Zeile suchen (`Strg + F` und z. B. `OBERARM_A` eintippen).
3. **Nur die Zahl ändern.** Beispiel:

   ```cpp
   const float OBERARM_A        = 223.6f;  // PLATZHALTER  Motorachse -> Kugelmitten Ellbogen
   ```
   wird zu
   ```cpp
   const float OBERARM_A        = 224.8f;  // gemessen
   ```
   Dabei gilt:
   - **Punkt statt Komma**: `224.8` ✅, `224,8` ❌
   - Das **`f`** nach der Zahl bleibt stehen (`224.8f`).
   - Der **Strichpunkt `;`** am Ende bleibt stehen.
   - Bei Listen in `{ … }` bleiben die Kommas **zwischen** den Werten: `{183.4f, 177.9f, 181.2f}`
   - Alles nach `//` ist nur ein Kommentar und darf beliebig geändert werden.
4. Speichern: **`Strg + S`**.
5. **Hochladen (A4).** Erst dann kennt der Roboter den neuen Wert.

## A7 – Motoren einschalten

Der Roboter sucht die Motoren beim Start. Hast du die 12 V **erst danach** eingeschaltet,
oder waren sie zwischendurch aus, dann gib **`m`** ein:

```
Suche Motoren ...
Alle 3 Motoren gefunden (Drehmoment noch AUS).
```

Nach dem Start bzw. nach `m` sind die Motoren **kraftlos** (Drehmoment aus) und die Plattform hängt locker.
Erst ein Bewegungsbefehl (`s`, `g`, `z`) schaltet sie ein.

## A8 – Not-Halt

| Situation | Was tun |
|---|---|
| Eine Bewegung soll stoppen | beliebigen Buchstaben, z. B. `x`, + **Enter**. Der Roboter bleibt stehen bzw. hebt beim Zeichnen den Stift. |
| Sofort alles aus | **12-V-Stecker ziehen.** Die Motoren werden kraftlos, die Plattform fällt. |
| Motoren kraftlos schalten | `a` + Enter. Die Plattform fällt, also vorher festhalten. |

## A9 – Die Koordinaten

Alle Positionen gibst du in **Papier-Koordinaten** in mm an:

- **X** = nach rechts, **Y** = nach oben (so, wie du von oben auf das Blatt schaust)
- `0 0` = **Mitte des Blatts**
- **Z** = Höhe der Stiftspitze **über dem Papier** (`0` = Stift berührt das Papier)

Beispiel: `g 50 0 10` heißt 50 mm rechts von der Blattmitte, Stiftspitze 10 mm über dem Papier.

Die Oberarmwinkel heißen **phi**: `0` = Oberarm waagrecht, **positiv = nach unten**.

---

# Teil B – Inbetriebnahme

## Schritt 1 – Hardware prüfen

**Ziel:** Alles ist richtig verkabelt, bevor zum ersten Mal Strom fließt.

**Du brauchst:** Roboter, OpenRB-150, 12-V-Netzteil (mindestens 3 A), Dynamixel-Kabel.

**So geht's:**

1. **Stift ausbauen.** Er kommt erst in Schritt 7 wieder hinein.
2. Die **drei Motoren** mit den 3-poligen Dynamixel-Kabeln an die **DXL-Anschlüsse** des OpenRB-150 anschließen.
   Jeder Motor an einen eigenen Anschluss, oder hintereinander (Motor → Motor) – beides geht.
3. Das **12-V-Netzteil** an die **Schraubklemme** des OpenRB-150: **+ an +, − an −**.
   ⚠️ Falsche Polung zerstört das Board.
4. Den **Power-Jumper** auf dem OpenRB-150 auf die Seite **VIN (DXL)** stecken. Das verlangt ROBOTIS,
   wenn der Strom über die Schraubklemme kommt (Beschriftung auf der Platine beachten).
5. Netzteil noch **nicht** einstecken.

**✅ Fertig, wenn:** Der Stift ist draußen, alle drei Motoren hängen am OpenRB, das Netzteil ist richtig herum angeschlossen,
aber noch nicht eingesteckt.

---

## Schritt 2 – Software-Test ohne Motoren (Trockenlauf)

**Ziel:** Prüfen, ob Hochladen, Programm und Zeichnung funktionieren, **ohne dass sich etwas bewegt**.
Ohne 12 V findet das Programm keine Motoren und rechnet dann nur.

**Du brauchst:** Laptop, OpenRB-150, USB-Kabel. **Keine 12 V.**

**So geht's:**

1. **GitHub Desktop** öffnen → oben **Fetch origin**, dann **Pull origin**. Damit hast du die neueste Version.
2. Sketch öffnen (A2).
3. USB anstecken, Board und Port wählen (A3).
4. Hochladen (A4) und auf „Done uploading.“ warten.
5. Seriellen Monitor öffnen (A5), dabei **New Line** und **115200 baud** einstellen.
6. `?` + Enter.

   **Das solltest du sehen:**
   ```
   === DeltaPlotter - Befehle ===
     z          Zeichnung abfahren (Start -> Bild -> Ende)
     p          Zeichnung pruefen (alle Punkte erreichbar?)
     s          Startposition langsam anfahren
     g X Y Z    zu Papier-Koordinate fahren (mm), z.B.  g 0 0 10
     + / -      Stift um 0,5 mm heben / senken (Papierebene ermitteln)
     k          Kalibrieren: Drehmoment AUS, Motorwinkel anzeigen
     a          Drehmoment AUS (Roboter vorher festhalten!)
     m          Motoren neu suchen (z.B. nach dem Einschalten der 12 V)
     t          Trockenlauf ein/aus
     ?          diese Hilfe
   Modus: TROCKENLAUF (Motoren bewegen sich nicht)
   Zeichnung: triangle (39 Punkte)
   ```
   Wichtig ist die Zeile **`Modus: TROCKENLAUF`**.

7. `p` + Enter (prüft die Zeichnung).

   **Das solltest du sehen:**
   ```
   Zeichnung 'triangle': 39 Punkte, alle erreichbar. Oberarmwinkel 9.8 .. 40.4 Grad
   ```
   Das heißt: Alle 39 Punkte des Beispiel-Dreiecks sind erreichbar. Die Oberarme stehen dabei zwischen
   9,8° und 40,4° unter der Waagrechten.

8. `z` + Enter (fährt die Zeichnung ab, hier nur rechnerisch).

   **Das solltest du sehen** (läuft in ein paar Sekunden durch, etwa 4000 Zeilen):
   ```
   Zeichnung 'triangle': 39 Punkte, alle erreichbar. Oberarmwinkel 9.8 .. 40.4 Grad
   TROCKENLAUF (Motoren bewegen sich nicht):  x  y  z  phi1  phi2  phi3
   0.00    0.00    50.00   10.54   10.54   10.54
   -0.02   0.36    49.83   10.58   10.54   10.62
   -0.03   0.72    49.66   10.63   10.54   10.70
   ...
   0.00    0.36    49.84   10.58   10.54   10.62
   0.00    0.00    50.00   10.54   10.54   10.54
   Fertig.
   ```
   Jede Zeile ist ein Zwischenpunkt: `x y z` ist die Stiftposition in mm (A9), `phi1 phi2 phi3` sind die drei Oberarmwinkel.
   Die erste und die letzte Zeile sind Start- und Endposition: Blattmitte, 50 mm über dem Papier.

**Ergebnis eintragen:** nichts.

**✅ Fertig, wenn:** `?` zeigt `Modus: TROCKENLAUF`, `p` meldet `alle erreichbar` und `z` endet mit `Fertig.`

**Wenn nicht:**
- Gar keine Ausgabe: 115200 baud eingestellt? Richtiger Port? Reset-Taste **einmal** drücken, dann wieder `?`.
- Komische Zeichen: Die Baudrate ist falsch, also 115200 einstellen.
- Upload-Fehler: siehe A4 (zweimal Reset).

---

## Schritt 3 – Motor-IDs einstellen

**Ziel:** Jeder Motor bekommt eine eigene Nummer (ID 1, 2, 3). Ab Werk haben **alle drei die ID 1**,
dann würden sie gleichzeitig antworten. Die ID bleibt dauerhaft im Motor gespeichert, das musst du nur **einmal** machen.

**Du brauchst:** 12 V, USB, Klebeband und Stift zum Beschriften.

### 3.1 Motoren beschriften

Schau **von oben** auf die Basisplatte. Such dir einen Motor als **Motor 1** aus.
**Gegen den Uhrzeigersinn** folgen Motor 2 und Motor 3:

```
                (von OBEN gesehen)

     Motor 2 ●
                 ╲
                  ╲
                   ✚ Mitte ───────────────►  ● Motor 1
                  ╱
                 ╱
     Motor 3 ●
```

Klebe Zettel „M1“, „M2“, „M3“ auf die Motoren oder Oberarme.
Kannst du nur **von unten** schauen, ist es genau umgekehrt (im Uhrzeigersinn).

### 3.2 Hilfsprogramm hochladen

1. In der Arduino IDE **File → Open…** → Repo-Ordner → `arduino` → `MotorID_Setzen` → **`MotorID_Setzen.ino`**.
2. Board und Port wie in A3 (meist schon richtig).
3. Hochladen (A4).
4. Seriellen Monitor öffnen (A5): **New Line**, **115200 baud**.

### 3.3 Motor 2 auf ID 2 stellen

1. 12 V **aus**.
2. **Alle** Motorkabel vom OpenRB abstecken. Dann **nur Motor 2** anstecken.
   Hängen die Motoren hintereinander, die Kette so trennen, dass nur Motor 2 am Board hängt.
3. 12 V **ein**. Am OpenRB leuchtet die rote LED (DXL).
4. `s` + Enter (sucht etwa 3 Sekunden).

   **Das solltest du sehen:**
   ```
   Suche Motoren (ID 0..252) ...
     gefunden: ID 1  (Modellnummer 1060, XL430-W250 = 1060)
   1 Motor(en) gefunden.
   ```
5. `i 1 2` + Enter (ändert ID 1 auf ID 2).

   **Das solltest du sehen:**
   ```
   OK: ID 1 -> 2
   ```
6. Zur Kontrolle `s` + Enter. Jetzt muss `gefunden: ID 2` dastehen.

### 3.4 Motor 3 auf ID 3 stellen

Genauso: 12 V **aus** → **nur Motor 3** anstecken → 12 V **ein** → `s` → `i 1 3` → `s` zeigt `ID 3`.

### 3.5 Motor 1 prüfen

12 V **aus** → **nur Motor 1** anstecken → 12 V **ein** → `s` zeigt `ID 1`. Dann passt alles.
(Zeigt er eine andere ID, z. B. 7: `i 7 1`.)

### 3.6 Alle drei gemeinsam prüfen

1. 12 V **aus** → **alle drei** Motoren anstecken → 12 V **ein**.
2. `s` + Enter:
   ```
   Suche Motoren (ID 0..252) ...
     gefunden: ID 1  (Modellnummer 1060, XL430-W250 = 1060)
     gefunden: ID 2  (Modellnummer 1060, XL430-W250 = 1060)
     gefunden: ID 3  (Modellnummer 1060, XL430-W250 = 1060)
   3 Motor(en) gefunden.
   ```
3. `l 1` + Enter → die LED an dem Motor mit dem Zettel **M1** leuchtet 3 Sekunden.
   Dann `l 2` und `l 3`. Jede LED muss beim richtigen Zettel leuchten.

**Ergebnis eintragen:** nichts. `Konfiguration.h` verwendet schon die IDs 1, 2, 3.

**✅ Fertig, wenn:** `s` findet ID 1, 2 und 3, und `l 1` / `l 2` / `l 3` leuchten bei M1 / M2 / M3.

**Wenn nicht:**
- `Kein Motor gefunden`: 12 V an? Rote DXL-LED am Board an? Kabel richtig eingerastet? Jumper (Schritt 1)?
- `ID 2 ist schon vergeben`: Es hängt mehr als ein Motor am Board.

> Auf dem OpenRB läuft immer nur **ein** Programm. Ab Schritt 5 lädst du wieder `DeltaPlotter` hoch.

---

## Schritt 4 – Geometrie messen und eintragen

**Ziel:** In `Konfiguration.h` stehen die echten Maße deines Roboters statt der Werte aus dem CAD.
Sind sie falsch, werden Zeichnungen verzerrt.

**Du brauchst:** Messschieber und/oder Maßband, das [Messprotokoll](#messprotokoll).

### 4.1 Was muss wirklich gemessen werden?

| Konstante | Muss gemessen werden? | Warum |
|---|---|---|
| `OBERARM_A` | **ja** | bestimmt Größe und Form der Zeichnung |
| `UNTERARM_B` | **ja** | bestimmt Größe und Form der Zeichnung |
| `RADIUS_BASIS`, `RADIUS_PLATTFORM` | ja, aber nur ihr **Unterschied** zählt | im Programm wird nur `RADIUS_PLATTFORM − RADIUS_BASIS` verwendet (Bericht: 55,8 − 51,1 = 4,7 mm) |
| `STIFT_UEBERSTAND` | **nein** | wird in Schritt 7 automatisch mit ausgeglichen |

**Regel:** Weicht dein Messwert **weniger als 1 mm** vom Bericht ab, lass den Wert stehen (Messungenauigkeit).
Ab **1 mm Abweichung** trägst du deinen Messwert ein.

### 4.2 So misst du „Mitte bis Mitte“

Gemessen wird immer **von Drehpunkt zu Drehpunkt**, also zwischen den Mitten der Kugelgelenke bzw. der Motorwelle.
Weil man eine Mitte schlecht anpeilen kann:

```
      ◯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━◯
    |<──────────── außen ─────────────>|
         |<──────── innen ────────>|

    Mitte bis Mitte = (außen + innen) / 2
```

Beispiel: außen 478 mm, innen 442 mm → (478 + 442) / 2 = **460 mm**.

### 4.3 Oberarm `OBERARM_A` (alle 3 Arme messen)

Schau **seitlich** auf den Oberarm, genau in Richtung der Motorachse:

```
     Motorwelle                                    Kugelgelenke am Ellbogen
        ◉━━━━━━━━━━━━━━━━━ Oberarm ━━━━━━━━━━━━━━━━━━━━●
        |<────────────────── a ──────────────────────>|
       Mitte der Welle                           Mitte der Kugel
```

Miss von der **Mitte der Motorwelle** bis zur **Mitte der Kugel** am Ellbogen. Die beiden Kugeln eines Arms sitzen
nebeneinander, beide haben denselben Abstand. Trag alle 3 Werte ins Messprotokoll ein und bilde den **Mittelwert**.

Bericht: **223,6 mm**.

### 4.4 Unterarm `UNTERARM_B` (alle 6 Stangen messen)

```
     ● Kugelmitte oben (Ellbogen)
     ┃
     ┃   b = Kugelmitte bis Kugelmitte
     ┃
     ● Kugelmitte unten (Plattform)
```

Miss jede der **6 Stangen** von Kugelmitte zu Kugelmitte (Trick aus 4.2) und bilde den **Mittelwert**.

Bericht: **460 mm**.

> **Wichtig:** Alle 6 Stangen müssen **gleich lang** sein (Unterschied höchstens 0,5 mm). Sonst steht die Plattform schief.
> Ist eine Stange länger oder kürzer: den Gelenkkopf an ihrem Gewinde etwas hinein- oder herausdrehen und kontern.

### 4.5 Radien `RADIUS_BASIS` und `RADIUS_PLATTFORM`

```
   BASIS (von oben)                        PLATTFORM (von oben)

        ══════════════  ← Motorachse              ●   ●   ← Kugelpaar eines Arms
              ┃                                     ┃
              ┃  r_B                                ┃  r_P
              ┃                                     ┃
              ✚ Mitte der Basis                     ✚ Mitte der Plattform
```

- **r_B**: von der **Mitte der Basis** senkrecht bis zur **Motorachse** (Linie durch die Mitte der Motorwelle).
  Bericht: **51,1 mm**.
- **r_P**: von der **Mitte der Plattform** bis zur **Mitte zwischen den zwei Kugeln** eines Kugelpaars.
  Bericht: **55,8 mm**.

Für jeden der 3 Arme messen und mitteln. Entscheidend ist der **Unterschied r_P − r_B** (Bericht 4,7 mm).
Weicht nur er um weniger als 1 mm ab, beide Werte stehen lassen.

### 4.6 Ergebnis eintragen

1. `DeltaPlotter.ino` öffnen (A2), Reiter **`Konfiguration.h`**.
2. Diese Zeilen findest du ganz oben:
   ```cpp
   const float OBERARM_A        = 223.6f;  // PLATZHALTER  Motorachse -> Kugelmitten Ellbogen
   const float UNTERARM_B       = 460.0f;  // PLATZHALTER  Kugelmitte -> Kugelmitte (Unterarm)
   const float RADIUS_BASIS     = 51.1f;   // PLATZHALTER  Basismitte -> Motorachse (r_B)
   const float RADIUS_PLATTFORM = 55.8f;   // PLATZHALTER  Plattformmitte -> Mitte Gelenkpaar (r_P)
   ```
3. Die Werte ändern, deren Abweichung 1 mm oder mehr ist (A6), z. B. mit deinen Mittelwerten:
   ```cpp
   const float OBERARM_A        = 224.8f;  // gemessen 09.10.
   const float UNTERARM_B       = 461.5f;  // gemessen 09.10.
   ```
   Die Werte mit weniger als 1 mm Abweichung lässt du stehen. Ersetze aber `PLATZHALTER` durch `geprüft`,
   damit du siehst, dass du sie kontrolliert hast.
4. Speichern (`Strg + S`), hochladen (A4).
5. Seriellen Monitor (A5): `p` + Enter → muss weiterhin `alle erreichbar` melden. Die Winkel können sich leicht ändern.

**✅ Fertig, wenn:** Das Messprotokoll ist ausgefüllt, alle Werte mit 1 mm Abweichung oder mehr sind eingetragen, und `p` meldet `alle erreichbar`.

---

## Schritt 5 – Motoren kalibrieren

**Ziel:** Das Programm muss wissen,
1. welchen Winkel jeder Motor meldet, wenn sein Oberarm **genau waagrecht** steht (`NULLPOSITION`), und
2. ob der Motorwinkel **größer oder kleiner** wird, wenn der Oberarm nach unten geht (`RICHTUNG`).

**Du brauchst:** 12 V, USB, **eine zweite Person** zum Halten der Plattform, Maßband oder Wasserwaage, Messprotokoll.

**So geht's:**

1. `DeltaPlotter.ino` öffnen (A2) und **hochladen** (A4), weil zuletzt `MotorID_Setzen` drauf war.
2. Alle 3 Motoren angesteckt, 12 V **ein**.
3. Seriellen Monitor (A5): `?` + Enter → es muss **`Modus: MOTOREN AKTIV`** dastehen.
   Steht `TROCKENLAUF` da: `m` + Enter (A7).
4. `k` + Enter.

   **Das solltest du sehen** (alle 0,3 Sekunden eine neue Zeile, deine Zahlen sind anders):
   ```
   ACHTUNG: Drehmoment wird ausgeschaltet - Plattform festhalten!
   Oberarm waagrecht stellen -> angezeigter Motorwinkel = NULLPOSITION.
   Oberarm nach unten druecken: Wert wird groesser -> RICHTUNG +1, kleiner -> -1.
   Beliebige Eingabe beendet die Anzeige.
   M1: 131.8 Grad (phi -48.2)   M2: 135.2 Grad (phi -44.8)   M3: 129.9 Grad (phi -50.1)
   M1: 131.8 Grad (phi -48.2)   M2: 135.2 Grad (phi -44.8)   M3: 129.9 Grad (phi -50.1)
   ```
   Die Motoren sind jetzt kraftlos, du kannst die Arme von Hand bewegen.

5. **NULLPOSITION von Motor 1 ablesen:**
   - Die zweite Person hebt die Plattform langsam an, bis der **Oberarm von M1 genau waagrecht** steht.
   - **Waagrecht prüfen:** Die Höhe der **Motorwellen-Mitte** über dem Tisch muss gleich sein wie die Höhe der
     **Kugelmitte am Ellbogen** (beide mit dem Maßband messen, ±1 mm). Alternativ eine Wasserwaage auf den Oberarm legen,
     wenn das Rohr genau von der Welle zum Ellbogen läuft.
   - Den Wert hinter **`M1:`** ablesen und notieren, z. B. `183.4`.
6. **RICHTUNG von Motor 1 bestimmen:** Die Plattform langsam etwas **absenken** (Oberarm geht nach unten) und dabei `M1` beobachten:
   - Zahl wird **größer** → `RICHTUNG = 1`
   - Zahl wird **kleiner** → `RICHTUNG = -1`
7. Punkt 5 und 6 für **M2** und **M3** wiederholen. Meist haben alle drei dieselbe Richtung.
8. `x` + Enter beendet die Anzeige:
   ```
   Kalibrierung beendet. Werte in Konfiguration.h eintragen und neu hochladen.
   ```
   Die Plattform kann jetzt abgelegt werden.
9. **Bereich prüfen:** Der Motor kann nur zwischen 0° und 360° fahren, und der Arm braucht Platz nach oben und unten.
   - Bei `RICHTUNG = 1` muss NULLPOSITION zwischen **20 und 290** liegen.
   - Bei `RICHTUNG = -1` muss NULLPOSITION zwischen **70 und 340** liegen.

   Liegt ein Wert außerhalb: **nicht weitermachen** und nachfragen. Das lässt sich über einen Versatz im Motor
   oder durch Versetzen des Oberarms am Motorhorn lösen.

**Ergebnis eintragen:**

1. Reiter **`Konfiguration.h`**, diese Zeilen suchen:
   ```cpp
   const float NULLPOSITION[3] = {180.0f, 180.0f, 180.0f};  // PLATZHALTER -> mit 'k' kalibrieren
   ```
   ```cpp
   const int8_t RICHTUNG[3]    = {1, 1, 1};                 // PLATZHALTER -> mit 'k' kalibrieren
   ```
2. Deine Werte eintragen, **Reihenfolge M1, M2, M3**. Bei `NULLPOSITION` mit `f`, bei `RICHTUNG` nur `1` oder `-1` ohne `f`:
   ```cpp
   const float NULLPOSITION[3] = {183.4f, 177.9f, 181.2f};  // kalibriert 09.10.
   ```
   ```cpp
   const int8_t RICHTUNG[3]    = {-1, -1, -1};              // kalibriert 09.10.
   ```
3. Speichern, Plattform festhalten, hochladen (A4).
4. **Kontrolle:** `k` + Enter. Oberarm waagrecht halten → bei diesem Motor muss **`phi` zwischen −1 und +1** stehen.
   Absenken → `phi` wird **positiv** (hängt der Arm ganz unten, etwa 60 bis 90). Mit `x` beenden.

**✅ Fertig, wenn:** Bei waagrechtem Oberarm zeigt jeder Motor `phi ≈ 0`, und nach unten wird `phi` positiv.

---

## Schritt 6 – Erste Bewegung

**Ziel:** Der Roboter hält sich selbst und fährt in die richtigen Richtungen.

**Du brauchst:** 12 V, USB, Lineal, ein Blatt Papier mit Richtungspfeilen. **Noch ohne Stift.**

**So geht's:**

1. **Richtungen auf den Tisch zeichnen.** Leg ein Blatt unter die Mitte des Roboters und zeichne zwei Pfeile darauf:

   ```
                         ▲ Y+  („oben“)
       Motor 2 ●         │
                         │
                         ✚───────────────►  X+  („rechts“)          ● Motor 1
                       Mitte
       Motor 3 ●
   ```
   - **X+** zeigt zu **Motor 1**.
   - **Y+** steht rechtwinklig dazu, auf der Seite von **Motor 2**.
2. Seriellen Monitor (A5): `?` → `Modus: MOTOREN AKTIV` (sonst `m`).
3. **Eine Hand am 12-V-Stecker.** `s` + Enter.

   Der Roboter fährt **langsam** (2 bis 4 Sekunden) zur Startposition.

   **Das solltest du sehen:**
   ```
   Startposition erreicht.
   ```
   Und am Roboter: Alle drei Oberarme stehen **knapp unter der Waagrechten** (etwa 10°), die Plattform hängt **mittig und waagrecht**.

   ⚠️ Geht ein Arm **über** die Waagrechte nach oben, schlägt irgendwo an oder knackt es: **sofort 12-V-Stecker ziehen**.
   Dann stimmt `RICHTUNG` oder `NULLPOSITION` dieses Motors nicht → Schritt 5 wiederholen.

4. `g 50 0 50` + Enter. Die Plattform fährt **5 cm Richtung Motor 1 (X+)**. Mit dem Lineal grob nachmessen.
   ```
   Stift Z = 50.00 mm ueber Papier  |  Stiftspitze im Roboter-KS: z = -490.50 mm  (...)
   ```
5. `g 0 0 50` + Enter → zurück zur Mitte.
6. `g 0 50 50` + Enter. Die Plattform fährt **5 cm Richtung Y+** (Seite von Motor 2).
   - Fährt sie stattdessen auf die Seite von **Motor 3**, sind Motor 2 und 3 vertauscht.
     Dann in `Konfiguration.h`
     ```cpp
     const uint8_t  MOTOR_ID[3]  = {1, 2, 3};
     ```
     ändern auf
     ```cpp
     const uint8_t  MOTOR_ID[3]  = {1, 3, 2};
     ```
     Danach speichern, Plattform festhalten, hochladen, `s` und Punkt 4 bis 6 wiederholen.
7. `g 0 0 50` + Enter, dann `g 0 0 30` + Enter. Die Plattform fährt **2 cm nach unten**.
   `g 0 0 50` + Enter fährt sie wieder hinauf.
8. **Ausschalten:** Plattform festhalten → `a` + Enter (Motoren kraftlos) → ablegen → 12 V aus.

**✅ Fertig, wenn:** `s` bringt die Plattform mittig und waagrecht in Position, `g 50 0 50` fährt Richtung Motor 1,
`g 0 50 50` Richtung Motor 2 und `g 0 0 30` nach unten.

---

## Schritt 7 – Papierebene einstellen

**Ziel:** Der Roboter weiß genau, wo die Papieroberfläche liegt (dort ist Z = 0).

**Du brauchst:** 12 V, USB, **Stift**, A4-Blatt, Klebeband, einen Papierstreifen.

**So geht's:**

1. **Stift** in die Halterung stecken und festklemmen.
2. Seriellen Monitor: `?` → `MOTOREN AKTIV` (sonst `m`). `s` + Enter, dann `g 0 0 50` + Enter.
   Der Stift steht jetzt über der Mitte.
3. **Blatt hinlegen:** A4 zweimal falten, sodass die Knicke ein Kreuz in der Blattmitte ergeben.
   Das Kreuz genau **unter die Stiftspitze** legen, Hochformat, mit der **rechten Blattkante Richtung Motor 1**:
   ```
               Y+ („oben“)
        ┌───────────────┐
        │               │
        │               │
        │       ✚       │   ──►  X+   (rechte Kante zeigt zu Motor 1)
        │     Mitte     │
        │               │
        │               │
        └───────────────┘
          210 × 297 mm
   ```
   Die Ecken mit Klebeband festkleben.
4. **Stift in Etappen absenken.** Nach jedem Befehl auf den Abstand zwischen Stiftspitze und Papier schauen:
   - Abstand **größer als 15 mm** → nächster Befehl 10 weniger: `g 0 0 40`, `g 0 0 30`, …
   - Abstand **3 bis 15 mm** → nächster Befehl 2 weniger, z. B. `g 0 0 12`, `g 0 0 10`, …
   - Abstand **unter 3 mm** → nur noch `-` + Enter (jeweils 0,5 mm tiefer)

   ⚠️ Wenn der Stift das Papier schon berührt: **nicht weiter absenken.**
5. Mit `-` so lange absenken, bis die Stiftspitze das Papier **gerade berührt**. Probe: Einen Papierstreifen unter die Spitze
   schieben. Er lässt sich noch herausziehen, aber mit **leichtem Widerstand**.
6. **Wert ablesen.** Jede Bewegung schreibt eine Zeile wie diese:
   ```
   Stift Z = 3.50 mm ueber Papier  |  Stiftspitze im Roboter-KS: z = -537.00 mm  (beruehrt der Stift das Papier -> diesen Wert als PAPIEREBENE_Z eintragen)
   ```
   Notiere die **letzte** Zahl nach `Roboter-KS: z =`, hier **`-537.00`**.
   Die Zahl bei `Stift Z` ist noch falsch, weil der Roboter das Papier ja noch nicht kennt.
7. `g 0 0 20` + Enter (Stift wieder hoch).

**Ergebnis eintragen:**

1. In `Konfiguration.h` diese Zeile ändern:
   ```cpp
   const float PAPIEREBENE_Z    = -540.5f; // PLATZHALTER  Papieroberflaeche (z) -> mit 'g 0 0 5' und '-' ermitteln
   ```
   auf deinen Wert (minus bleibt!):
   ```cpp
   const float PAPIEREBENE_Z    = -537.0f; // ermittelt 09.10.
   ```
2. Speichern, Plattform festhalten, hochladen.
3. **Kontrolle in der Mitte:** `s` → `g 0 0 5` → `g 0 0 1` (Stift 1 mm über dem Papier) → `g 0 0 0` (Stift berührt gerade) → `g 0 0 10`.
4. **Kontrolle in den Ecken:** Für jede Ecke nacheinander:
   ```
   g 95 138 5
   ```
   dann `-` + Enter, bis der Stift berührt. Notiere den Wert bei **`Stift Z = …`** und fahre mit `g 95 138 10` wieder hoch.
   Danach die anderen Ecken:

   | Ecke | Befehl |
   |---|---|
   | rechts oben | `g 95 138 5` |
   | links oben | `g -95 138 5` |
   | links unten | `g -95 -138 5` |
   | rechts unten | `g 95 -138 5` |

   **Auswertung:**
   - Überall berührt der Stift bei **Z zwischen −0,5 und +0,5** → ✅ perfekt.
   - Eine Seite berührt früher als die andere → Tisch oder Blatt liegt schief (mit der Wasserwaage prüfen und unterlegen),
     oder `NULLPOSITION` eines Motors ist ungenau (Schritt 5 genauer wiederholen).
   - Die Mitte passt, aber **alle Ecken** sind gleich zu hoch oder zu tief → Geometrie (Schritt 4) nochmal nachmessen.
     Die 5 Werte aufschreiben und nachfragen.
5. **Schutz scharf schalten:** In `Konfiguration.h`
   ```cpp
   const float Z_MIN = -20.0f;    // PLATZHALTER
   ```
   ändern auf
   ```cpp
   const float Z_MIN = -1.0f;     // Papierebene ist ermittelt
   ```
   Speichern, hochladen. Jetzt fährt der Roboter nie mehr als 1 mm „ins Papier“.

**✅ Fertig, wenn:** `g 0 0 0` berührt das Papier gerade, die vier Ecken liegen innerhalb ±0,5 mm, und `Z_MIN` steht auf `-1.0f`.

> Wechselst du später auf einen **anderen Stift** (andere Länge), wiederhole Schritt 7.

---

## Schritt 8 – Testquadrat zeichnen

**Ziel:** Mit einer Figur, deren Maße man kennt, prüfen, ob Größe und Winkel stimmen.

**Du brauchst:** Stift, Blatt (wie in Schritt 7), Lineal.

**So geht's:**

1. `s` + Enter.
2. Diese Befehle **einzeln** eingeben, jeweils + Enter. Warte nach jedem, bis die Zeile `Stift Z = …` erscheint:
   ```
   g -50 -50 5
   g -50 -50 0
   g 50 -50 0
   g 50 50 0
   g -50 50 0
   g -50 -50 0
   g -50 -50 5
   ```
   Der Stift setzt links unten auf, zeichnet ein Quadrat mit **100 × 100 mm** und hebt wieder ab.
3. **Nachmessen** und ins Messprotokoll eintragen:
   - alle **4 Seiten**: Soll **100 mm**
   - beide **Diagonalen**: Soll **141,4 mm**

**Auswertung:**

| Ergebnis | Bedeutung |
|---|---|
| alles auf ±1 mm genau | ✅ weiter mit Schritt 9 |
| alle Seiten **gleich** zu lang bzw. zu kurz | `OBERARM_A` / `UNTERARM_B` stimmen nicht → Schritt 4 nachmessen |
| Seiten passen, aber die **Diagonalen sind verschieden** | Quadrat ist schief → Schritt 5 genauer wiederholen |
| Linien sind **gebogen** statt gerade | Geometrie stimmt nicht → Messprotokoll schicken und nachfragen |

**✅ Fertig, wenn:** Seiten und Diagonalen stimmen auf ±1 mm.

---

## Schritt 9 – Erste Zeichnung

**Ziel:** Das Beispiel-Dreieck vom Converter zeichnen, zuerst in der Luft, dann auf Papier.

**Du brauchst:** Laptop mit VS Code (Repo geöffnet, `.venv` aktiv), Roboter mit Stift, Blatt.

### 9.1 Testlauf in der Luft

1. **VS Code:** links im Explorer `config/plotter.toml` öffnen. Im Abschnitt `[stift]` vorübergehend ändern:
   ```toml
   [stift]
   z_zeichnen = 20.0
   z_heben    = 30.0
   ```
   Speichern (`Strg + S`).
2. **VS Code:** Terminal öffnen (**Terminal → New Terminal**). Die Zeile muss mit `(.venv)` beginnen. Dann eingeben:
   ```powershell
   python -m deltaconvert beispiele\triangle.png -m mittellinie -f header -o arduino\DeltaPlotter
   ```
   **Das solltest du sehen** (am Ende):
   ```
   Erzeugt:
     arduino\DeltaPlotter\triangle.h
   ```
3. **Arduino IDE:** Plattform festhalten, hochladen (A4). Falls der Reiter `triangle.h` noch die alte Version zeigt:
   das Sketch-Fenster schließen und neu öffnen (A2).
4. Seriellen Monitor: `?` (sonst `m`), dann `s`, `p` (`alle erreichbar`), `z`.

   **Das solltest du sehen:**
   ```
   Zeichnung 'triangle': 39 Punkte, alle erreichbar. Oberarmwinkel ...
   Zeichnen startet - beliebige Eingabe = ABBRUCH
   10 %
   20 %
   ...
   100 %
   Fertig.
   ```
   Der Roboter „zeichnet“ das Dreieck **20 mm über dem Papier**. Es ist etwa 20 cm breit, und die **Spitze zeigt nach Y+**
   (Blatt-oben). Die Zeichnung dauert etwa 1½ Minuten.

### 9.2 Echt zeichnen

1. **VS Code:** `config/plotter.toml` zurückstellen und speichern:
   ```toml
   [stift]
   z_zeichnen = 0.0
   z_heben    = 5.0
   ```
2. Converter-Befehl von oben **nochmal** ausführen.
3. Arduino IDE: hochladen, dann im Seriellen Monitor `s`, `z`.
4. **Abbrechen** geht jederzeit mit `x` + Enter:
   ```
   ABGEBROCHEN - Stift wird angehoben.
   ```

### 9.3 Eigenes Bild zeichnen

1. Bild speichern, z. B. als `katze.png` im Ordner **Downloads**. Am besten eignen sich einfache schwarz-weiße Strichbilder.
2. **Vorschau ansehen** (VS Code-Terminal):
   ```powershell
   python -m deltaconvert "$HOME\Downloads\katze.png" -m mittellinie
   ```
   Dann `ausgabe\katze_vorschau.svg` im Browser öffnen. Passt es nicht, `-m kontur` oder `-m kanten` probieren.
3. **Für den Roboter erzeugen:**
   ```powershell
   python -m deltaconvert "$HOME\Downloads\katze.png" -m mittellinie -f header -o arduino\DeltaPlotter
   ```
   → erzeugt `arduino\DeltaPlotter\katze.h`.
4. **Arduino IDE**, Reiter `DeltaPlotter.ino`: diese zwei Zeilen
   ```cpp
   #include "triangle.h"
   const DeltaZeichnung& ZEICHNUNG = zeichnung_triangle;
   ```
   ändern auf
   ```cpp
   #include "katze.h"
   const DeltaZeichnung& ZEICHNUNG = zeichnung_katze;
   ```
   Der Name hinter `zeichnung_` ist der Dateiname aus Punkt 3 ohne `.h`.
   Großbuchstaben werden klein, Leerzeichen und Sonderzeichen werden zu `_`, Umlaute zu `ae`/`oe`/`ue`.
   Aus „Mein Bild.png“ wird also `mein_bild.h` bzw. `zeichnung_mein_bild`.
5. Speichern, hochladen, `s`, `p`, `z`.

**✅ Fertig, wenn:** Das Dreieck ist sauber auf dem Papier.

---

## Schritt 10 – Feinabstimmen

Was du wo änderst:

| Was | Datei | Eintrag | Danach |
|---|---|---|---|
| Geschwindigkeit beim Zeichnen | `config/plotter.toml` | `[vorschub] zeichnen` (mm/min) | Converter + hochladen |
| Geschwindigkeit der Leerfahrten | `config/plotter.toml` | `[vorschub] leerfahrt` | Converter + hochladen |
| Hubhöhe bei Leerfahrten | `config/plotter.toml` | `[stift] z_heben` | Converter + hochladen |
| Stift-Andruck | `config/plotter.toml` | `[stift] z_zeichnen` (z. B. `-0.5`) | Converter + hochladen |
| Geschwindigkeit bei `g`, `+`, `-` | `Konfiguration.h` | `VORSCHUB_HAND` | hochladen |

„Converter“ bedeutet: den Befehl aus Schritt 9 nochmal ausführen.

| Problem | Abhilfe |
|---|---|
| Linien zittern, Ecken schwingen nach | `zeichnen` kleiner |
| Stift setzt stellenweise aus | `z_zeichnen = -0.5` (geht nur, wenn die Halterung etwas nachgibt) |
| Stift kratzt / drückt zu stark | `z_zeichnen` etwas größer (z. B. `0.3`) oder Schritt 7 wiederholen |
| Leerfahrten dauern lange | `z_heben` auf 3 bis 5, `leerfahrt` größer |

Wenn alles sauber läuft: `zeichnen` schrittweise erhöhen, z. B. 600 → 1000 → 1500, und jedes Mal kontrollieren.

---

## Schritt 11 – Werte auf GitHub sichern

Die ermittelten Werte gehören zu **eurem** Roboter. Damit sie nicht verloren gehen und dein Kollege sie auch hat:

1. **GitHub Desktop** öffnen. Links unter **Changes** stehen die geänderten Dateien (`Konfiguration.h`, evtl. `plotter.toml`).
2. Prüfen: In `plotter.toml` darf **nicht** mehr `z_zeichnen = 20.0` vom Lufttest stehen.
3. Links unten bei **Summary** eintragen:
   ```
   Kalibrierung: Geometrie, Nullpositionen und Papierebene eingetragen
   ```
4. **Commit to main** klicken, dann oben **Push origin**.

---

# Fehlersuche

| Meldung / Verhalten | Was tun |
|---|---|
| Serieller Monitor zeigt **nichts** | 115200 baud? Richtiger Port (A3)? `?` eingeben. Reset einmal drücken (sind Motoren an: Plattform festhalten). |
| Serieller Monitor zeigt **komische Zeichen** | Baudrate auf **115200** stellen |
| **Upload-Fehler** | Reset **zweimal schnell** drücken, Port neu wählen, nochmal hochladen. Anderes USB-Kabel probieren (manche können nur laden). |
| `Modus: TROCKENLAUF`, obwohl Motoren dran sind | 12 V an? Dann `m`. |
| `Motor 2 (ID 2) antwortet nicht.` | Kabel von Motor 2 prüfen. Mit `MotorID_Setzen` → `s` nachsehen, welche IDs da sind. |
| `Position unbekannt - zuerst 's' (Startposition anfahren).` | Nach dem Hochladen, nach `k` oder `a` immer zuerst `s` |
| `Nicht erreichbar: X=… Y=… Z=…` | Der Punkt liegt außerhalb des Arbeitsraums oder unter `Z_MIN`. Bei `g`: andere Werte nehmen. Bei `z`/`p`: Zeichenfläche in `plotter.toml` und `PAPIEREBENE_Z` prüfen. |
| `Sollwert ausserhalb 0..360 Grad - NULLPOSITION/RICHTUNG pruefen!` | Schritt 5, Punkt 9 (Bereich) prüfen |
| Motor-LED **blinkt**, Motor macht nichts mehr | Überlastschutz des Motors. 12 V aus und wieder ein, dann `m`, `s`. Ursache suchen: Arm blockiert, zu schnell? |
| Plattform **fällt** beim Hochladen | Normal: Beim Neustart wird kurz der Motorstrom abgeschaltet. Immer festhalten (A4). |
| Arme **ruckeln** | `zeichnen` bzw. `leerfahrt` kleiner |

---

# Messprotokoll

Zum Abschreiben oder Ausdrucken.

**Schritt 4 – Geometrie**

| Maß | Arm 1 | Arm 2 | Arm 3 | Mittelwert | Bericht | Eingetragen? |
|---|---|---|---|---|---|---|
| Oberarm `a` (mm) | | | | | 223,6 | |
| Unterarm `b`, Stange links (mm) | | | | | 460 | |
| Unterarm `b`, Stange rechts (mm) | | | | | 460 | |
| `r_B` Basis (mm) | | | | | 51,1 | |
| `r_P` Plattform (mm) | | | | | 55,8 | |
| `r_P − r_B` (mm) | | | | | 4,7 | |

**Schritt 5 – Kalibrierung**

| Motor | NULLPOSITION (Grad) | RICHTUNG (+1 / −1) | Bereich ok? |
|---|---|---|---|
| M1 | | | |
| M2 | | | |
| M3 | | | |

**Schritt 7 – Papierebene**

| Messung | Wert |
|---|---|
| `PAPIEREBENE_Z` (Roboter-KS z) | |
| Mitte `g 0 0 …` berührt bei Z | |
| rechts oben berührt bei Z | |
| links oben berührt bei Z | |
| links unten berührt bei Z | |
| rechts unten berührt bei Z | |

**Schritt 8 – Testquadrat**

| Seite unten | Seite rechts | Seite oben | Seite links | Diagonale 1 | Diagonale 2 |
|---|---|---|---|---|---|
| | | | | | |
| Soll 100 | Soll 100 | Soll 100 | Soll 100 | Soll 141,4 | Soll 141,4 |
