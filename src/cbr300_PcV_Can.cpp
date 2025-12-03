/*
 * Cory J Fowler MCP_CAN 라이브러리용 예제
 *  - PCV 내부 CAN ID : 0x0C00110A (29bit Extended)
 *  - 데이터[0] = 태그 (0x1C = RPM, 0xC5 = TPS)
 *  - 데이터[1] = LSB, 데이터[2] = MSB
 */

#include <SPI.h>
#include <mcp_can.h>
#include "ShiftLight.h" // ShiftLight 모듈코드 포함

const uint8_t NEOPIXEL_PIN = 6; //LED 데이터핀
const uint8_t NEOPIXEL_COUNT = 8;   // LED 개수에 맞게

ShiftLight shiftLight(NEOPIXEL_PIN, NEOPIXEL_COUNT);


//-----------------------------
// 핀 설정
//-----------------------------
const int CAN_CS_PIN  = 10;   // MCP2515 CS
const int CAN_INT_PIN = 2;    // MCP2515 INT (옵션)

MCP_CAN CAN(CAN_CS_PIN);

// PCV 내부 버스 CAN ID (29bit)
const unsigned long PCV_CAN_ID = 0x0C00110A;

uint16_t rpm      = 0;
uint16_t tps_raw  = 0;
float    tps_pct  = 0.0;

void printValues()
{
  Serial.print("RPM: ");
  Serial.print(rpm);
  Serial.print("\tTPS: ");
  Serial.print(tps_pct, 1);
  Serial.println(" %");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial) {;}

  Serial.println("PCV CAN Sniffer (Cory J Fowler lib)");

  // 16MHz 크리스탈 기준 / PCV 버스가 500kbps라고 가정
  // 속도 다르면 CAN_250KBPS 등으로 바꿔야 함
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("CAN init OK");
  } else {
    Serial.println("CAN init FAIL");
    while (1);
  }

  CAN.setMode(MCP_NORMAL);  // 노멀 모드
  pinMode(CAN_INT_PIN, INPUT);

   shiftLight.begin();
  // 필요하면 임계값 조정
  // shiftLight.setThresholds(7000, 10500, 10000);
}

void loop()
{
  // 수신 프레임 있으면
  if (CAN_MSGAVAIL == CAN.checkReceive()) {

    unsigned long rxId;
    unsigned char len = 0;
    unsigned char buf[8];

    // *** Cory J Fowler 형식 ***
    //  readMsgBuf(&id, &len, buf)
    CAN.readMsgBuf(&rxId, &len, buf);

    // 29bit ID만 사용하므로, 하위 29비트만 비교
    if ( (rxId & 0x1FFFFFFFUL) == PCV_CAN_ID && len >= 3 ) {

      byte tag = buf[0];
      uint16_t raw = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);

      if (tag == 0x1C) {
        // RPM
        rpm = raw;
        shiftLight.update(rpm);   // 시프트라이트 갱신
        printValues();
      }
      else if (tag == 0xC5) {
        // TPS
        tps_raw = raw;
        tps_pct = tps_raw / 10.0;
        printValues();
      }
      
    }
  }
}