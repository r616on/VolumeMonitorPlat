# Volume Adapter

Аппаратный I2C/USART-мост для управления аудиосистемой на базе Arduino Nano (ATmega328P). Принимает JSON-команды через Serial, управляет цифровыми потенциометрами MCP4561 по I2C, эмулирует нажатия кнопок магнитолы через GPIO и детектирует смену пресетов по импульсам оптопары.

**Платформа:** `atmelavr`, плата `nanoatmega328new`  
**Протокол:** [JUDI](docs/protocol.md) (JSON Unit Device Interface), 115200 бод  
**Версия прошивки:** 1.0.1  
**Язык документации:** русский (согласно `rules.md`)

---

## Документация

| Документ | Содержание |
|---|---|
| [docs/hardware.md](docs/hardware.md) | Схема платы, BOM, функциональные блоки (Mermaid), таблица цепей, распиновка коннекторов |
| [docs/architecture.md](docs/architecture.md) | Архитектура ПО: диаграмма классов, логика работы, порядок инициализации, Known Issues |
| [docs/protocol.md](docs/protocol.md) | Протокол JUDI: формат сообщений, все команды с примерами JSON, коды ошибок |
| [docs/pinout.md](docs/pinout.md) | Назначение всех выводов Arduino Nano (код + схема) |
| [docs/build.md](docs/build.md) | Сборка, прошивка, структура проекта, зависимости |
