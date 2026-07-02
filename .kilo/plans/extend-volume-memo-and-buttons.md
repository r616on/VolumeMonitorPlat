# План расширения функционала VolumeMonitor328p

## 1. Команда `set_volume_memo`

### Описание
При получении команды `{"command":"set_volume_memo","value":$value}` сохранить значение громкости в энергонезависимую память (NV) потенциометра MCP4561.

### Реализация
- **PotentiometerController.h** — добавить объявление функции `bool setVolumeMemory(int value);`
- **PotentiometerController.cpp** — добавить реализацию: вызов `setPotValueMemory(MCP4561_ADDR_VOLUME, value)` с обновлением `currentVolume` при успехе.
- **main.cpp** — добавить ветку `else if (strcmp(command, "set_volume_memo") == 0)` в `parseCommand()` по аналогии с `set_volume`, но вызывающую `setVolumeMemory()`.

### SOLID
Изменения минимальны, логика остаётся в PotentiometerController (Single Responsibility — управление потенциометрами).

---

## 2. Обработка кнопок через оптопары (D10, D8, D7, D6, D5, D4, D3)

### Описание
К портам D10, D8, D7, D6, D5, D4, D3 подключены аноды оптопар через токоограничивающие резисторы. Эти 7 пинов считаются кнопками 1–7. Команды:
- `{"command":"button_down","value":$value}` — нажать кнопку $value (1–7)
- `{"command":"button_up","value":$value}` — отпустить кнопку $value (1–7)

Одновременно может быть нажата только одна кнопка. Если приходит `button_down` для другой кнопки, предыдущая автоматически отпускается.

### Реализация

#### Новый класс `ButtonController` (файлы `ButtonController.h` / `ButtonController.cpp`)
Следуя SOLID, выделяем управление кнопками в отдельный класс:

```cpp
class ButtonController {
public:
    ButtonController();                          // инициализирует пины как OUTPUT, LOW
    void press(uint8_t button);                  // нажать кнопку 1-7
    void release(uint8_t button);                // отпустить кнопку 1-7
    void releaseAll();                           // отпустить все
    uint8_t getPressedButton() const;            // вернуть номер нажатой кнопки или 0
private:
    static const uint8_t BUTTON_PINS[7];         // {10, 8, 7, 6, 5, 4, 3}
    uint8_t _pressedButton;                      // 0 = ни одна не нажата
};
```

Логика `press()`:
1. Если `_pressedButton != 0` и `_pressedButton != button` → `digitalWrite(BUTTON_PINS[_pressedButton-1], LOW)`
2. `digitalWrite(BUTTON_PINS[button-1], HIGH)`
3. `_pressedButton = button`

Логика `release()`:
1. Если `_pressedButton == button` → `digitalWrite(BUTTON_PINS[button-1], LOW)`, `_pressedButton = 0`

#### Интеграция в `main.cpp`
- Создать глобальный объект `ButtonController buttonController;`
- В `parseCommand()` добавить обработку команд `button_down` и `button_up`:
  - `button_down`: проверить `value` (1–7), вызвать `buttonController.press(value)`, отправить ответ.
  - `button_up`: проверить `value` (1–7), вызвать `buttonController.release(value)`, отправить ответ.

### SOLID
- **ButtonController** — Single Responsibility: только управление пинами кнопок.
- **PotentiometerController** — остаётся неизменным (отвечает только за потенциометры).
- **main.cpp** — только маршрутизация команд и бизнес-логика верхнего уровня.

---

## Порядок выполнения

1. Добавить `setVolumeMemory()` в PotentiometerController (.h + .cpp)
2. Добавить обработку `set_volume_memo` в `parseCommand()` (main.cpp)
3. Создать `ButtonController.h` и `ButtonController.cpp`
4. Интегрировать ButtonController в `main.cpp`:
   - `#include "ButtonController.h"`
   - Глобальный объект
   - Обработка `button_down` / `button_up` в `parseCommand()`
5. Проверить сборку и загрузку
