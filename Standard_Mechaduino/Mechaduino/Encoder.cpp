#include "Encoder.h"
#include <SPI.h>

int readEncoder() {
  uint8_t byte_1[8] = {0};
  uint8_t byte_2[8] = {0};

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
  SERCOM4->SPI.DATA.bit.DATA = 0x3F;
  byte a1 = SERCOM4->SPI.DATA.bit.DATA & 0xff;
  delayMicroseconds(1);
  SERCOM4->SPI.DATA.bit.DATA = 0xFF;
  byte a2 = SERCOM4->SPI.DATA.bit.DATA & 0xff;
  delayMicroseconds(1);
  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  for (int i = 0; i < 4; i++) {
    REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
    SERCOM4->SPI.DATA.bit.DATA = 0x3F;
    byte_1[i] = SERCOM4->SPI.DATA.bit.DATA & 0xff;
    delayMicroseconds(1);
    SERCOM4->SPI.DATA.bit.DATA = 0xFF;
    byte_2[i] = SERCOM4->SPI.DATA.bit.DATA & 0xff;
    delayMicroseconds(1);
    REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH
  }

  int sum = 0;

  for (int k = 0; k < 4; k++) {
    sum = sum + (((byte_1[k] << 8) | byte_2[k]) & ~(0xffffffff << 14));
  }


  return sum / 4;
}
