# Протокол JUDI

Volume Adapter использует текстовый протокол JUDI (JSON Unified Device Interface). Все сообщения передаются через Serial (UART, 115200 бод, 8N1).

---

## Формат сообщений

```
[JSON]\n
```

Каждое сообщение — JSON-объект внутри квадратных скобок, завершающийся символом новой строки (`\n`). Максимальная длина JSON — 256 байт (`StaticJsonDocument<256>`).

---

## Welcome-сообщение

При старте устройство отправляет:

```json
[{"status":"ready","device":"Volume Adapter","protocol":"JUDI","mode":"standard","volume_range":"0-255","bass_range":"0-255","version":"1.0.1"}]
```

| Поле | Описание |
|---|---|
| `status` | `"ready"` |
| `device` | `"Volume Adapter"` (welcome) / `"Volume_Adapter"` (ping) |
| `protocol` | `"JUDI"` |
| `mode` | Текущий режим (всегда `"standard"` на старте — см. Known Issue в [architecture.md](architecture.md)) |
| `volume_range` | `"0-255"` |
| `bass_range` | `"0-255"` |
| `version` | Версия прошивки (`"1.0.1"`) |

---

## Команды

### Управление громкостью и басом

#### `set_volume` — установка громкости (только RAM)

Сбрасывается при выключении питания.

```json
→ [{"command":"set_volume","value":128}]
← [{"status":"success","command":"set_volume","volume":128}]
```

Ошибка значения:

```json
← [{"status":"error","message":"value must be 0-255","received":300}]
```

Ошибка I2C:

```json
← [{"status":"error","message":"I2C communication failed (volume)"}]
```

---

#### `set_volume_memo` — установка громкости с сохранением (RAM + NV)

Значение сохраняется в энергонезависимую память MCP4561 и восстанавливается после выключения питания.

```json
→ [{"command":"set_volume_memo","value":128}]
← [{"status":"success","command":"set_volume_memo","volume":128}]
```

Ошибка значения:

```json
← [{"status":"error","message":"value must be 0-255","received":300}]
```

Ошибка I2C:

```json
← [{"status":"error","message":"I2C communication failed (volume memo)"}]
```

---

#### `set_bass_level` — установка уровня баса (RAM + NV)

Значение сохраняется в NV-память.

```json
→ [{"command":"set_bass_level","value":100}]
← [{"status":"success","command":"set_bass_level","value":100}]
```

Ошибка значения:

```json
← [{"status":"error","message":"value must be 0-255","received":300}]
```

Ошибка I2C:

```json
← [{"status":"error","message":"I2C communication failed (value)"}]
```

---

### Управление режимом

#### `set_mode` — переключение режима (сохраняется в EEPROM)

```json
→ [{"command":"set_mode","value":"start_min_value"}]
← [{"status":"success","command":"set_mode","mode":"start_min_value"}]
```

Допустимые значения: `"standard"`, `"start_min_value"`. Любое другое значение трактуется как `"standard"`.

---

#### `get_mode` — запрос текущего режима

```json
→ [{"command":"get_mode"}]
← [{"status":"success","command":"get_mode","mode":"standard"}]
```

---

### Управление пресетами

#### `change_preset` — эмуляция нажатия кнопки смены пресета

Устанавливает D9 в HIGH на 500 мс (импульс).

```json
→ [{"command":"change_preset"}]
← [{"status":"success","command":"change_preset","message":"Button press simulated (500ms)"}]
```

---

#### `get_preset` — получение последнего подтверждённого пресета

```json
→ [{"command":"get_preset"}]
← [{"command":"preset_changed","value":3}]
```

Возвращает `0`, если пресет не подтверждён. Подтверждение требует **двух последовательных серий с одинаковым числом импульсов** (см. [architecture.md](architecture.md), PresetDetector).

---

### Управление REM

#### `set_is_enable_rem` — включение/выключение REM-выхода

Управляет пином A2 (HIGH = REM вкл, LOW = REM выкл).

```json
→ [{"command":"set_is_enable_rem","value":true}]
← [{"status":"success","command":"set_is_enable_rem","is_enable_rem":true}]
```

---

### Эмуляция кнопок

#### `button_down` — нажатие кнопки (1–7)

```json
→ [{"command":"button_down","value":3}]
← [{"status":"success","command":"button_down","value":3}]
```

Ошибка диапазона:

```json
← [{"status":"error","message":"button value must be 1-7","received":9}]
```

---

#### `button_up` — отпускание кнопки (1–7)

```json
→ [{"command":"button_up","value":3}]
← [{"status":"success","command":"button_up","value":3}]
```

Ошибка диапазона:

```json
← [{"status":"error","message":"button value must be 1-7","received":9}]
```

---

### Служебные

#### `ping` — проверка связи и состояние устройства

```json
→ [{"command":"ping"}]
← [{"status":"success","command":"pong","device":"Volume_Adapter","volume":128,"volume_range":"0-255","bass":100,"bass_range":"0-255","preset":3,"is_enable_rem":true,"mode":"standard"}]
```

| Поле | Тип | Описание |
|---|---|---|
| `status` | string | `"success"` |
| `command` | string | `"pong"` |
| `device` | string | `"Volume_Adapter"` |
| `volume` | int | Текущее значение громкости (−1 если не установлено) |
| `volume_range` | string | `"0-255"` |
| `bass` | int | Текущее значение баса (−1 если не установлено) |
| `bass_range` | string | `"0-255"` |
| `preset` | int | Последний подтверждённый пресет (0 если нет) |
| `is_enable_rem` | bool | Состояние REM-выхода |
| `mode` | string | Текущий режим (`"standard"` / `"start_min_value"`) |

---

## Коды ошибок

### Ошибка валидации JSON

```json
[{"status":"error","message":"Invalid JSON","detail":"...описание ошибки парсинга..."}]
```

`detail` содержит строку от `DeserializationError::c_str()` (библиотека ArduinoJson).

### Ошибка формата сообщения

Если сообщение не заключено в квадратные скобки `[...]`:

```json
[{"status":"error","message":"Message must be enclosed in [ ]"}]
```

### Ошибка отсутствия команды

Если поле `command` отсутствует или пустое:

```json
[{"status":"error","message":"Missing or empty 'command' field"}]
```

### Ошибка валидации значения

```json
[{"status":"error","message":"value must be 0-255","received":300}]
[{"status":"error","message":"button value must be 1-7","received":9}]
```

### Ошибка неизвестной команды

```json
[{"status":"error","message":"Unknown command","received_command":"some_unknown"}]
```

### Ошибка I2C

```json
[{"status":"error","message":"I2C communication failed (volume)"}]
[{"status":"error","message":"I2C communication failed (volume memo)"}]
[{"status":"error","message":"I2C communication failed (value)"}]
```

Возникает при получении NACK от MCP4561 (устройство не ответило на шине I2C).
