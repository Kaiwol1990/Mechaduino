#include "Serial.h"
#include "Utils.h"
#include <SPI.h>
#include <arduino.h>
#include "board.h"


#include "Configuration.h"
#include "Configurationals.h"
#include "State.h"
#include "analogFastWrite.h"
#include "A4954.h"
#include "Encoder.h"
#include "lookup_table.h"
#include "board.h"
#include "Language.h"

void serialCheck() {
  bool ended = false;

  String Input = String("");
  while (SerialUSB.available() > 0) {
    char incomming = SerialUSB.read();
    if (incomming != '\n' && incomming != '\r') {

      Input = String(Input  + incomming);

      SerialUSB.print(incomming);
    }
    else {
      ended = true;
    }
  }

  // carriage return received
  if (ended) {
    SerialUSB.println();

    // split the input at the first space
    int first_space = Input.indexOf(' ');
    String inString = "";
    String secondString = "";

    if (first_space != -1) {
      // first command
      inString = Input.substring(0, first_space);

      // second command
      secondString = Input.substring(first_space + 1, sizeof(Input));
    }
    else {
      inString = Input;
    }


    if (inString.indexOf(calibrate_command) == 0 && inString.length() == calibrate_command.length()) {
      calibration();
    }
    else if (inString.indexOf(set_command) == 0 && inString.length() == set_command.length()) {
      setpoint(secondString);
    }
    else if (inString.indexOf(parameter_command) == 0 && inString.length() == parameter_command.length()) {
      parameterQuery();
    }
    else if (inString.indexOf(editparam_command) == 0 && inString.length() == editparam_command.length()) {
      parameterEdit(secondString);
    }
    else if (inString.indexOf(step_response_command) == 0 && inString.length() == step_response_command.length()) {
      step_response(secondString);
    }
    else if (inString.indexOf(help_command) == 0 && inString.length() == help_command.length()) {
      Serial_menu();
    }
    else if (inString.indexOf(looptime_command) == 0 && inString.length() == looptime_command.length()) {
      get_max_frequency();
    }
    else if (inString.indexOf(autotune_command) == 0 && inString.length() == autotune_command.length()) {
      PID_autotune();
    }
    else if (inString.indexOf(diagnostics_command) == 0 && inString.length() == diagnostics_command.length()) {
      readEncoderDiagnostics();
    }
    else if (inString.indexOf(noise_command) == 0 && inString.length() == noise_command.length()) {
      measure_noise();
    }
    else if (inString.indexOf(enable_command) == 0 && inString.length() == enable_command.length()) {
      enable();
    }
    else if (inString.indexOf(disable_command) == 0 && inString.length() == disable_command.length()) {
      disable();
    }
    else if (inString.indexOf(read_command) == 0 && inString.length() == read_command.length()) {
      readangle();
    }
    else if (inString.indexOf(reset_command) == 0 && inString.length() == reset_command.length()) {
      SoftReset();
    }
    else if (inString.indexOf(getstate_command) == 0 && inString.length() == getstate_command.length()) {
      getstate();
    }
    else {
      SerialUSB.println("unknown command send 'help'");
    }

    SerialUSB.println("");
    SerialUSB.print(":>");
  }
}


void SoftReset() {
  SerialUSB.println(reset_header);
  NVIC_SystemReset();      // processor software reset
}


void getstate() {
  SerialUSB.println(getstate_header);

  if (enabled) {
    SerialUSB.println(enable_header);
  }
  else {
    SerialUSB.println(disable_header);
  }
}


void enable() {
  enabled = true;
  SerialUSB.println(enable_header);
}

void disable() {
  enabled = false;
  SerialUSB.println(disable_header);
}

void Serial_menu() {
  SerialUSB.println(help_header);
  SerialUSB.println(help_command + " - " + help_menu);
  SerialUSB.println(calibrate_command + " - " + calibrate_menu);
  SerialUSB.println(diagnostics_command + " - " + diagnostics_menu);
  SerialUSB.println(disable_command + " - " + disable_menu);
  SerialUSB.println(editparam_command + " - " + editparam_menu);
  SerialUSB.println(enable_command + " - " + enable_menu);
  SerialUSB.println(read_command + " - " + read_menu);
  SerialUSB.println(reset_command + " - " + reset_menu);
  SerialUSB.println(getstate_command + " - " + getstate_menu);
  SerialUSB.println(step_response_command + " - " + step_response_menu);
  SerialUSB.println(parameter_command + " - " + parameter_menu);
  SerialUSB.println(set_command + " - " + set_menu);
  SerialUSB.println(autotune_command + " - " + autotune_menu);
  SerialUSB.println(looptime_command + " - " + looptime_menu);
  SerialUSB.println(noise_command + " - " + noise_menu);
}


