#include "Encoder.h"
#include <SPI.h>

int readEncoder() {
  uint8_t hibyte[8] = {0};
  uint8_t lobyte[8] = {0};

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW

  SERCOM4->SPI.DATA.bit.DATA = 0xFF;
  SERCOM4->SPI.DATA.bit.DATA & 0xff;
  delayMicroseconds(1);
  SERCOM4->SPI.DATA.bit.DATA = 0xFF;
  SERCOM4->SPI.DATA.bit.DATA & 0xff;
  delayMicroseconds(1);

  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  for (int i = 0; i < 4; i++) {
    REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW

    SERCOM4->SPI.DATA.bit.DATA = 0xFF;
    hibyte[i] = SERCOM4->SPI.DATA.bit.DATA & 0xff;
    delayMicroseconds(1);
    SERCOM4->SPI.DATA.bit.DATA = 0xFF;
    lobyte[i] = SERCOM4->SPI.DATA.bit.DATA & 0xff;
    delayMicroseconds(1);

    REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH
  }

  int sum = 0;
  for (int k = 0; k < 4; k++) {
    sum = sum + (((hibyte[k] << 8) | lobyte[k]) & ~(0xffffffff << 14));
  }

  return sum / 4;
}
