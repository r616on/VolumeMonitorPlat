# План: Режим запуска с минимальной громкостью (`start_min_value`)

## Цель
Добавить систему режимов, сохраняемую в EEPROM контроллера, которая управляет поведением при старте:
- **`standard`** (по умолчанию) — громкость при старте определяется значением, сохранённым в NV-памяти потенциометра MCP4561 (никакого принудительного вызова `setVolume`)
- **`start_min_value`** — при старте вызывается `setVolume(1)`, гарантируя минимальную громкость

Режим переключается командой `set_mode` из последовательного порта и сохраняется в EEPROM.

---

## Детали реализации

### 1. Новый модуль `ModeManager` (файлы `ModeManager.h` и `ModeManager.cpp`)

Следуя существующему паттерну проекта (`ButtonController`, `PresetDetector` — класс в паре `.h`/`.cpp`):

**`ModeManager.h`:**
```cpp
class ModeManager {
public:
    enum Mode : uint8_t {
        STANDARD = 0,
        START_MIN_VALUE = 1
    };

    ModeManager();

    void begin();                    // Читает режим из EEPROM, инициализирует STANDARD если данные невалидны
    Mode getMode() const;
    bool setMode(Mode mode);         // Сохраняет в EEPROM, возвращает true
    const char* getModeName() const; // "standard" / "start_min_value"
    static Mode modeFromName(const char* name); // "standard" -> STANDARD и т.д.

private:
    Mode _mode;

    // Адреса в EEPROM (ATmega328p — 1024 байта)
    static const int EEPROM_MAGIC_ADDR = 0;
    static const int EEPROM_MODE_ADDR  = 1;
    static const uint8_t EEPROM_MAGIC = 0xA5;
};
```

**`ModeManager.cpp`:**
- `begin()`: читает EEPROM_MAGIC_ADDR, сверяет с `EEPROM_MAGIC`; если совпало — читает `EEPROM_MODE_ADDR` в `_mode`; иначе — записывает magic, устанавливает `_mode = STANDARD` и сохраняет
- `setMode()`: записывает новое значение в EEPROM через `EEPROM.put()`, обновляет `_mode`
- `getModeName()`: возвращает строковое представление
- `modeFromName()`: парсит строку в enum, возвращает `STANDARD` при несовпадении

**Используемые библиотеки:** `<EEPROM.h>` (встроена в Arduino AVR framework, не требует добавления в `lib_deps`)

### 2. Изменения в `main.cpp`

#### 2a. Подключение модуля
```cpp
#include "ModeManager.h"
ModeManager modeManager;
```

#### 2b. Изменение `setup()`
Заменить безусловный вызов:
```cpp
// Было:
setVolume(1);

// Стало:
modeManager.begin();
if (modeManager.getMode() == ModeManager::START_MIN_VALUE) {
    setVolume(1);
}
```

#### 2c. Новая команда `set_mode`
В `parseCommand()` добавить ветку:
```cpp
else if (strcmp(command, "set_mode") == 0) {
    const char* modeValue = doc["value"] | "standard";
    ModeManager::Mode newMode = ModeManager::modeFromName(modeValue);
    modeManager.setMode(newMode);

    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "set_mode";
    responseDoc["mode"] = modeManager.getModeName();
    sendResponse(responseDoc);
}
```

#### 2d. Команда `get_mode` (опционально, для удобства)
```cpp
else if (strcmp(command, "get_mode") == 0) {
    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "get_mode";
    responseDoc["mode"] = modeManager.getModeName();
    sendResponse(responseDoc);
}
```

#### 2e. Добавить режим в ответ `ping`
```cpp
responseDoc["mode"] = modeManager.getModeName();
```

### 3. Формат JSON-команд

**Установка режима:**
```json
{"command":"set_mode","value":"start_min_value"}
```
Ответ:
```json
{"status":"success","command":"set_mode","mode":"start_min_value"}
```

**Получение текущего режима:**
```json
{"command":"get_mode"}
```
Ответ:
```json
{"status":"success","command":"get_mode","mode":"standard"}
```

### 4. Файлы для создания/изменения

| Файл | Действие | Описание |
|------|----------|----------|
| `src/ModeManager.h` | **Создать** | Заголовочный файл класса ModeManager |
| `src/ModeManager.cpp` | **Создать** | Реализация ModeManager с EEPROM-логикой |
| `src/main.cpp` | **Изменить** | Интеграция ModeManager, новые команды, условный `setVolume(1)` |

### 5. Принципы SOLID в реализации

| Принцип | Применение |
|---------|------------|
| **S** (Single Responsibility) | `ModeManager` отвечает только за хранение/чтение режима; `main.cpp` — за оркестрацию и парсинг команд |
| **O** (Open/Closed) | Добавление нового режима — только enum + имя в `getModeName()`/`modeFromName()`, без изменения внешнего API |
| **D** (Dependency Inversion) | `main.cpp` зависит от интерфейса `ModeManager`, а не от деталей работы с EEPROM |

### 6. Поведение при старте (итоговая логика `setup()`)

```
1. Инициализация Serial, Wire, пинов (без изменений)
2. Отправка welcome-сообщения (без изменений)
3. modeManager.begin() — чтение режима из EEPROM
4. Если режим == START_MIN_VALUE → setVolume(1)
   Если режим == STANDARD → ничего не делаем (MCP4561 сам загрузит значение из NV)
```

---

## Примечания
- EEPROM ATmega328p имеет ресурс ~100 000 циклов записи, что достаточно для редкой смены режима
- Magic-байт (0xA5) защищает от чтения «мусора» при первом включении или после сброса EEPROM
- План совместим с текущей кодовой базой: не ломает существующие команды, добавляет только новую функциональность