void setpoint(String arg) {
  int new_angle = 0;
  bool ended = false;

  SerialUSB.print(set_header);

  if (arg == "") {
    // no argument was send!
    unsigned long start_millis;
    start_millis = millis();
    int time_out = 5000;

    while (ended == false && millis() < start_millis + time_out) {

      while (SerialUSB.available() > 0) {
        char incomming = SerialUSB.read();
        if (incomming != '\n' && incomming != '\r') {

          arg = String(arg  + incomming);
        }
        else {
          ended = true;
        }
      }
    }

  }
  else {
    ended = true;
  }

  if (ended) {
    // get the first char and check if its numeric
    char first = arg.charAt(0);
    char second = arg.charAt(1);

    if (isDigit(first) || (first == '-' && isDigit(second))) {
      SerialUSB.println(arg);

      new_angle = 100 * arg.toFloat();
      step_target = step_target + ( (new_angle - y) / (stepangle / 100.0));
    }
    else {
      SerialUSB.println("no valid input!");
    }
  }
  else {
    SerialUSB.println("time out");
  }
}


void readangle() {
  SerialUSB.print(read_header);
  SerialUSB.println((y / 100.0));
}


void parameterQuery() {
  SerialUSB.println(parameter_header);

  SerialUSB.print("#define Kp ");
  SerialUSB.println(int_Kp / 1000.0, 5);

  SerialUSB.print("#define Ki ");
  SerialUSB.println(int_Ki / 1000.0, 5);

  SerialUSB.print("#define Kd ");
  SerialUSB.println(int_Kd / 1000.0, 5);
}


void parameterEdit(String arg) {
  bool received_1 = false;
  bool received_2 = false;
  unsigned long start_millis = millis();
  int time_out = 5000;

  SerialUSB.println(editparam_header);
  SerialUSB.print("p ----- Kp = ");
  SerialUSB.println(int_Kp / 1000.0);

  SerialUSB.print("i ----- Ki = ");
  SerialUSB.println(int_Ki / 1000.0);

  SerialUSB.print("d ----- Kd = ");
  SerialUSB.println(int_Kd / 1000.0);

  SerialUSB.println("c ----- canceled");
  SerialUSB.println();


  while (!received_1)  {
    delay(100);
    char inChar2 = (char)SerialUSB.read();

    if (millis() > start_millis + time_out) {
      SerialUSB.println("time out!");
      return;
    }

    switch (inChar2) {
      case 'p':
        {
          SerialUSB.print("enter new Kp = ");
          while (millis() < start_millis + (2 * time_out)) {
            delay(100);
            if (SerialUSB.available()) {
              if (SerialUSB.peek() == '\r') {
                char dump = SerialUSB.read();
              }
              else {
                float temp_Kp = SerialUSB.parseFloat();
                SerialUSB.println(temp_Kp);

                int_Kp = 1000 * temp_Kp;
                big_Kp = (int_Kp * 2) / 10;
                small_Kp =  int_Kp;
                received_2 = true;
              }
            }
          }
          received_1 = true;
        }
        break;
      case 'i':
        {
          SerialUSB.print("enter new Ki = ");
          while (millis() < start_millis + (2 * time_out)) {
            delay(100);
            if (SerialUSB.available()) {
              if (SerialUSB.peek() == '\r') {
                char dump = SerialUSB.read();
              }
              else {
                float temp_Ki = 0.0;
                temp_Ki = SerialUSB.parseFloat();
                SerialUSB.println(temp_Ki);

                int_Ki = 1000 * temp_Ki;
                big_Ki = int_Ki;
                small_Ki = int_Ki / 2;
                ITerm_max = (uMAX * 1000) / (3 * big_Ki);
                received_2 = true;
              }
            }
          }
          received_1 = true;
        }
        break;
      case 'd':
        {
          SerialUSB.print("enter new Kd = ");
          while (millis() < start_millis + (2 * time_out)) {
            delay(100);
            if (SerialUSB.available()) {
              if (SerialUSB.peek() == '\r') {
                char dump = SerialUSB.read();
              }
              else {
                float temp_Kd = SerialUSB.parseFloat();
                SerialUSB.println(temp_Kd);

                int_Kd = 1000 * temp_Kd;
                small_Kd = int_Kd / 4;
                big_Kd = (8 * int_Kd) / 3;
                received_2 = true;
              }
            }
          }
          received_1 = true;
        }
        break;
      case 'c':
        {
          SerialUSB.println("canceled!");
          return;
        }
        break;
    }
  }
  if (!received_2) {
    SerialUSB.println("timed out!");
    return;
  }

  parameterQuery();
}


