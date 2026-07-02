#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <Arduino.h>

class ButtonController {
public:
  ButtonController();

  // Инициализация пинов (вызывать в setup())
  void begin();

  void press(uint8_t button);
  void release(uint8_t button);
  void releaseAll();

  uint8_t getPressedButton() const;

private:
  static const uint8_t BUTTON_COUNT = 7;
  static const uint8_t BUTTON_PINS[BUTTON_COUNT];
  uint8_t _pressedButton;
};

#endif
