#include "Encoder.h"
#include <SPI.h>
#include "lookup_table.h"
#include "State.h"
#include "Configuration.h"

const int positionLPFa = (1000 * exp(positionLPF * -2 * 3.14159283 / FPID)); // z = e^st pole mapping
const int positionLPFb = (1000 - positionLPFa);

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

int readAngle(int last_angle, int last_raw) {

  int temp_angle;

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);

  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW
  byte hibyte = SPI.transfer(0xFF);
  byte lobyte = SPI.transfer(0xFF);
  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  int raw = pgm_read_word_near(lookup + (((hibyte << 8) | lobyte) & 0B0011111111111111));

  int raw_diff = raw - last_raw;

  if (raw_diff < -18000) {
    temp_angle = last_angle + 36000 + raw_diff;
  }
  else if (raw_diff > 18000) {
    temp_angle = last_angle - 36000 + raw_diff;
  }
  else {
    temp_angle = last_angle  + raw_diff;
  }


  return ((last_angle * positionLPFa) + (temp_angle  * positionLPFb)) / 1000;

}
