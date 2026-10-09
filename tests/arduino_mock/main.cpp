// Fuehrt den Sketch auf dem PC aus. Die Zeit wird nur simuliert.
//
// Aufruf:  sketch PING "ms:befehl" "ms:befehl" ...
//   PING = 1 -> Motoren antworten (aktiv), 0 -> keine Motoren (Trockenlauf)
//   z.B.     sketch 0 "100:p" "200:z"
//
// Auf stderr: Anzahl der SyncWrite-Pakete, simulierte Dauer, letzter Sollwert.
#include "Arduino.h"
#include "Dynamixel2Arduino.h"

MockSerial Serial, Serial1;
bool MOCK_PING_OK = true;
std::vector<std::array<int32_t, 3>> MOCK_SYNC;
int32_t MOCK_POS[256];
bool MOCK_TORQUE[256];

static unsigned long long simUs = 0;
unsigned long millis() { return (unsigned long)(simUs / 1000ULL); }
unsigned long micros() { simUs += 50; return (unsigned long)simUs; }
void delay(unsigned long ms) { simUs += ms * 1000ULL; }
void delayMicroseconds(unsigned int us) { simUs += us; }

void setup();
void loop();

int main(int argc, char** argv) {
  MOCK_PING_OK = argc > 1 && argv[1][0] == '1';
  for (int i = 0; i < 256; i++) MOCK_POS[i] = 1500;  // Arme haengen irgendwo
  for (int i = 2; i < argc; i++) {
    const std::string a = argv[i];
    const size_t k = a.find(':');
    Serial.geplant.push_back({std::stoul(a.substr(0, k)), a.substr(k + 1) + "\n"});
  }
  setup();
  const unsigned long ende = Serial.geplant.empty() ? 0 : Serial.geplant.back().first + 3600000UL;
  while (millis() < ende) {
    loop();
    simUs += 1000;
    if (Serial.geplant.empty() && Serial.puffer.empty()) {
      for (int k = 0; k < 300; k++) { loop(); simUs += 1000; }
      break;
    }
  }
  std::fprintf(stderr, "SYNC=%zu\nZEIT_S=%.1f\n", MOCK_SYNC.size(), simUs / 1e6);
  if (!MOCK_SYNC.empty()) {
    const auto& w = MOCK_SYNC.back();
    std::fprintf(stderr, "LETZTER_SOLLWERT=%d,%d,%d\n", w[0], w[1], w[2]);
  }
  return 0;
}
