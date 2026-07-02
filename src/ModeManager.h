#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <Arduino.h>

class ModeManager {
public:
    enum Mode : uint8_t {
        STANDARD = 0,
        START_MIN_VALUE = 1
    };

    ModeManager();

    void begin();
    Mode getMode() const;
    bool setMode(Mode mode);
    const char* getModeName() const;
    static Mode modeFromName(const char* name);

private:
    Mode _mode;

    static const int EEPROM_MAGIC_ADDR = 0;
    static const int EEPROM_MODE_ADDR  = 1;
    static const uint8_t EEPROM_MAGIC = 0xA5;
};

#endif
