"""Tests fuer den Arduino-Sketch arduino/DeltaPlotter.

Der Sketch wird mit einem Arduino- und Dynamixel-Ersatz (tests/arduino_mock)
auf dem PC kompiliert und ausgefuehrt. Geprueft werden der Ablauf
Start -> Zeichnung -> Ende, die Kinematik (gegen eine eigene Python-Rechnung
mit den Werten aus Konfiguration.h), die an die Motoren gesendeten Sollwerte,
der Abbruch und die Sicherheitspruefungen.

Benoetigt g++ (unter Linux/GitHub vorhanden), sonst werden die Tests uebersprungen.
"""

from __future__ import annotations

import math
import re
import shutil
import subprocess
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parent.parent
SKETCH = REPO / "arduino" / "DeltaPlotter"
MOCK = REPO / "tests" / "arduino_mock"


# ---------------------------------------------------------------- Hilfen
def konstante(name: str) -> float | list[float]:
    """Liest eine Konstante aus Konfiguration.h (Zahl oder {a, b, c})."""
    text = (SKETCH / "Konfiguration.h").read_text()
    m = re.search(rf"\b{name}(?:\[3\])?\s*=\s*([^;]+);", text)
    assert m, f"{name} nicht in Konfiguration.h gefunden"
    wert = m.group(1).strip()
    zahlen = [float(z.rstrip("fF")) for z in re.findall(r"-?\d+(?:\.\d+)?[fF]?", wert)]
    return zahlen if wert.startswith("{") else zahlen[0]


def inverse_kinematik(x: float, y: float, z: float) -> list[float]:
    """Unabhaengige Python-Rechnung (double) mit den Werten aus Konfiguration.h."""
    a, b = konstante("OBERARM_A"), konstante("UNTERARM_B")
    rb, rp = konstante("RADIUS_BASIS"), konstante("RADIUS_PLATTFORM")
    phi = []
    for theta in konstante("MOTOR_THETA"):
        t = math.radians(theta)
        d = y * math.cos(t) - x * math.sin(t)
        e = rp - rb + x * math.cos(t) + y * math.sin(t)
        b_s = math.sqrt(b * b - d * d)
        c_s = math.hypot(e, z)
        beta = math.acos((a * a + c_s * c_s - b_s * b_s) / (2 * a * c_s))
        phi.append(math.degrees(math.acos(e / c_s) - beta))
    return phi


def papier_zu_roboter(x: float, y: float, z: float) -> tuple[float, float, float]:
    dr = math.radians(konstante("PAPIER_DREHUNG"))
    xr = konstante("PAPIER_MITTE_X") + math.cos(dr) * x - math.sin(dr) * y
    yr = konstante("PAPIER_MITTE_Y") + math.sin(dr) * x + math.cos(dr) * y
    return xr, yr, konstante("PAPIEREBENE_Z") + z + konstante("STIFT_UEBERSTAND")


def endposition() -> tuple[float, float, float]:
    """Endposition aus der im Sketch eingebundenen Zeichnung."""
    ino = (SKETCH / "DeltaPlotter.ino").read_text()
    header = re.search(r'#include\s+"(\w+\.h)"\s*\nconst DeltaZeichnung', ino).group(1)
    zeile = next(z for z in (SKETCH / header).read_text().splitlines() if "// Endposition" in z)
    return tuple(float(v.rstrip("fF")) for v in re.findall(r"-?\d+\.\d+f", zeile))


@pytest.fixture(scope="module")
def sketch(tmp_path_factory) -> Path:
    gpp = shutil.which("g++")
    if not gpp:
        pytest.skip("g++ nicht installiert")
    exe = tmp_path_factory.mktemp("sketch") / "sketch"
    subprocess.run(
        [gpp, "-std=gnu++17", "-O1", "-Wall", "-Wextra", "-Werror", f"-I{MOCK}", f"-I{SKETCH}",
         "-include", "Arduino.h", "-x", "c++", str(SKETCH / "DeltaPlotter.ino"),
         "-x", "c++", str(MOCK / "main.cpp"), "-o", str(exe)],
        check=True,
    )
    return exe


