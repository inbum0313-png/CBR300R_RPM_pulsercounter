// ShiftLight.h
#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class ShiftLight {
public:
  // pin  : 네오픽셀 데이터 핀
  // numLeds : LED 개수
  ShiftLight(uint8_t pin, uint8_t numLeds);

  // 네오픽셀 초기화 (setup()에서 1번 호출)
  void begin();

  // rpm 값을 넣어주면 내부에서 상태 업데이트
  void update(uint16_t rpm);

  // 시프트라이트 구간 설정
  //  startRpm : 이 rpm 부터 LED가 켜지기 시작
  //  fullRpm  : 이 rpm 에서 모든 LED가 켜짐
  //  flashRpm : 이 rpm 이상에서 전체가 점멸(빨간색 플래시)
  void setThresholds(uint16_t startRpm, uint16_t fullRpm, uint16_t flashRpm);

private:
  Adafruit_NeoPixel strip;
  uint8_t  _numLeds;
  uint16_t _startRpm;
  uint16_t _fullRpm;
  uint16_t _flashRpm;

  bool     _flashState;
  unsigned long _lastFlashToggle;
  uint16_t _flashIntervalMs;

  void showBar(uint8_t level);     // 0 ~ numLeds
  void showOff();
};
