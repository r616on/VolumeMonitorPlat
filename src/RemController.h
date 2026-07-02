#ifndef REM_CONTROLLER_H
#define REM_CONTROLLER_H

#include <Arduino.h>

class RemController {
public:
  RemController();

  void begin();
  void enable(bool on);
  void enable();
  void disable();
  bool isEnabled() const;

private:
  static constexpr uint8_t REM_PIN = A2;
  bool _enabled;
};

#endif
