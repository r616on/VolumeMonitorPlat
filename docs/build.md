# Сборка и прошивка

Проект собирается с помощью [PlatformIO](https://platformio.org/).

---

## Конфигурация (`platformio.ini`)

| Параметр | Значение |
|---|---|
| Платформа | `atmelavr` |
| Плата | `nanoatmega328new` |
| Фреймворк | `arduino` |
| Скорость монитора | 115200 |
| Зависимость | `bblanchon/ArduinoJson@^7.2.2` |
| Build flags | `-Wno-deprecated-declarations` |

Окружение: `[env:nanoatmega328]`.

---

## Команды

```bash
# Сборка проекта
pio run

# Сборка и прошивка
pio run --target upload

# Монитор последовательного порта
pio device monitor

# Очистка сборочных артефактов
pio run --target clean
```

Для прошивки плата должна быть подключена через USB. Порт определяется автоматически. При необходимости указать порт явно — добавить `--upload-port COMx` (Windows) или `--upload-port /dev/ttyUSB0` (Linux).

---

## Структура проекта

```
.
├── platformio.ini              # Конфигурация PlatformIO
├── README.md                   # Индекс документации
├── rules.md                    # Правила оформления (русский язык, SOLID)
├── docs/                       # Техническая документация
│   ├── hardware.md             # Аппаратная конфигурация (схема, BOM, нетлист)
│   ├── architecture.md         # Архитектура ПО (классы, взаимодействие)
│   ├── protocol.md             # Протокол JUDI (JSON-команды)
│   ├── pinout.md               # Назначение выводов Arduino
│   └── build.md                # Сборка и прошивка (этот файл)
├── src/                        # Исходный код
│   ├── main.cpp                # Точка входа (setup, loop, парсинг команд)
│   ├── PotentiometerController.h
│   ├── PotentiometerController.cpp
│   ├── PresetDetector.h
│   ├── PresetDetector.cpp
│   ├── ButtonController.h
│   ├── ButtonController.cpp
│   ├── ModeManager.h
│   ├── ModeManager.cpp
│   ├── RemController.h
│   └── RemController.cpp
├── include/                    # Заголовочные файлы библиотек
├── lib/                        # Локальные библиотеки
└── test/                       # Тесты
```

---

## Зависимости

| Библиотека | Версия | Назначение |
|---|---|---|
| `ArduinoJson` | ^7.2.2 | Парсинг и сериализация JSON-сообщений протокола JUDI |
| `Wire` | Встроенная | Управление шиной I2C (MCP4561) |
| `EEPROM` | Встроенная | Чтение/запись энергонезависимой памяти (режимы ModeManager) |

---

## Прошивка

1. Установить [PlatformIO IDE](https://platformio.org/install) (расширение VS Code) или [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html) (CLI)
2. Подключить Arduino Nano через USB
3. Выполнить:
   ```bash
   pio run --target upload
   ```
4. Проверить связь через монитор порта (115200 бод):
   ```bash
   pio device monitor
   ```
   Отправить тестовую команду:
   ```json
   [{"command":"ping"}]
   ```
   Ожидаемый ответ:
   ```json
   [{"status":"success","command":"pong","device":"Volume_Adapter",...}]
   ```