def starte(sketch: Path, motoren: bool, *befehle: str) -> tuple[list[str], dict[str, str]]:
    lauf = subprocess.run([str(sketch), "1" if motoren else "0", *befehle],
                          capture_output=True, text=True, timeout=120, check=True)
    info = dict(z.split("=", 1) for z in lauf.stderr.split() if "=" in z)
    return lauf.stdout.splitlines(), info


def positionszeilen(ausgabe: list[str]) -> list[list[float]]:
    return [[float(v) for v in z.split("\t")] for z in ausgabe if re.match(r"^-?\d+\.\d+\t", z)]


# ---------------------------------------------------------------- Tests
def test_trockenlauf_faehrt_start_zeichnung_ende(sketch: Path):
    ausgabe, _ = starte(sketch, False, "100:p", "200:z")
    text = "\n".join(ausgabe)
    assert "TROCKENLAUF" in text
    assert "alle erreichbar" in text
    assert ausgabe[-1] == "Fertig."
    pos = positionszeilen(ausgabe)
    assert len(pos) > 100
    assert pos[-1][:3] == pytest.approx(list(endposition()), abs=0.01)


def test_kinematik_stimmt_mit_python_rechnung(sketch: Path):
    ausgabe, _ = starte(sketch, False, "100:z")
    pos = positionszeilen(ausgabe)
    abweichung = max(
        abs(p - q)
        for x, y, z, *phi in pos[::25]
        for p, q in zip(phi, inverse_kinematik(*papier_zu_roboter(x, y, z)))
    )
    assert abweichung < 0.05  # Grad (float auf dem Board vs. double hier)


def test_bewegung_in_kleinen_schritten(sketch: Path):
    pos = positionszeilen(starte(sketch, False, "100:z")[0])
    schritte = [math.dist(p[:3], q[:3]) for p, q in zip(pos, pos[1:])]
    assert max(schritte) < 1.0  # mm zwischen zwei Sollwerten


def test_motoren_bekommen_richtige_sollwerte(sketch: Path):
    trocken = positionszeilen(starte(sketch, False, "100:z")[0])
    ausgabe, info = starte(sketch, True, "100:z")
    assert ausgabe[-1] == "Fertig."
    assert int(info["SYNC"]) == len(trocken)  # ein SyncWrite pro Sollwert
    phi_ende = inverse_kinematik(*papier_zu_roboter(*endposition()))
    null, richtung = konstante("NULLPOSITION"), konstante("RICHTUNG")
    erwartet = [round((n + r * p) * 4096 / 360) for n, r, p in zip(null, richtung, phi_ende)]
    gesendet = [int(v) for v in info["LETZTER_SOLLWERT"].split(",")]
    assert all(abs(g - e) <= 1 for g, e in zip(gesendet, erwartet))


def test_eingabe_bricht_zeichnen_ab(sketch: Path):
    ausgabe, info = starte(sketch, True, "100:z", "5000:x")
    assert "ABGEBROCHEN - Stift wird angehoben." in ausgabe
    assert "Fertig." not in ausgabe
    assert float(info["ZEIT_S"]) < 15


def test_unerreichbare_ziele_werden_abgelehnt(sketch: Path):
    ausgabe, info = starte(sketch, True, "100:g 0 0 10", "5000:g 0 0 -100", "6000:g 0 0")
    text = "\n".join(ausgabe)
    assert "Nicht erreichbar: X=0.0 Y=0.0 Z=-100.0" in text  # unter Z_MIN
    assert "Bitte so eingeben" in text  # Zahlen fehlen
    assert text.count("Stift Z =") == 1  # nur die erste Fahrt wurde ausgefuehrt


def test_stift_hoehe_schrittweise(sketch: Path):
    ausgabe, _ = starte(sketch, True, "100:g 0 0 10", "5000:-", "6000:-", "7000:+")
    hoehen = [float(re.search(r"Stift Z = (-?[\d.]+)", z).group(1)) for z in ausgabe if "Stift Z =" in z]
    assert hoehen == [10.0, 9.5, 9.0, 9.5]