void step_response(String arg) {

  SerialUSB.print(step_response_header);

  int current_position = y;
  int response_steps = 0;
  int last_step_target = step_target;

  bool ended = false;

  unsigned long start_millis;
  start_millis = millis();
  int time_out = 5000;

  if (arg == "") {
    // no argument was send!

    while (ended == false && millis() < start_millis + time_out) {

      while (SerialUSB.available() > 0) {
        char incomming = SerialUSB.read();
        if (incomming != '\n' && incomming != '\r') {

          arg = String(arg  + incomming);
        }
        else {
          ended = true;
        }
      }
    }

  }
  else {
    ended = true;
  }

  if (ended) {
    // get the first char and check if its numeric
    char first = arg.charAt(0);
    if (isDigit(first)) {
      SerialUSB.println(arg);

      bool last_enabled = enabled;
      dir = true;
      enabled = true;

      response_steps = arg.toInt();
      start_millis = millis();
      int timeframe = start_millis + 1000;
      int new_step_target = last_step_target + response_steps;

      while (millis() < timeframe) { //half a second

        SerialUSB.print(millis() - start_millis);
        SerialUSB.print(',');
        SerialUSB.print(r); //print target position
        SerialUSB.print(", ");
        SerialUSB.println(y); // print current position

        if (millis() > start_millis + 300) {
          step_target = new_step_target;
        }
      }
      step_target = last_step_target;
      delay(1000);
      enabled = last_enabled;

    }
    else {
      SerialUSB.println("invalid input!");
    }
  }
  else {
    SerialUSB.println("time out");
  }
}


void get_max_frequency() {
  disableTC5Interrupts();
  SerialUSB.println(looptime_header);

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
    SerialUSB.print(" / 10 frequency = ");
    SerialUSB.println(temp_frequency);

    k++;

  }

  frequency = 10 * (floor(( 0.99 * frequency) / 10));

  SerialUSB.println("");
  SerialUSB.print("define FPID ");
  SerialUSB.print(frequency);
  SerialUSB.println("  //Hz");

  enabled = last_enabled;

  enableTC5Interrupts();
}


