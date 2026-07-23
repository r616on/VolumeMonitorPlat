# Volume Adapter

Аппаратный I2C/USART-мост управления автомобильным аудиопроцессором на базе Arduino Nano (ATmega328P). Подключается в разрыв аналогового тракта магнитолы, управляет громкостью и басом через цифровые потенциометры MCP4561, эмулирует нажатия кнопок через GPIO и детектирует активный пресет аудиопроцессора по импульсам оптопары. I2C, вход пресета, REM-выход и выход смены пресета гальванически развязаны (ISO1540DR + оптопары FOD817BW). GPIO-кнопки — прямые TTL-выходы.

**Платформа:** `atmelavr`, плата `nanoatmega328new`  
**Протокол:** [JUDI](docs/protocol.md) (JSON Unit Device Interface), 115200 бод  
**Питание:** USB +5V от головного устройства  
**Версия прошивки:** 1.0.1  
**Язык документации:** русский (`rules.md`)

---

## Документация

### Техническая (hardware & software)

| Документ | Содержание |
|---|---|
| [docs/hardware.md](docs/hardware.md) | Схема платы, BOM (14 компонентов), функциональные блоки (Mermaid), таблица цепей, распиновка коннекторов |
| [docs/architecture.md](docs/architecture.md) | Архитектура ПО: 4 класса + модуль свободных функций (SOLID), PresetControl (D9+U33), диаграммы, Known Issues, trade-off анализ |
| [docs/pinout.md](docs/pinout.md) | Назначение всех 22 выводов Arduino Nano (код + нетлист), расхождения ревизий |
| [docs/protocol.md](docs/protocol.md) | Протокол JUDI: формат сообщений, 11 команд с примерами JSON, коды ошибок |
| [docs/build.md](docs/build.md) | Сборка (PlatformIO), прошивка, структура проекта, зависимости |

### Проектная (product & requirements)

| Документ | Роль | Содержание |
|---|---|---|
| [docs/product-spec.md](docs/product-spec.md) | Product Manager | Ценностное предложение, Lean Canvas, пользовательские истории, анализ конкурентов, дорожная карта |
| [docs/system-requirements.md](docs/system-requirements.md) | Systems Engineer | 57 прослеживаемых требований (SYS-ELEC/IF/FUNC/EMC/ENV/REL), матрица верификации, применимые стандарты ISO/CISPR |
| [docs/test-plan.md](docs/test-plan.md) | Test & Validation Engineer | 50 тест-кейсов (L1–L5), методики испытаний, оборудование, шаблон протокола |

### Инженерная (process)

| Документ | Роль | Содержание |
|---|---|---|
| [docs/engineering-log.md](docs/engineering-log.md) | Lead Engineer | Хронология сессий, критическая оценка AI-решений, корректировки (D9, питание), открытые задачи |

---

## Структура проекта

```
.
├── platformio.ini              # PlatformIO: atmelavr, nanoatmega328new
├── README.md                   # Этот файл
├── rules.md                    # Конвенции: русский язык, SOLID
│
├── src/                        # Исходный код прошивки
│   ├── main.cpp                # Точка входа: setup(), loop(), парсинг JUDI
│   ├── PotentiometerController.{h,cpp}  # I2C → MCP4561 (Volume + Bass)
│   ├── PresetDetector.{h,cpp}  # Оптопара A3 → конечный автомат
│   ├── ButtonController.{h,cpp} # GPIO D3–D8, D10 → эмуляция кнопок
│   ├── ModeManager.{h,cpp}     # EEPROM → режимы STANDARD / START_MIN_VALUE
│   └── RemController.{h,cpp}   # A2 → REM-выход усилителя
│
├── docs/                       # Техническая и проектная документация
│   ├── hardware.md             # Схема, BOM, цепи, коннекторы
│   ├── architecture.md         # Архитектура ПО, Known Issues, trade-off
│   ├── pinout.md               # Распиновка Arduino (нетлист + код)
│   ├── protocol.md             # Протокол JUDI
│   ├── build.md                # Сборка и прошивка
│   ├── product-spec.md         # Продуктовая спецификация
│   ├── system-requirements.md  # Системные требования
│   ├── test-plan.md            # План испытаний и валидации
│   └── engineering-log.md      # Инженерный журнал
│
├── include/                    # Заголовочные файлы библиотек
├── lib/                        # Локальные библиотеки
└── test/                       # Тесты
```

---

## Быстрый старт

```bash
# Сборка
pio run

# Прошивка
pio run --target upload

# Монитор порта (115200 бод)
pio device monitor
```

**Проверка связи:**

```json
→ [{"command":"ping"}]
← [{"status":"success","command":"pong","device":"Volume_Adapter","volume":128,"bass":64,"preset":3,"is_enable_rem":true,"mode":"standard"}]
```

---

## Ключевые характеристики

| Параметр | Значение |
|---|---|
| MCU | ATmega328P (Arduino Nano) |
| Питание | USB +5V от ГУ |
| I2C | 100 кГц, гальванически развязан (ISO1540DR, 560V isolation) |
| Потенциометры | MCP4561 × 2 (5 кОм, 256 шагов, NV-память) |
| GPIO-кнопки | 7 выходов (5V TTL) |
| Выход пресета | D9 → U33 (FOD817BW) → CN1-4 (импульс +3.3V, 500 мс) |
| Вход пресета | A3 ← U2 (FOD817BW) ← CN1-5 (оптопара) |
| REM-выход | A2 → U3 (FOD817BW) → CN2 (реле усилителя) |
| UART | 115200 бод, 8N1, JSON-протокол JUDI |
| EEPROM | 2 байта: магическое число + режим |
| Зависимость | ArduinoJson ^7.2.2 |

---

## Известные проблемы (v1.0.1)

| # | Проблема | Приоритет |
|---|---|---|
| 1 | ODR-нарушение: `static int currentVolume` в `.cpp` vs `extern` в `.h` — AVR-GCC «прощает», но переносимость под угрозой | P1 |
| 2 | Welcome-сообщение отправляется до `ModeManager::begin()` — `mode` всегда `"standard"` на старте | P1 |
| 3 | Расхождение пинов `BUTTON_PINS` между нетлистом и кодом — разные ревизии платы | P1 |
| 4 | Не реализован Watchdog Timer — нет защиты от зависания | P0 |

---

## Дорожная карта

**v1.1 (исправления):** WDT, ODR-fix, welcome после ModeManager, сверка BUTTON_PINS.

**v1.2 (расширение протокола):** асинхронные события, баланс/фейдер (требует доп. MCP4561), CRC-защита сообщений.

**v2.0 (следующее поколение):** STM32F042 (Cortex-M0 + CAN), Bluetooth, корпус, CAN-транспорт для JUDI.
