#include "ModeManager.h"
#include <EEPROM.h>

ModeManager::ModeManager()
    : _mode(STANDARD) {
}

void ModeManager::begin() {
    uint8_t magic = EEPROM.read(EEPROM_MAGIC_ADDR);
    if (magic == EEPROM_MAGIC) {
        _mode = static_cast<Mode>(EEPROM.read(EEPROM_MODE_ADDR));
        if (_mode != STANDARD && _mode != START_MIN_VALUE) {
            _mode = STANDARD;
        }
    } else {
        _mode = STANDARD;
        EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
        EEPROM.update(EEPROM_MODE_ADDR, _mode);
    }
}

ModeManager::Mode ModeManager::getMode() const {
    return _mode;
}

bool ModeManager::setMode(Mode mode) {
    if (mode != STANDARD && mode != START_MIN_VALUE) {
        return false;
    }
    _mode = mode;
    EEPROM.update(EEPROM_MODE_ADDR, _mode);
    return true;
}

const char* ModeManager::getModeName() const {
    switch (_mode) {
        case STANDARD:
            return "standard";
        case START_MIN_VALUE:
            return "start_min_value";
        default:
            return "unknown";
    }
}

ModeManager::Mode ModeManager::modeFromName(const char* name) {
    if (strcmp(name, "start_min_value") == 0) {
        return START_MIN_VALUE;
    }
    return STANDARD;
}