void readEncoderDiagnostics() {
  disableTC5Interrupts();
  long angleTemp;

  SerialUSB.println(diagnostics_header);

  digitalWrite(chipSelectPin, HIGH);
  delay(1);
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0xFF);
  SPI.transfer(0xFC);

  digitalWrite(chipSelectPin, HIGH);
  delay(1);
  digitalWrite(chipSelectPin, LOW);

  byte b1 = SPI.transfer(0xC0);
  byte b2 = SPI.transfer(0x00);

  digitalWrite(chipSelectPin, HIGH);
  delay(1);
  digitalWrite(chipSelectPin, LOW);

  b1 = SPI.transfer(0xC0);
  b2 = SPI.transfer(0x00);

  SerialUSB.println("Check DIAAGC register (0x3FFC)");

  angleTemp = (((b1 << 8) | b2) & 0B1111111111111111);
  SerialUSB.println((angleTemp | 0B1110000000000000000 ), BIN);

  if (angleTemp & (1 << 14))    SerialUSB.println("  Error occurred  ");

  if (angleTemp & (1 << 11))    SerialUSB.println("  MAGH - magnetic field strength too high, set if AGC = 0x00. This indicates the non-linearity error may be increased");

  if (angleTemp & (1 << 10))    SerialUSB.println("  MAGL - magnetic field strength too low, set if AGC = 0xFF. This indicates the output noise of the measured angle may be increased");

  if (angleTemp & (1 << 9))     SerialUSB.println("  COF - CORDIC overflow. This indicates the measured angle is not reliable");

  if (angleTemp & (1 << 8))     SerialUSB.println("  LF - offset compensation completed. At power-up, an internal offset compensation procedure is started, and this bit is set when the procedure is completed");

  if (!((angleTemp & (1 << 14)) | (angleTemp & (1 << 11)) | (angleTemp & (1 << 10)) | (angleTemp & (1 << 9)))) {
    SerialUSB.println("Looks good!");
  }

  SerialUSB.println(" ");


  digitalWrite(chipSelectPin, HIGH);
  delay(1);
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0x40);
  SPI.transfer(0x01);

  digitalWrite(chipSelectPin, HIGH);
  delay(1);
  digitalWrite(chipSelectPin, LOW);

  b1 = SPI.transfer(0xC0);
  b2 = SPI.transfer(0x00);


  SerialUSB.println("Check ERRFL register (0x0001)");


  angleTemp = (((b1 << 8) | b2) & 0B1111111111111111);
  SerialUSB.println((angleTemp | 0B1110000000000000000 ), BIN);

  if (angleTemp & (1 << 14)) {
    SerialUSB.println("  Error occurred  ");
  }
  if (angleTemp & (1 << 2)) {
    SerialUSB.println("  parity error ");
  }
  if (angleTemp & (1 << 1)) {
    SerialUSB.println("  invalid register  ");
  }
  if (angleTemp & (1 << 0)) {
    SerialUSB.println("  framing error  ");
  }

  if (!((angleTemp & (1 << 14)) | (angleTemp & (1 << 2)) | (angleTemp & (1 << 1)) | (angleTemp & (1 << 0)))) {
    SerialUSB.println("Looks good!");
  }


  digitalWrite(chipSelectPin, HIGH);

  delay(1);
  enableTC5Interrupts();
}


void measure_noise() {
  SerialUSB.println(noise_header);

  disableTC5Interrupts();
  int counter = 0;
  float points[3000] = {0};


  unsigned long times[3000] = {0};
  unsigned long now = micros();
  unsigned long last_time = now;
  int dt = ((1000000.0 / FPID) - 1);
  while (counter < 3000) {

    now = micros();
    if (now > last_time + dt) {
      last_time = now;
      points[counter] = (pgm_read_word_near(lookup + readEncoder()));
      counter++;
    }

  }

  float mean = 0;
  for (int i = 0; i < 3000; i++) {
    mean = mean + points[i];
  }
  mean = mean / 3000.0;

  float upper = 0;
  float lower = 0;
  int upcounter = 0;
  int lowcounter = 0;

  for (int i = 0; i < 3000; i++) {
    if (points[i] > mean ) {
      upcounter++;
      upper = upper + points[i];
    }
    else if (points[i] < mean) {
      lowcounter++;
      lower = lower + points[i];
    }
  }
  upper = upper / upcounter;
  lower = lower / lowcounter;

  float highest = 0;
  float lowest = 36000;

  for (int i = 0; i < 3000; i++) {
    if (points[i] > highest) {
      highest = points[i];
    }
    else if (points[i] < lowest) {
      lowest = points[i];
    }
  }
  /*
    for (int i = 0; i < 3000; i++) {
      SerialUSB.print(times[i]);
      SerialUSB.print(',');
      SerialUSB.println(points[i]);
    }
  */
  SerialUSB.print("min = ");
  SerialUSB.print( lowest / 100.0, 3 );
  SerialUSB.println(" degree");
  SerialUSB.print("max = ");
  SerialUSB.print( highest / 100.0, 3 );
  SerialUSB.println(" degree");
  SerialUSB.print("mean = ");
  SerialUSB.print( mean / 100.0, 3 );
  SerialUSB.println(" degree");
  SerialUSB.print("mean error = ");
  SerialUSB.print( (abs(upper) - abs(lower)) / 100, 3 );
  SerialUSB.println(" degree");
  SerialUSB.print("peak to peak error = ");
  SerialUSB.print( (abs(highest) - abs(lowest)) / 100, 3 );
  SerialUSB.println(" degree");

  enableTC5Interrupts();
}



