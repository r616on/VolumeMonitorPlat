# План: Исправить износ EEPROM в setBassLevel

## Проблема

`setBassLevel()` вызывает `setPotValueMemory()` (RAM + NV/EEPROM), в то время как `setVolume()` вызывает `setPotValue()` (только RAM). Это несимметрично и вызывает цикл записи EEPROM (~4ms) при каждом изменении баса, изнашивая NV-память MCP4561.

## Исправление

### Файл: `src/PotentiometerController.cpp`

Заменить в `setBassLevel()` вызов `setPotValueMemory` на `setPotValue`:

```cpp
// Было (строка 45):
bool setBassLevel(int value) {
  bool success = setPotValueMemory(MCP4561_ADDR_BASS, value);
  if (success) currentBass = value;
  return success;
}

// Стало:
bool setBassLevel(int value) {
  bool success = setPotValue(MCP4561_ADDR_BASS, value);
  if (success) currentBass = value;
  return success;
}
```

### После исправления

| Функция | Пишет в |
|---------|---------|
| `setVolume()` | RAM |
| `setBassLevel()` | RAM |
| `setVolumeMemory()` | RAM + NV |
| `setPotValueMemory()` | RAM + NV (низкоуровневая) |

`setVolume` и `setBassLevel` теперь симметричны — обе пишут только в RAM.
Для сохранения в EEPROM доступна отдельная функция `setVolumeMemory()` (громкость). При необходимости можно добавить `setBassMemory()`.

## Проверка

После изменения — `pio run` должен собраться чисто (0 errors, 0 warnings).
