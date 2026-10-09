// Ersatz fuer die Bibliothek Dynamixel2Arduino (nur die Teile, die der Sketch nutzt).
// Die Motoren erreichen jede Sollposition sofort; alle SyncWrite-Pakete werden
// mitgeschrieben, damit die Tests die gesendeten Sollwerte pruefen koennen.
#pragma once
#include <array>
#include <vector>
#include "Arduino.h"

enum { OP_POSITION = 3 };
enum { UNIT_RAW = 0, UNIT_DEGREE = 2 };
namespace ControlTableItem {
enum { PROFILE_ACCELERATION = 84, PROFILE_VELOCITY = 85 };
}
namespace DYNAMIXEL {
struct InfoSyncBulkBuffer_t { uint8_t* p_buf; uint16_t buf_capacity; bool is_completed; };
struct XELInfoSyncWrite_t { uint8_t* p_data; uint8_t id; };
struct InfoSyncWriteInst_t {
  uint16_t addr;
  uint16_t addr_length;
  XELInfoSyncWrite_t* p_xels;
  uint8_t xel_count;
  bool is_info_changed;
  InfoSyncBulkBuffer_t packet;
};
}  // namespace DYNAMIXEL

extern bool MOCK_PING_OK;
extern std::vector<std::array<int32_t, 3>> MOCK_SYNC;
extern int32_t MOCK_POS[256];
extern bool MOCK_TORQUE[256];

class Dynamixel2Arduino {
 public:
  Dynamixel2Arduino(MockSerial&, int) {}
  void begin(unsigned long) {}
  bool setPortProtocolVersion(float) { return true; }
  bool ping(uint8_t) { return MOCK_PING_OK; }
  bool torqueOff(uint8_t id) { MOCK_TORQUE[id] = false; return true; }
  bool torqueOn(uint8_t id) { MOCK_TORQUE[id] = true; return true; }
  bool setOperatingMode(uint8_t, uint8_t) { return true; }
  bool writeControlTableItem(uint8_t, uint8_t, int32_t, uint32_t = 100) { return true; }
  bool setGoalPosition(uint8_t id, float v, uint8_t = UNIT_RAW) {
    if (!MOCK_TORQUE[id]) MOCK_POS[id] = (int32_t)v;
    return true;
  }
  float getPresentPosition(uint8_t id, uint8_t unit = UNIT_RAW) {
    return unit == UNIT_DEGREE ? MOCK_POS[id] * 360.0f / 4096.0f : (float)MOCK_POS[id];
  }
  bool syncWrite(DYNAMIXEL::InfoSyncWriteInst_t* p) {
    std::array<int32_t, 3> werte{};
    for (int i = 0; i < p->xel_count; i++) {
      int32_t v;
      std::memcpy(&v, p->p_xels[i].p_data, 4);
      werte[i] = v;
      if (MOCK_TORQUE[p->p_xels[i].id]) MOCK_POS[p->p_xels[i].id] = v;
      else std::printf("!! SyncWrite ohne Drehmoment\n");
    }
    MOCK_SYNC.push_back(werte);
    return true;
  }
  int getLastLibErrCode() { return 0; }
};
