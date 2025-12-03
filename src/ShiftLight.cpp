// ShiftLight.cpp
#include "ShiftLight.h"

ShiftLight::ShiftLight(uint8_t pin, uint8_t numLeds)
  : strip(numLeds, pin, NEO_GRB + NEO_KHZ800),
    _numLeds(numLeds),
    _startRpm(1000),    // 기본값: 7000rpm부터 서서히 점등
    _fullRpm(10500),    // 10500rpm에서 풀 바
    _flashRpm(10000),   // 10000rpm 이상에서 플래시
    _flashState(false),
    _lastFlashToggle(0),
    _flashIntervalMs(100)   // 플래시 주기 100ms
{
}

void ShiftLight::begin() {
  strip.begin();
  strip.show();  // 모두 끄기
}

void ShiftLight::setThresholds(uint16_t startRpm, uint16_t fullRpm, uint16_t flashRpm) {
  _startRpm = startRpm;
  _fullRpm  = fullRpm;
  _flashRpm = flashRpm;
}

void ShiftLight::update(uint16_t rpm) {
  // 시프트 시작 이전 → 꺼짐
  if (rpm < _startRpm) {
    showOff();
    return;
  }

  // 플래시 구간
  if (rpm >= _flashRpm) {
    unsigned long now = millis();
    if (now - _lastFlashToggle >= _flashIntervalMs) {
      _lastFlashToggle = now;
      _flashState = !_flashState;
    }

    if (_flashState) {
      // 빨간색 풀 바
      for (uint8_t i = 0; i < _numLeds; i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
      }
    } else {
      showOff();
    }
    strip.show();
    return;
  }

  // startRpm ~ fullRpm 구간 → 레벨바
  if (rpm >= _startRpm && rpm < _fullRpm) {
    long level = map((long)rpm, (long)_startRpm, (long)_fullRpm, 1, (long)_numLeds);
    if (level < 1) level = 1;
    if (level > _numLeds) level = _numLeds;
    showBar((uint8_t)level);
    return;
  }

  // fullRpm 이상, flashRpm 미만 → 풀바 고정 (노란색 → 빨강 중간 느낌)
  if (rpm >= _fullRpm && rpm < _flashRpm) {
    for (uint8_t i = 0; i < _numLeds; i++) {
      strip.setPixelColor(i, strip.Color(255, 80, 0));  // 주황~붉은색
    }
    strip.show();
  }
}

void ShiftLight::showBar(uint8_t level) {
  // 앞쪽부터 level개 켜기 (초록→노랑→주황 느낌)
  for (uint8_t i = 0; i < _numLeds; i++) {
    if (i < level) {
      // 위치에 따라 색 살짝 바꾸기
      float ratio = (float)i / (_numLeds - 1);
      uint8_t r = (uint8_t)(255 * ratio);          // 0 ~ 255
      uint8_t g = (uint8_t)(255 * (1.0 - ratio));  // 255 ~ 0
      uint8_t b = 0;
      strip.setPixelColor(i, strip.Color(r, g, b));
    } else {
      strip.setPixelColor(i, 0);   // off
    }
  }
  strip.show();
}

void ShiftLight::showOff() {
  for (uint8_t i = 0; i < _numLeds; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}
