#include "RemController.h"

RemController::RemController()
  : _enabled(false) {
}

void RemController::begin() {
  pinMode(REM_PIN, OUTPUT);
  digitalWrite(REM_PIN, LOW);
}

void RemController::enable(bool on) {
  _enabled = on;
  digitalWrite(REM_PIN, on ? HIGH : LOW);
}

void RemController::enable() {
  enable(true);
}

void RemController::disable() {
  enable(false);
}

bool RemController::isEnabled() const {
  return _enabled;
}
