#include "ButtonController.h"

const uint8_t ButtonController::BUTTON_PINS[7] = {10, 8, 7, 6, 5, 4, 3};

ButtonController::ButtonController()
  : _pressedButton(0) {
}

void ButtonController::begin() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_PINS[i], OUTPUT);
    digitalWrite(BUTTON_PINS[i], LOW);
  }
}

void ButtonController::press(uint8_t button) {
  if (button < 1 || button > BUTTON_COUNT) return;

  if (_pressedButton != 0 && _pressedButton != button) {
    digitalWrite(BUTTON_PINS[_pressedButton - 1], LOW);
  }

  digitalWrite(BUTTON_PINS[button - 1], HIGH);
  _pressedButton = button;
}

void ButtonController::release(uint8_t button) {
  if (button < 1 || button > BUTTON_COUNT) return;

  if (_pressedButton == button) {
    digitalWrite(BUTTON_PINS[button - 1], LOW);
    _pressedButton = 0;
  }
}

void ButtonController::releaseAll() {
  if (_pressedButton != 0) {
    digitalWrite(BUTTON_PINS[_pressedButton - 1], LOW);
    _pressedButton = 0;
  }
}

uint8_t ButtonController::getPressedButton() const {
  return _pressedButton;
}
