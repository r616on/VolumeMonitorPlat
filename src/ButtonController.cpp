#include "ButtonController.h"

// Таблица соответствия номера кнопки (1–7) и пина Arduino, к которому она подключена.
// Кнопка 1 = пин 10, кнопка 2 = пин 8, …, кнопка 7 = пин 3.
const uint8_t ButtonController::BUTTON_PINS[7] = {10, 8, 7, 6, 5, 4, 3};

// Конструктор: в начальный момент ни одна кнопка не нажата (_pressedButton = 0).
ButtonController::ButtonController()
  : _pressedButton(0) {
}

// Инициализация всех пинов кнопок как выходов и перевод их в LOW (кнопки отпущены).
// Вызывается один раз в setup().
void ButtonController::begin() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_PINS[i], OUTPUT);   // настроить пин как выход
    digitalWrite(BUTTON_PINS[i], LOW); // гарантированно выключить сигнал
  }
}

// «Нажать» кнопку — установить её пин в HIGH.
// Если в данный момент нажата другая кнопка, она сначала отпускается.
void ButtonController::press(uint8_t button) {
  // Проверка допустимого диапазона (1..BUTTON_COUNT)
  if (button < 1 || button > BUTTON_COUNT) return;

  // Если другая кнопка уже нажата — отпустить её
  if (_pressedButton != 0 && _pressedButton != button) {
    digitalWrite(BUTTON_PINS[_pressedButton - 1], LOW);
  }

  // Установить сигнал на нужном пине и запомнить номер нажатой кнопки
  digitalWrite(BUTTON_PINS[button - 1], HIGH);
  _pressedButton = button;
}

// «Отпустить» конкретную кнопку — перевести её пин в LOW.
// Действие выполняется только если переданная кнопка действительно нажата.
void ButtonController::release(uint8_t button) {
  // Проверка допустимого диапазона (1..BUTTON_COUNT)
  if (button < 1 || button > BUTTON_COUNT) return;

  // Отпускаем только если переданная кнопка совпадает с текущей нажатой
  if (_pressedButton == button) {
    digitalWrite(BUTTON_PINS[button - 1], LOW);
    _pressedButton = 0; // теперь ни одна кнопка не нажата
  }
}

// Немедленно отпустить любую нажатую кнопку (если такая есть).
void ButtonController::releaseAll() {
  if (_pressedButton != 0) {
    digitalWrite(BUTTON_PINS[_pressedButton - 1], LOW);
    _pressedButton = 0;
  }
}

// Возвращает номер нажатой в данный момент кнопки, либо 0 если ни одна не нажата.
uint8_t ButtonController::getPressedButton() const {
  return _pressedButton;
}
