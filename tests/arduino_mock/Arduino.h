// Minimaler Ersatz für Arduino.h, damit der Beispiel-Sketch in den Tests
// auf dem PC kompiliert und ausgeführt werden kann. Die print-Überladungen
// entsprechen denen der echten Arduino-Print-Klasse.
#pragma once
#include <cmath>
#include <cstdint>
#include <cstdio>

struct MockSerial {
  void begin(unsigned long) {}
  explicit operator bool() const { return true; }
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
unsigned long millis();
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
