"""Tests für den Deltaroboter-Converter.  Ausführen mit:  python -m pytest"""

from __future__ import annotations

import re
import shutil
import subprocess
from pathlib import Path

import cv2
import numpy as np
import pytest

from deltaconvert.ausgabe_arduino import c_bezeichner, erzeuge_header
from deltaconvert.ausgabe_gcode import erzeuge_gcode
from deltaconvert.cli import main
from deltaconvert.geometrie import einpassen, laenge, reihenfolge_optimieren, vereinfachen
from deltaconvert.konfig import KonfigFehler, Konfig, konfig_aus_dict, lade_konfig
from deltaconvert.konverter import konvertiere
from deltaconvert.werkzeugweg import STIFT_OBEN, STIFT_UNTEN, berechne_statistik, erzeuge_werkzeugweg

REPO = Path(__file__).resolve().parent.parent


# ---------------------------------------------------------------- Hilfen
def bild_speichern(pfad: Path, img: np.ndarray) -> Path:
    cv2.imwrite(str(pfad), img)
    return pfad


@pytest.fixture
def quadrat_bild(tmp_path: Path) -> Path:
    img = np.full((200, 200), 255, np.uint8)
    cv2.rectangle(img, (50, 50), (150, 150), 0, -1)  # gefülltes schwarzes Quadrat
    return bild_speichern(tmp_path / "quadrat.png", img)


@pytest.fixture
def linien_bild(tmp_path: Path) -> Path:
    img = np.full((200, 300), 255, np.uint8)
    cv2.line(img, (20, 100), (280, 100), 0, 9)  # eine dicke waagrechte Linie
    return bild_speichern(tmp_path / "linie.png", img)


# ---------------------------------------------------------------- Konfiguration
def test_konfig_datei_im_repo_ist_gueltig():
    cfg = lade_konfig(REPO / "config" / "plotter.toml")
    assert cfg.zeichenflaeche.breite > 0


def test_konfig_tippfehler_wird_gemeldet():
    with pytest.raises(KonfigFehler, match="breit"):
        konfig_aus_dict({"zeichenflaeche": {"breit": 100}})
    with pytest.raises(KonfigFehler, match="stfit"):
        konfig_aus_dict({"stfit": {}})


def test_konfig_unplausible_werte():
    with pytest.raises(KonfigFehler):
        konfig_aus_dict({"stift": {"z_zeichnen": 5, "z_heben": 2}})
    with pytest.raises(KonfigFehler):
        konfig_aus_dict({"ausrichtung": {"drehung": 45}})
    with pytest.raises(KonfigFehler):
        konfig_aus_dict({"zeichenflaeche": {"breite": 20, "rand": 10}})


# ---------------------------------------------------------------- Geometrie
def test_einpassen_haelt_seitenverhaeltnis_und_rand():
    striche = [np.array([[0, 0], [400, 0], [400, 100], [0, 100], [0, 0]], float)]
    mm, massstab = einpassen(striche, 200, 200, 10)
    alle = np.vstack(mm)
    assert np.allclose(alle.min(0), [-90, -22.5])
    assert np.allclose(alle.max(0), [90, 22.5])
    assert massstab == pytest.approx(180 / 400)


def test_vereinfachen_entfernt_punkte_auf_gerader():
    s = np.column_stack([np.linspace(0, 10, 101), np.zeros(101)])
    v = vereinfachen(s, 0.01)
    assert len(v) == 2 and np.allclose(v, [[0, 0], [10, 0]])


def test_reihenfolge_verkuerzt_leerfahrten():
    rng = np.random.default_rng(1)
    striche = [np.array([p, p + [1, 0]]) for p in rng.uniform(-80, 80, (60, 2))]

    def leerweg(liste):
        pos, summe = np.zeros(2), 0.0
        for s in liste:
            summe += np.hypot(*(s[0] - pos))
            pos = s[-1]
        return summe

    sortiert = reihenfolge_optimieren(striche, (0, 0))
    assert len(sortiert) == len(striche)
    assert leerweg(sortiert) < leerweg(striche) / 2


# ---------------------------------------------------------------- Bild → Striche
def test_kontur_eines_quadrats(quadrat_bild: Path):
    cfg = Konfig()
    erg = konvertiere(quadrat_bild, cfg)
    assert len(erg.striche) == 1
    alle = np.vstack(erg.striche)
    # Quadrat füllt die Fläche 200 - 2*10 = 180 mm
    assert np.allclose(alle.max(0) - alle.min(0), [180, 180], atol=0.5)
    assert laenge(erg.striche[0]) == pytest.approx(4 * 180, rel=0.03)


def test_mittellinie_einer_dicken_linie(linien_bild: Path):
    cfg = Konfig()
    cfg.bild.modus = "mittellinie"
    erg = konvertiere(linien_bild, cfg)
    assert len(erg.striche) == 1  # nur einmal gezeichnet, nicht als Umriss
    assert laenge(erg.striche[0]) == pytest.approx(180, rel=0.02)


def test_kanten_modus_findet_linien(quadrat_bild: Path):
    cfg = Konfig()
    cfg.bild.modus = "kanten"
    erg = konvertiere(quadrat_bild, cfg)
    assert erg.statistik.zeichenweg > 500


def test_leeres_bild_gibt_verstaendlichen_fehler(tmp_path: Path):
    leer = bild_speichern(tmp_path / "leer.png", np.full((50, 50), 255, np.uint8))
    with pytest.raises(ValueError, match="Keine Linien"):
        konvertiere(leer, Konfig())


