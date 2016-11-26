//Contains the definitions of the functions used by the firmware.
#include <SPI.h>

#include "Utils.h"
#include "Parameters.h"
#include "State.h"
#include "Controller.h"
#include "analogFastWrite.h"

void setupPins() {
  analogWriteResolution(10);  //set the PWM resolution from 8 bit to to 10 bit

  pinMode(VREF_2, OUTPUT);
  pinMode(VREF_1, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(step_pin, INPUT);
  pinMode(dir_pin, INPUT);
  pinMode(chipSelectPin, OUTPUT); // CSn -- has to toggle high and low to signal chip to start data transfer

  attachInterrupt(step_pin, stepInterrupt, RISING);
  attachInterrupt(dir_pin, dirInterrupt, CHANGE);

  if (use_enable_pin == true) {
    pinMode(ena_pin, INPUT_PULLUP);
    attachInterrupt(ena_pin, enaInterrupt, CHANGE);
  }

  digitalWriteDirect(IN_4, true);
  digitalWriteDirect(IN_3, false);
  digitalWriteDirect(IN_2, true);
  digitalWriteDirect(IN_1, false);
}


void setupSPI() {
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE1);             ///400000, MSBFIRST, SPI_MODE1);

  SPI.begin();    //AS5047D SPI uses mode=1 (CPOL=0, CPHA=1)
  delay(1000);
  SPI.beginTransaction(settingsA);
}


void stepInterrupt() {
  if (dir) {
    step_target += 1;
  }
  else {
    step_target -= 1;
  }
}


void dirInterrupt() {
  if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
    dir = false;
  }
  else {
    dir = true;
  }
}


void enaInterrupt() {
  if (REG_PORT_IN0 & PORT_PA14) { // check if ena_pin is HIGH
    enabled = false;
  }
  else {
    enabled = true;
  }
}


void output(int theta, int effort) {
  static int v_coil_A;
  static int v_coil_B;
  
  static int phase_multiplier = (10 * steps_per_revolution / 4) / 100;
  
  int angle = mod((phase_multiplier * theta) , 3600);
  
  int sin_coil_A = pgm_read_word_near(sin_lookup + angle);
  if (sin_coil_A > 1024) {
    sin_coil_A = sin_coil_A - 65536;
  }

  int sin_coil_B = pgm_read_word_near(cos_lookup + angle);
  if (sin_coil_B > 1024) {
    sin_coil_B = sin_coil_B - 65536;
  }

  if (calibration_running) {
    v_coil_A = ((effort * sin_coil_A) / 1024);
    v_coil_B = ((effort * sin_coil_B) / 1024);
  }
  else {
    v_coil_A = ((2 * v_coil_A) + ((effort * sin_coil_A) / 1024)) / 3;
    v_coil_B = ((2 * v_coil_B) + ((effort * sin_coil_B) / 1024)) / 3;
  }

  analogFastWrite(VREF_1, abs(v_coil_A));
  analogFastWrite(VREF_2, abs(v_coil_B));

  if (v_coil_A >= 0)  {
    REG_PORT_OUTSET0 = PORT_PA21;     //write IN_2 HIGH
    REG_PORT_OUTCLR0 = PORT_PA06;     //write IN_1 LOW
  }
  else  {
    REG_PORT_OUTCLR0 = PORT_PA21;     //write IN_2 LOW
    REG_PORT_OUTSET0 = PORT_PA06;     //write IN_1 HIGH
  }

  if (v_coil_B >= 0)  {
    REG_PORT_OUTSET0 = PORT_PA20;     //write IN_4 HIGH
    REG_PORT_OUTCLR0 = PORT_PA15;     //write IN_3 LOW
  }
  else  {
    REG_PORT_OUTCLR0 = PORT_PA20;     //write IN_4 LOW
    REG_PORT_OUTSET0 = PORT_PA15;     //write IN_3 HIGH
  }
}


