#ifndef POTENTIOMETER_CONTROLLER_H
#define POTENTIOMETER_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>

// Адреса и константы MCP4561
static constexpr uint8_t MCP4561_ADDR_VOLUME = 0x2F;
static constexpr uint8_t MCP4561_ADDR_BASS   = 0x2E;
static constexpr uint8_t MCP4561_CMD_WRITE_WIPER0    = 0x00;
static constexpr uint8_t MCP4561_CMD_WRITE_NV_WIPER0 = 0x20;
static constexpr int POT_MAX_VALUE = 255;

// Функции установки значений
bool setPotValue(uint8_t deviceAddr, int value);           // только RAM
bool setPotValueMemory(uint8_t deviceAddr, int value);     // RAM + NV

// Функции для конкретных потенциометров (возвращают true при успехе)
bool setVolume(int value);
bool setVolumeMemory(int value);
bool setBassLevel(int value);

// Глобальные переменные для хранения текущих значений (если нужны извне)
extern int currentVolume;
extern int currentBass;

#endif
