// Minimaler Ersatz fuer Arduino.h, damit der Sketch arduino/DeltaPlotter in
// den Tests auf dem PC kompiliert und ausgefuehrt werden kann.
// Die print-Ueberladungen entsprechen denen der echten Arduino-Print-Klasse.
// Eingaben fuer den Seriellen Monitor werden mit Zeitpunkt eingeplant.
#pragma once
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <string>
#include <utility>

unsigned long millis();
unsigned long micros();
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);

struct MockSerial {
  std::deque<std::pair<unsigned long, std::string>> geplant;  // (Zeitpunkt in ms, Text)
  std::string puffer;

  void begin(unsigned long) {}
  explicit operator bool() const { return true; }
  void nachladen() {
    while (!geplant.empty() && geplant.front().first <= millis()) {
      puffer += geplant.front().second;
      geplant.pop_front();
    }
  }
  int available() { nachladen(); return (int)puffer.size(); }
  int read() {
    nachladen();
    if (puffer.empty()) return -1;
    const int c = (unsigned char)puffer[0];
    puffer.erase(0, 1);
    return c;
  }
  void print(const char* s) { std::printf("%s", s); }
  void print(char c) { std::printf("%c", c); }
  void print(unsigned char v, int = 10) { std::printf("%u", (unsigned)v); }
  void print(int v, int = 10) { std::printf("%d", v); }
  void print(unsigned int v, int = 10) { std::printf("%u", v); }
  void print(long v, int = 10) { std::printf("%ld", v); }
  void print(unsigned long v, int = 10) { std::printf("%lu", v); }
  void print(double v, int d = 2) { std::printf("%.*f", d, v); }
  void println() { std::printf("\n"); }
  template <typename T> void println(T v) { print(v); println(); }
  template <typename T> void println(T v, int d) { print(v, d); println(); }
};

extern MockSerial Serial;
extern MockSerial Serial1;