void calibration() {
  calibration_running = true;
  disableTC5Interrupts();

  SerialUSB.println(" ");
  SerialUSB.println("---- Calibration Routine ----");
  SerialUSB.println(" ");

  int encoderReading = 0;
  int lastencoderReading = 0;

  int avg = 20;         //how many readings to average

  int iStart = 0;
  int jStart = 0;
  int stepNo = 0;
  //step_target = 0;

  int fullStepReadings[steps_per_revolution];
  int ticks = 0;

  float lookupAngle = 0.0;

  dir = true;
  output(0, 64);

  for (int reading = 0; reading < avg; reading++) {
    lastencoderReading += readEncoder();
    delay(5);
  }
  lastencoderReading = lastencoderReading / avg;

  output(PA, 64);
  output(2 * PA, 64);
  output(3 * PA, 64);

  delay(500);

  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delay(5);
  }
  encoderReading = encoderReading / avg;

  if ((encoderReading - lastencoderReading) < 0)
  {
    SerialUSB.println("Wired backwards");
    return;
  }

  //jump to fullstep
  output(0, 64);

  delay(500);

  SerialUSB.println("Calibrating single steps");
  SerialUSB.println("0%      20%       40%       60%        80%       100%");
  SerialUSB.println("|--------------------------------------------------|");
  SerialUSB.print("|");

  // step to every single fullstep position and read the Encoder
  step_target = 0;

  for (int x = 0; x < steps_per_revolution; x++) {

    //output((x * PA), 64);

    delay(100);

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(5);
    }
    encoderReading = encoderReading / avg;


    fullStepReadings[x] = encoderReading;

    oneStep();

    if (((50 * x) % steps_per_revolution) == 0) {
      SerialUSB.print("-");
    }
  }

  SerialUSB.println("|");
  SerialUSB.println();
  SerialUSB.println("done!");
  SerialUSB.println();
  // end fullsteps


  // interpolate between the fullsteps
  for (int i = 0; i < steps_per_revolution; i++) {
    ticks = fullStepReadings[mod((i + 1), steps_per_revolution)] - fullStepReadings[mod((i), steps_per_revolution)];
    if (ticks < -15000) {
      ticks += counts_per_revolution;
    }
    else if (ticks > 15000) {
      ticks -= counts_per_revolution;
    }

    if (ticks > 1) {
      for (int j = 0; j < ticks; j++) {
        stepNo = (mod(fullStepReadings[i] + j, counts_per_revolution));
        if (stepNo == 0) {
          iStart = i;
          jStart = j;
        }
      }
    }

    if (ticks < 1) {
      for (int j = -ticks; j > 0; j--) {
        stepNo = (mod(fullStepReadings[steps_per_revolution - 1 - i] + j, counts_per_revolution));
        if (stepNo == 0) {
          iStart = i;
          jStart = j;
        }

      }
    }
  }


  SerialUSB.println("Generating lookuptable");
  SerialUSB.print("const PROGMEM int lookup[] = {");

  for (int i = iStart; i < (iStart + steps_per_revolution + 1); i++) {
    ticks = fullStepReadings[mod((i + 1), steps_per_revolution)] - fullStepReadings[mod((i), steps_per_revolution)];

    if (ticks < -15000) {
      ticks += counts_per_revolution;

    }
    else if (ticks > 15000) {
      ticks -= counts_per_revolution;
    }

    if (ticks > 1) {

      if (i == iStart) {
        for (int j = jStart; j < ticks; j++) {
          lookupAngle = 0.1 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }

      else if (i == (iStart + steps_per_revolution)) {
        for (int j = 0; j < jStart; j++) {
          lookupAngle = 0.1 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }
      else {
        for (int j = 0; j < ticks; j++) {
          lookupAngle = 0.1 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }

    }

    else if (ticks < 1) {
      if (i == iStart) {
        for (int j = - ticks; j > (jStart); j--) {
          lookupAngle = 0.1 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }
      else if (i == iStart + steps_per_revolution) {
        for (int j = jStart; j > 0; j--) {
          lookupAngle = 0.1 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }
      else {
        for (int j = - ticks; j > 0; j--) {
          lookupAngle = 0.1 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }

    }
  }
  SerialUSB.println();
  SerialUSB.println("};");
  SerialUSB.println();

  calibration_running = false;
  enableTC5Interrupts();
}


void serialCheck() {
  if (SerialUSB.peek() != -1) {

    char inChar = (char)SerialUSB.read();

    switch (inChar) {
      case 'c':
        calibration();           //cal routine
        break;

      case 's':             //new setpoint
        setpoint();
        break;

      case 'p':
        parameterQuery();     // prints copy-able parameters
        break;

      case 'e':
        parameterEdit();
        break;

      case 'j':
        step_response();
        break;

      case 'm':
        Serial_menu();
        break;

      case 'f':
        get_max_frequency();
        break;

      case 't':
        PID_autotune();
        break;

      default:
        break;
    }
  }

}


void Serial_menu() {
  SerialUSB.println("");
  SerialUSB.println("");
  SerialUSB.println("----- Mechaduino 0.1 -----");
  SerialUSB.print("Identifier: ");
  SerialUSB.println(identifier);
  SerialUSB.println("");
  SerialUSB.println("");
  SerialUSB.println("Main menu");
  SerialUSB.println("c  -  calibration");
  SerialUSB.println("s  -  setpoint");
  SerialUSB.println("p  -  print parameter");
  SerialUSB.println("e  -  edit parameter ");
  SerialUSB.println("j  -  step response");
  SerialUSB.println("m  -  print main menu");
  SerialUSB.println("f  -  get max loop frequency");
  SerialUSB.println("t  -  PID autotune");
  SerialUSB.println("");
}


void setpoint() {
  unsigned long start_millis;
  start_millis = millis();
  int time_out = 5000;
  int new_angle = 0;
  bool received = false;

  SerialUSB.println("Enter steps that should be jumped!");

  SerialUSB.print("current Setpoint: ");
  SerialUSB.println((y / 100.0));
  SerialUSB.println("Enter setpoint:");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      new_angle = 100 * SerialUSB.parseFloat();

      step_target = step_target + ( (new_angle - y) / stepangle);

      received = true;
    }
    else if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      new_angle = r * stepangle;
      return;
    }
  }

  SerialUSB.print("new Setpoint: ");
  SerialUSB.println((new_angle / 100.0));
}


void parameterQuery() {
  SerialUSB.println(' ');
  SerialUSB.println("//---- PID Values -----");

  SerialUSB.print("volatile int Kp = ");
  SerialUSB.print(Kp);
  SerialUSB.println(';');

  SerialUSB.print("volatile int Ki = ");
  SerialUSB.print(Ki);
  SerialUSB.println(';');

  SerialUSB.print("volatile int Kd = ");
  SerialUSB.print(Kd);
  SerialUSB.println(';');
}


void oneStep() {
  if (dir == 0) {
    step_target += PA;
  }
  else {
    step_target -= PA;
  }

  output(step_target, 255);
}


int readEncoder()
{
  int angleTemp;

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW

  //read with Measured angle with dynamic angle error compensation (DAEC)
  byte b1 = SPI.transfer(0xFF);
  byte b2 = SPI.transfer(0xFF);

  //read with Measured angle without dynamic angle error compensation (DAEC)
  //byte b1 = SPI.transfer(0x7F);
  //byte b2 = SPI.transfer(0xFE);

  angleTemp = (((b1 << 8) | b2) & 0B0011111111111111);

  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  return angleTemp;
}


int mod(int xMod, int mMod) {
  return (xMod % mMod + mMod) % mMod;
}


void setupTCInterrupts() {
  const int overflow_TC_5 = (48000000 / FPID) - 1;
  //const int overflow_TC_4 = (48000000 / FSAMPLE) - 1;

  // Enable GCLK for TC4 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY);

  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;   // Set Timer counter Mode to 16 bits
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal Normal Frq
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;   // Set perscaler
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.CC[0].reg = overflow_TC_5;
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC5->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC5->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0

  NVIC_SetPriority(TC5_IRQn, 1);

  // Enable InterruptVector
  NVIC_EnableIRQ(TC5_IRQn);
}


void enableTC5Interrupts() {
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
}


void disableTC5Interrupts() {
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync
}


void parameterEdit() {
  bool received_1 = false;
  bool received_2 = false;
  unsigned long start_millis = millis();
  int time_out = 5000;

  SerialUSB.println();
  SerialUSB.println("---- Edit position loop gains: ----");
  SerialUSB.println();
  SerialUSB.print("p ----- Kp = ");
  SerialUSB.println(Kp);

  SerialUSB.print("i ----- Ki = ");
  SerialUSB.println(Ki);

  SerialUSB.print("d ----- Kd = ");
  SerialUSB.println(Kd);

  SerialUSB.println("q ----- quit");
  SerialUSB.println();


  while (!received_1)  {
    delay(100);
    char inChar2 = (char)SerialUSB.read();

    if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }

    switch (inChar2) {
      case 'p':
        {
          SerialUSB.println("enter new Kp!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              Kp = SerialUSB.parseInt();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'i':
        {
          SerialUSB.println("enter new Ki!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              Ki = SerialUSB.parseInt();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'd':
        {
          SerialUSB.println("enter new Kd!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              Kd = SerialUSB.parseInt();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'q':
        {
          SerialUSB.println("quit");
          received_1 = true;
        }
        break;
    }
  }

  SerialUSB.println();
  SerialUSB.println();
  parameterQuery();
}


void step_response() {
  bool last_enabled = enabled;
  dir = true;
  enabled = 1;

  int current_position = y;
  bool received = false;
  int response_steps = 0;
  int last_step_target = step_target;

  unsigned long start_millis = millis();
  int time_out = 5000;

  //SerialUSB.println("Enter step value in degree!");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      response_steps = SerialUSB.parseInt();
      received = true;
    }
    else if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }
  }

  start_millis = millis();
  int timeframe = start_millis + 1000;
  int new_step_target = last_step_target + response_steps;

  while (millis() < timeframe) { //half a second

    SerialUSB.print(millis() - start_millis);
    SerialUSB.print(',');
    SerialUSB.print(r); //print target position
    SerialUSB.print(",");
    SerialUSB.println(y); // print current position

    if (millis() > start_millis + 300) {
      step_target = new_step_target;
    }
  }
  step_target = last_step_target;
  delay(1000);
  enabled = last_enabled;
}


void get_max_frequency() {
  disableTC5Interrupts();

  SerialUSB.println("");
  SerialUSB.println("Calibrating loop time");
  SerialUSB.println("---------------------");
  SerialUSB.println("make sure you move the motor while testing");
  SerialUSB.println("");

  int k = 1;
  int max_counter = 10000;
  int frequency = 100000;
  int temp_frequency = 0;
  bool last_enabled = enabled;
  enabled = 1;
  frequency_test = true;

  while (k <= 10) {
    int i = 1;

    unsigned long starting = micros();

    while (i <= max_counter) {
      TC5_Handler();
      i++;
    }

    temp_frequency = 1000000 / ((micros() - starting) / max_counter);

    if (temp_frequency < frequency) {
      frequency = temp_frequency;
    }

    SerialUSB.print("loop : ");
    SerialUSB.print(k);
    SerialUSB.print("/10 frequency = ");
    SerialUSB.println(temp_frequency);

    k++;

  }

  frequency = 10 * (floor(( 0.99 * frequency) / 10));

  SerialUSB.println("");
  SerialUSB.println("-----------");
  SerialUSB.print("volatile int FPID = ");
  SerialUSB.print(frequency);
  SerialUSB.println("; //Hz");

  enabled = last_enabled;

  enableTC5Interrupts();
}


void PID_autotune() {

  int loops = 0;
  int outputStep = (int)(uMAX);

  float temp_Kp = 0;
  float temp_Ki = 0;
  float temp_Kd = 0;

  bool abbort = false;
  bool received_1 = false;
  bool received_2 = false;
  bool timed_out = false;
  bool debug = false;
  unsigned long now = millis();
  int k = 0;

  SerialUSB.println("--- Autotuning the PID controller ---");
  SerialUSB.println("Please enter number of tuning cycles");
  SerialUSB.println("Press c to cancle!");
  SerialUSB.println("Tuning PID Parameters");

  while (!timed_out && !received_1) {
    if (SerialUSB.peek() != -1) {

      if (SerialUSB.peek() == 'd') {
        debug = true;
        while (!timed_out && !received_2) {
          if (SerialUSB.peek() != -1) {
            loops = SerialUSB.parseInt();
            received_2 = true;
            received_1 = true;
            k = 1;
          }
        }
      }
      else {
        loops = SerialUSB.parseInt();
        received_1 = true;
        k = 1;
      }
      SerialUSB.print("Tuning for: ");
      SerialUSB.print(loops);
      SerialUSB.println(" loop");
      SerialUSB.println("");
    }
    if (millis() > now + 5000) {
      timed_out = true;
      abbort = true;
      SerialUSB.print("Timed out: ");
    }
  }

  SerialUSB.println("| loop | Noise | Frequency | lookback | P    | I  | D     |");

  // start autotune
  while (k <= loops && !abbort) {
    SerialUSB.println("|---------------------------------------------------------|");
    SerialUSB.print("|   ");
    SerialUSB.print(k);
    SerialUSB.print("  ");

    tune_running = true;

    // set effort to zero for steady position
    u = 0;

    delay(200);

    // measure current setpoint
    int setpoint = 0;
    for (int i = 0; i < 10000; i++) {
      setpoint = setpoint + y;
    }
    setpoint = setpoint / 10000;

    // measuring the noise while the system is steady
    int higher_noise = 0;
    int lower_noise = 0;
    int temp_diff = 0;

    for (int i = 0; i < 10000; i++) {
      temp_diff = y - setpoint;
      if (temp_diff > higher_noise) higher_noise = temp_diff;
      if (temp_diff < lower_noise) lower_noise = temp_diff;
    }

    int noiseBand = (higher_noise - lower_noise);
    SerialUSB.print("|   ");
    SerialUSB.print(noiseBand);
    if (noiseBand >= 10) {
      SerialUSB.print("  ");
    }
    else {
      SerialUSB.print("   ");
    }

    unsigned long times_raw[1124] = {0};
    int points_raw[1124] = {0};
    int smoothed_raw[1124] = {0};
#define filterSamples   10
    int sensSmoothArray1 [filterSamples];

    int peakType = 0;
    int peakCount = 0;
    bool justchanged = false;
    int absMax = setpoint;
    int absMin = setpoint;

    unsigned long last_time = 0;

    bool isMax = true;
    bool isMin = true;

    // turn the PID loop off
    disableTC5Interrupts();

    // canceling call if something goes wrong
    if (SerialUSB.peek() != -1) {
      if (SerialUSB.read() == 'c') {
        tune_running = false;
        abbort = true;
      }
    }

    // step up the effort
    u = outputStep;

    int counter = 0;
    unsigned long start_millis = millis() + 500;

    // start the oscilations and measure the behavior every 50 microsseconds
    while (counter < 1124) {
      now = micros();
      if (now > last_time + 48) {

        raw_0 =  (pgm_read_word_near(lookup + readEncoder()));

        raw_diff = raw_0 - raw_1;

        if (raw_diff < -18000) {
          y = y + 36000 + raw_diff;
        }
        else if (raw_diff > 18000) {
          y = y - 36000 + raw_diff;
        }
        else {
          y = y  + raw_diff;
        }

        last_time = now;
        raw_1 = raw_0;


        if (y > absMax) {
          absMax = y;
        }
        if (y < absMin) {
          absMin = y;
        }

        // oscillate the output base on the input's relation to the setpoint
        if (y > setpoint + noiseBand) {
          u = -outputStep;
        }
        else if (y < setpoint - noiseBand) {
          u = outputStep;
        }

        if (u > 0) {
          output(-raw_0 - PA, abs(u));
        }
        else {
          output(-raw_0 + PA, abs(u));
        }

        // wait 1 second to get stable oscilations
        if (millis() > start_millis) {
          times_raw[counter] = now;
          points_raw[counter] = y;
          counter++;
          u = 0;
        }

      }
    }

    // turn PID loop back on
    enableTC5Interrupts();

    //smoothing the measured data
    for (int i = 0; i <= 1124; i++) {
      smoothed_raw[i] = digitalSmooth(points_raw[i], sensSmoothArray1);
    }


    // dump the first values
    unsigned long times[1024] = {0};
    int points[1024] = {0};
    float smoothed[1024] = {0};

    for (int i = 0; i < 1024; i++) {
      times[i] = times_raw[i + 50];
      points[i] = points_raw[i + 50];
      smoothed[i] = smoothed_raw[i + 50];
    }

    // building mean of the position data
    int sum = 0;
    for (int i = 0; i < 1024; i++) {
      sum = sum + smoothed[i];
    }
    sum = sum / 1024.0;

    for (int i = 0; i < 1024; i++) {
      smoothed[i] = smoothed[i] - sum;
      points[i] = points[i] - sum;
    }

    if (debug) {
      for (int i = 0; i < 1024; i++) {
        SerialUSB.print(times[i]);
        SerialUSB.print(',');
        SerialUSB.print(points[i]);
        SerialUSB.print(',');
        SerialUSB.println(smoothed[i]);
      }
    }

    // searching the main frequency
    int len = 1024;
    int thresh = 0;
    int sum_old = 0;
    char pd_state = 0;
    int period = 0;
    int i = 0;

    while (i < len - 1 && pd_state < 3) {
      sum_old = sum;
      sum = 0;

      for (int k = 1; k < len - i; k++) {
        sum = sum + (points[k]) * (points[k + i]);
      }

      if (pd_state == 2 && (sum - sum_old) <= 0) {
        period = i;
        pd_state = 3;
      }

      if (pd_state == 1 && (sum > thresh) && (sum - sum_old) > 0) {
        pd_state = 2;
      }

      if (i == 1) {
        thresh = sum * 0.5;
        pd_state = 1;
      }

      i = i + 1;
    }

    float frequency = (1000000.0 / 50.0) / period;
    double Tu = 1.0 / frequency;

    if (frequency < 40.0 || frequency > 9950.0) {
      abbort = true;
      SerialUSB.println();
      SerialUSB.println("Autotune failed, frequency not in usable range!");
    }
    else {
      SerialUSB.print("|   ");
      SerialUSB.print(frequency, 1);
      if (frequency >= 100.0) {
        SerialUSB.print("   ");
      }
      else if (frequency >= 1000.0) {
        SerialUSB.print("  ");
      }
      else {
        SerialUSB.print("    ");
      }

      // calculating lookback points
      int nLookBack = ((1000000 * Tu) / 50) / 4;
      if (nLookBack >= 99) {
        nLookBack = 99;
      }
      SerialUSB.print("|   ");
      SerialUSB.print(nLookBack);
      SerialUSB.print("     ");



      // searching the min and max in data

      int peaks[20] = {0};
      int lastInputs[101] = {0};
      int refVal = 0;

      for (int i = 0; i < 1024; i++) {
        refVal = smoothed[i];
        now = times[i];

        if (refVal > absMax) {
          absMax = refVal;
        }
        if (refVal < absMin) {
          absMin = refVal;
        }
        isMax = true;
        isMin = true;

        for (int i = nLookBack - 1; i >= 0; i--) {
          int val = lastInputs[i];

          if (isMax == 1) {
            isMax = refVal > val;
          }

          if (isMin == 1) {
            isMin = refVal < val;
          }

          lastInputs[i + 1] = lastInputs[i];
        }
        lastInputs[0] = refVal;

        if (isMax) {
          if (peakType == 0) {
            peakType = 1;
          }
          if (peakType == -1)
          {
            peakType = 1;
            justchanged = true;
          }
          peaks[peakCount] = refVal;

        }
        else if (isMin) {

          if (peakType == 0) {
            peakType = -1;
          }

          if (peakType == 1) {
            peakType = -1;
            peakCount++;
            justchanged = true;
          }
        }
      }


      //Amplitude of the oscilation
      float A = 0;
      for (int j = 1; j <= peakCount - 1; j++) {
        A = A + peaks[j];
      }
      A = A / (peakCount - 1);

      // calculating PID settings
      float Ku = 4 * 2 * outputStep * 1000 / (M_Pi * 2 * A);

      if (k == 1) {
        temp_Kp = (0.6 * Ku);
        temp_Ki = ((1.2 * Ku) / (Tu * FPID));
        temp_Kd = ((0.6 * Ku * Tu * FPID) / 8);
      }
      else {
        temp_Kp = temp_Kp + (0.6 * Ku);
        temp_Ki = temp_Ki + ((1.2 * Ku) / (Tu * FPID));
        temp_Kd = temp_Kd + ((0.6 * Ku * Tu * FPID) / 8);
      }

      SerialUSB.print("| ");
      SerialUSB.print((0.6 * Ku), 0);
      SerialUSB.print(" | ");
      SerialUSB.print(((1.2 * Ku) / (Tu * FPID)), 0);
      SerialUSB.print(" | ");
      SerialUSB.print(((0.6 * Ku * Tu * FPID) / 8), 0);
      SerialUSB.print(" |");

      SerialUSB.println("");
      delay(100);
    }

    k++;

  }
  // we are finished
  tune_running = false;
  if (!abbort) { //succses!
    SerialUSB.println("|---------------------------------------------------------|");
    SerialUSB.println();
    SerialUSB.println("Building Average!");

    Kp = (((temp_Kp / loops)) + 0.5);
    Ki = (((temp_Ki / loops)) + 0.5);
    Kd = (((temp_Kd / loops)) + 0.5);

    parameterQuery(); // print Parameter over Serialport
  }
  else {
    SerialUSB.println("cancelled!");
  }
}



int digitalSmooth(int rawIn, int *sensSmoothArray) {    // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  for (j = 0; j < filterSamples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j + 1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1);
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
  }

  return total / k;    // divide by number of samples
}


