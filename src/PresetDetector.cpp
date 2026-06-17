#include "PresetDetector.h"

PresetDetector::PresetDetector(uint8_t pin)
  : _pin(pin),
    _minPulseDur(100),
    _maxPulseDur(200),
    _seriesTimeout(400),
    _state(IDLE),
    _pulseActive(false),
    _pulseStart(0),
    _lastPulseEnd(0),
    _debounceTime(5),
    _lastRawReading(HIGH),
    _lastReadChange(0),
    _stableLevel(HIGH),
    _stableLevelValid(false),
    _currentSeriesCount(0),
    _confirmedPreset(0) {
  pinMode(_pin, INPUT_PULLUP);
}

void PresetDetector::update() {
  unsigned long now = millis();
  int curRaw = digitalRead(_pin);

  if (curRaw != _lastRawReading) {
    _lastRawReading = curRaw;
    _lastReadChange = now;
  }

  if (now - _lastReadChange < _debounceTime) {
    return;
  }

  int stable = _lastRawReading;

  if (!_stableLevelValid) {
    _stableLevel = stable;
    _stableLevelValid = true;
    return;
  }

  if (stable == _stableLevel) {
    return;
  }

  int curLevel = stable;
  _stableLevel = stable;

  if (curLevel == LOW) {
    _pulseActive = true;
    _pulseStart = now;
  } else {
    if (_pulseActive) {
      unsigned long duration = now - _pulseStart;

      if (duration >= _minPulseDur && duration <= _maxPulseDur) {
        if (_state == IDLE) {
          _state = IN_SERIES;
          _currentSeriesCount = 1;
        } else {
          _currentSeriesCount++;
        }
        _lastPulseEnd = now;
      } else {
        _state = IDLE;
        _currentSeriesCount = 0;
      }
      _pulseActive = false;
    }
  }

  if (_state == IN_SERIES && !_pulseActive) {
    unsigned long timeSinceLastPulse = now - _lastPulseEnd;
    if (timeSinceLastPulse > _seriesTimeout) {
      _endSeries();
    }
  }
}

void PresetDetector::_endSeries() {
  // Подтверждаем пресет сразу (если количество импульсов в разумных пределах)
  if (_currentSeriesCount >= 1 && _currentSeriesCount <= 10) {
    _confirmedPreset = _currentSeriesCount;
  }

  // Сбрасываем состояние для следующей серии
  _state = IDLE;
  _currentSeriesCount = 0;
}

int PresetDetector::getConfirmedPreset() const {
  return _confirmedPreset;
}

void PresetDetector::reset() {
  _state = IDLE;
  _currentSeriesCount = 0;
  _confirmedPreset = 0;
  _pulseActive = false;
  _stableLevelValid = false;
}

void PresetDetector::setPulseRange(unsigned long minDur, unsigned long maxDur) {
  _minPulseDur = minDur;
  _maxPulseDur = maxDur;
}

void PresetDetector::setTimeout(unsigned long seriesTimeout) {
  _seriesTimeout = seriesTimeout;
}

void PresetDetector::setDebounce(unsigned long ms) {
  _debounceTime = ms;
}
