// Führt setup() des Sketches einmal aus. Die Zeit wird nur simuliert,
// am Ende wird die simulierte Gesamtdauer auf stderr ausgegeben.
#include "Arduino.h"

MockSerial Serial;
static unsigned long long simulierteUs = 0;

unsigned long millis() { return (unsigned long)(simulierteUs / 1000ULL); }
void delay(unsigned long ms) { simulierteUs += (unsigned long long)ms * 1000ULL; }
void delayMicroseconds(unsigned int us) { simulierteUs += us; }

void setup();

int main() {
  setup();
  std::fprintf(stderr, "SIMULIERTE_DAUER_S=%.1f\n", simulierteUs / 1e6);
  return 0;
}