def test_svg_eingabe(tmp_path: Path):
    svg = tmp_path / "form.svg"
    svg.write_text(
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 50">'
        '<rect x="0" y="0" width="100" height="50" fill="none" stroke="black"/>'
        '<circle cx="50" cy="25" r="10"/></svg>'
    )
    erg = konvertiere(svg, Konfig())
    assert len(erg.striche) == 2
    alle = np.vstack(erg.striche)
    assert np.allclose(alle.max(0) - alle.min(0), [180, 90], atol=0.01)
    # Ecken des Rechtecks bleiben exakt erhalten
    assert np.any(np.all(np.isclose(alle, [90, 45]), axis=1))


# ---------------------------------------------------------------- Werkzeugweg & Ausgaben
def test_werkzeugweg_verbindet_nahe_striche():
    a = np.array([[0.0, 0.0], [10.0, 0.0]])
    b = np.array([[10.1, 0.0], [20.0, 0.0]])
    c = np.array([[50.0, 0.0], [60.0, 0.0]])
    weg = erzeuge_werkzeugweg([a, b, c], verbinden_abstand=0.3)
    assert list(weg.stift) == [STIFT_OBEN, STIFT_UNTEN, STIFT_UNTEN, STIFT_UNTEN, STIFT_OBEN, STIFT_UNTEN]


def test_gcode_ablauf_start_zeichnen_ende(quadrat_bild: Path):
    cfg = Konfig()
    erg = konvertiere(quadrat_bild, cfg)
    g = erzeuge_gcode(erg.weg, cfg, "quadrat.png", erg.statistik, "test")
    befehle = [z.split(";")[0].strip() for z in g.splitlines() if z.split(";")[0].strip()]
    assert befehle[0] == "G21" and befehle[1] == "G90"
    assert befehle[2].startswith("G0 X0.00 Y0.00 Z30.00")  # Startposition
    assert befehle[-1].startswith("G0 X0.00 Y0.00 Z30.00")  # Endposition
    assert befehle[-2].startswith("G1 Z10.00")  # vorher Stift heben
    # gezeichnet (G1 X/Y) wird nur mit abgesetztem Stift
    z = None
    for b in befehle:
        m = re.search(r"Z(-?[\d.]+)", b)
        if m:
            z = float(m.group(1))
        if b.startswith("G1 X"):
            assert z == cfg.stift.z_zeichnen


def test_header_kompiliert_und_stimmt(tmp_path: Path, quadrat_bild: Path):
    gpp = shutil.which("g++")
    if not gpp:
        pytest.skip("g++ nicht installiert")
    cfg = Konfig()
    erg = konvertiere(quadrat_bild, cfg)
    (tmp_path / "quadrat.h").write_text(erzeuge_header(erg.weg, cfg, "quadrat", "quadrat.png", erg.statistik, "t"))
    # zweite Zeichnung gleichzeitig einbinden → Include-Guards müssen funktionieren
    (tmp_path / "zwei.h").write_text(erzeuge_header(erg.weg, cfg, "zwei", "quadrat.png", erg.statistik, "t"))
    (tmp_path / "t.cpp").write_text(
        '#include <cstdio>\n#include "quadrat.h"\n#include "zwei.h"\n#include "quadrat.h"\n'
        "int main(){ long sx=0; for(uint32_t i=0;i<zeichnung_quadrat.anzahl;i++) sx+=zeichnung_quadrat.punkte[i].x;"
        ' std::printf("%u %ld %d\\n", (unsigned)zeichnung_quadrat.anzahl, sx, (int)sizeof(DeltaPunkt)); }\n'
    )
    subprocess.run([gpp, "-std=c++11", "-Wall", "-Werror", "t.cpp", "-o", "t"], cwd=tmp_path, check=True)
    out = subprocess.run([str(tmp_path / "t")], capture_output=True, text=True, check=True).stdout.split()
    assert int(out[0]) == len(erg.weg)
    assert int(out[1]) == int((erg.weg.xy[:, 0] * 100).round().sum())
    assert int(out[2]) == 6


def test_statistik_dauer_passt_zum_werkzeugweg(quadrat_bild: Path):
    cfg = Konfig()
    erg = konvertiere(quadrat_bild, cfg)
    st = berechne_statistik(erg.weg, cfg)
    erwartet = st.zeichenweg / cfg.vorschub.zeichnen * 60
    assert st.dauer_s > erwartet


def test_c_bezeichner():
    assert c_bezeichner("Bär & Katze-2") == "baer_katze_2"
    assert c_bezeichner("3eck") == "z_3eck"


# ---------------------------------------------------------------- Kommandozeile
def test_kommandozeile_erzeugt_alle_dateien(tmp_path: Path, quadrat_bild: Path):
    code = main([str(quadrat_bild), "-o", str(tmp_path / "aus"), "-c", str(REPO / "config" / "plotter.toml")])
    assert code == 0
    for datei in ("quadrat.gcode", "quadrat.h", "quadrat_vorschau.svg"):
        assert (tmp_path / "aus" / datei).stat().st_size > 0


def test_kommandozeile_meldet_fehler(tmp_path: Path, capsys):
    assert main([str(tmp_path / "gibtsnicht.png")]) == 1
    assert "nicht gefunden" in capsys.readouterr().err
