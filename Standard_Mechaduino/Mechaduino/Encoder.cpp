#include "Encoder.h"
#include <SPI.h>
#include "lookup_table.h"
#include "State.h"


int readEncoder() {

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);

  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
  byte hibyte = SPI.transfer(0xFF);
  byte lobyte = SPI.transfer(0xFF);
  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  return (((hibyte << 8) | lobyte) & 0B0011111111111111);

}

#define samples 2
int readAngle(int last_angle, int last_raw) {
  int temp_angle[samples] = {0};


  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);

  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  for (byte i = 0; i < samples; i++) {
    REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
    byte hibyte = SPI.transfer(0xFF);
    byte lobyte = SPI.transfer(0xFF);
    REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

    int raw = pgm_read_word_near(lookup + (((hibyte << 8) | lobyte) & 0B0011111111111111));

    int raw_diff = raw - last_raw;

    if (raw_diff < -18000) {
      temp_angle[i] = last_angle + 36000 + raw_diff;
    }
    else if (raw_diff > 18000) {
      temp_angle[i] = last_angle - 36000 + raw_diff;
    }
    else {
      temp_angle[i] = last_angle  + raw_diff;
    }

  }

  int angle = 0;
  for (byte k = 0; k < samples; k++) {
    angle = angle + temp_angle[k];
  }

  return (last_angle * positionLPFa + (angle / samples) * positionLPFb) / 1000;
}
