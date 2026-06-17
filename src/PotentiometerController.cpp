#include "PotentiometerController.h"

// Глобальные переменные для хранения текущих значений (только внутри модуля)
static int currentVolume = -1;
static int currentBass = -1;

static bool writePotBytes(uint8_t deviceAddr, uint8_t cmd, int value) {
  if (value < 0) value = 0;
  if (value > POT_MAX_VALUE) value = POT_MAX_VALUE;

  Wire.beginTransmission(deviceAddr);
  if (Wire.write(cmd) != 1 || Wire.write((uint8_t)value) != 1) {
    Wire.endTransmission();
    return false;
  }
  return (Wire.endTransmission() == 0);
}

bool setPotValue(uint8_t deviceAddr, int value) {
  return writePotBytes(deviceAddr, MCP4561_CMD_WRITE_WIPER0, value);
}

bool setPotValueMemory(uint8_t deviceAddr, int value) {
  // Запись в RAM
  if (!writePotBytes(deviceAddr, MCP4561_CMD_WRITE_WIPER0, value)) {
    return false;
  }
  // Запись в NV
  return writePotBytes(deviceAddr, MCP4561_CMD_WRITE_NV_WIPER0, value);
}

bool setVolume(int value) {
  bool success = setPotValue(MCP4561_ADDR_VOLUME, value);
  if (success) currentVolume = value;
  return success;
}

bool setVolumeMemory(int value) {
  bool success = setPotValueMemory(MCP4561_ADDR_VOLUME, value);
  if (success) currentVolume = value;
  return success;
}

bool setBassLevel(int value) {
  bool success = setPotValueMemory(MCP4561_ADDR_BASS, value);
  if (success) currentBass = value;
  return success;
}


int getVolume() {
  return currentVolume;
}

int getBassLevel() {
  return currentBass;
}
