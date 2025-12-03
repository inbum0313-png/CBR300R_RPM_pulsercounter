#include <Arduino.h>// ECU → 아두이노 신호 핀
const int rpmPin = 2;   // 인터럽트 가능한 핀(UNO/Nano 기준)

volatile unsigned long pulseCount = 0;
unsigned long lastMillis = 0;

// 인터럽트 서비스 루틴
void rpmISR() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);

  pinMode(rpmPin, INPUT_PULLUP);  
  // ECU는 신호를 LOW로 당기므로 FALLING edge 사용
  attachInterrupt(digitalPinToInterrupt(rpmPin), rpmISR, FALLING);

  lastMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1초마다 RPM 계산
  if (currentMillis - lastMillis >= 1000) {

    // 단기통 CBR300R: 회전당 1펄스 가정 → RPM = Hz * 60
    unsigned long rpm = pulseCount * 60;

    Serial.print("RPM = ");
    Serial.println(rpm);

    pulseCount = 0;
    lastMillis = currentMillis;
  }
}
