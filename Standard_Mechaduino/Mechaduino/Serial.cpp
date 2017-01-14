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
  String Command = "";
  String argument = "";

  if (read_serialcommand(5000, &Command, &argument)) {
    if (Command.indexOf(calibrate_command) == 0 && Command.length() == calibrate_command.length()) {
      calibration();
    }
    else if (Command.indexOf(set_command) == 0 && Command.length() == set_command.length()) {
      setpoint(argument);
    }
    else if (Command.indexOf(parameter_command) == 0 && Command.length() == parameter_command.length()) {
      parameterQuery();
    }
    else if (Command.indexOf(editparam_command) == 0 && Command.length() == editparam_command.length()) {
      parameterEdit(argument);
    }
    else if (Command.indexOf(step_response_command) == 0 && Command.length() == step_response_command.length()) {
      step_response(argument);
    }
    else if (Command.indexOf(help_command) == 0 && Command.length() == help_command.length()) {
      Serial_menu();
    }
    else if (Command.indexOf(looptime_command) == 0 && Command.length() == looptime_command.length()) {
      get_max_frequency();
    }
    else if (Command.indexOf(autotune_command) == 0 && Command.length() == autotune_command.length()) {
      PID_autotune();
    }
    else if (Command.indexOf(diagnostics_command) == 0 && Command.length() == diagnostics_command.length()) {
      readEncoderDiagnostics();
    }
    else if (Command.indexOf(noise_command) == 0 && Command.length() == noise_command.length()) {
      measure_noise();
    }
    else if (Command.indexOf(enable_command) == 0 && Command.length() == enable_command.length()) {
      enable();
    }
    else if (Command.indexOf(disable_command) == 0 && Command.length() == disable_command.length()) {
      disable();
    }
    else if (Command.indexOf(read_command) == 0 && Command.length() == read_command.length()) {
      readangle();
    }
    else if (Command.indexOf(reset_command) == 0 && Command.length() == reset_command.length()) {
      SoftReset();
    }
    else if (Command.indexOf(getstate_command) == 0 && Command.length() == getstate_command.length()) {
      getstate();
    }
    else if (Command.indexOf(anticogging_command) == 0 && Command.length() == anticogging_command.length()) {
      antiCoggingCal();
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
  SerialUSB.println(anticogging_command  + " - " + anticogging_menu);
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

          SerialUSB.print(incomming);
        }
        else {
          ended = true;
        }
      }
    }

  }
  else {
    ended = true;
    SerialUSB.print(arg);
  }

  SerialUSB.println();

  if (ended) {
    // get the first char and check if its numeric
    char first = arg.charAt(0);
    char second = arg.charAt(1);

    if (isDigit(first) || (first == '-' && isDigit(second))) {

      new_angle = 100 * arg.toFloat();
      step_target = step_target + ( (new_angle - y) / (stepangle / 100.0));
    }
    else {
      SerialUSB.println(" no valid input!");
    }
  }
  else {
    SerialUSB.println(" timed out");
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
  SerialUSB.println(cancle_header);

  SerialUSB.print("p ----- Kp = ");
  SerialUSB.println(int_Kp / 1000.0);

  SerialUSB.print("i ----- Ki = ");
  SerialUSB.println(int_Ki / 1000.0);

  SerialUSB.print("d ----- Kd = ");
  SerialUSB.println(int_Kd / 1000.0);

  while (!received_1)  {

    if (timed_out(start_millis, time_out)) return;

    if (canceled()) return;

    char inChar2 = (char)SerialUSB.read();

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
                ITerm_max = (uMAX * 1000) / (3 * int_Ki);
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
                received_2 = true;
              }
            }
          }
          received_1 = true;
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

/*
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

          SerialUSB.print(incomming);
        }
        else {
          ended = true;
        }
      }
    }

  }
  else {
    ended = true;
    SerialUSB.print(arg);
  }

  SerialUSB.println();

  if (ended) {
    // get the first char and check if its numeric
    char first = arg.charAt(0);
    if (isDigit(first)) {

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
*/
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

    while (ended == false) {

      if (timed_out(start_millis, time_out)) return;

      while (SerialUSB.available() > 0) {
        char incomming = SerialUSB.read();
        if (incomming != '\n' && incomming != '\r') {

          arg = String(arg  + incomming);

          SerialUSB.print(incomming);
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

  SerialUSB.println();

  if (ended) {
    // get the first char and check if its numeric
    char first = arg.charAt(0);
    if (isDigit(first)) {

      // get the steps for te step response
      response_steps = arg.toInt();

      bool last_enabled = enabled;
      bool last_dir = dir;
      dir = true;
      enabled = true;

      SerialUSB.println("Close Serial Monitor and open Tools>>Serial Plotter");
      SerialUSB.println("You have 5 seconds...");

      for (byte i = 1; i <= 5; i++) {
        delay(1000);
        SerialUSB.print(5 - i);
        SerialUSB.println("...");
      }

      // set setp response flag to true to start the output
      response = true;

      //wait 300 ms to plot some values befor starting the step response
      delay(100);

      //set the target to the new value
      step_target = step_target + response_steps;
      //r = r + ((response_steps * stepangle) / 100);

      // wait 1 second to get the response
      delay(200);

      // set setp response flag back to false to stop the output
      response = false;

      // set parameters back to the values before the response
      enabled = last_enabled;
      dir = last_dir;

    }
    else {
      SerialUSB.println("invalid input!");
    }
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


int measure_noise(bool serialoutput) {
  if (serialoutput) {
    SerialUSB.println(noise_header);
    disableTC5Interrupts();
  }

  delay(100);
  int counter = 0;
  float points[500] = {0};


  unsigned long times[500] = {0};
  unsigned long now = micros();
  unsigned long last_time = now;
  int dt = ((1000000.0 / FPID) - 1);

  int y_1 = y;
  int raw_0 = mod(y, 36000);
  int raw_1 = raw_0;

  while (counter < 500) {

    now = micros();

    if (now > last_time + dt) {
      last_time = now;

      y = readAngle(y_1, raw_1);

      points[counter] = y;
      raw_0 = mod(y, 36000);

      raw_1 = raw_0;
      y_1 = y;
      counter++;
    }


  }

  float mean = 0;
  for (int i = 0; i < 500; i++) {
    mean = mean + points[i];
  }
  mean = mean / 500.0;

  float upper = 0;
  float lower = 0;
  int upcounter = 0;
  int lowcounter = 0;

  for (int i = 0; i < 500; i++) {
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

  float highest = mean;
  float lowest = mean;

  for (int i = 0; i < 500; i++) {
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

  if (serialoutput) {
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
    SerialUSB.print( abs(abs(upper) - abs(lower)) / 100.0, 3 );
    SerialUSB.println(" degree");
    SerialUSB.print("peak to peak error = ");
    SerialUSB.print( abs(abs(highest) - abs(lowest)) / 100.0, 3 );
    SerialUSB.println(" degree");

    enableTC5Interrupts();
  }
  else {
    return abs((abs(highest) - abs(lowest)));
  }

}

int measure_setpoint() {

  int setpoint = 0;
  int raw_0 = mod(y, 36000);
  int raw_1 = raw_0;
  int y_1 = y;

  for (int i = 0; i < 1000; i++) {

    y = readAngle(y_1, raw_1);
    raw_0 = mod(y, 36000);

    raw_1 = raw_0;
    y_1 = y;

    setpoint = setpoint + y;
  }
  return setpoint / 1000.0;

}




bool read_serialcommand(int timeout, String *command, String *argument) {
  static String Input = "";
  unsigned long start_millis;
  start_millis = millis();
  bool ended = false;

  while (ended == false && millis() < start_millis + timeout) {

    while (SerialUSB.available() > 0) {
      char incomming = SerialUSB.read();
      if (incomming != '\n' && incomming != '\r') {

        Input = String(Input  + incomming);

        SerialUSB.print(incomming);
      }
      else {
        SerialUSB.println();
        ended = true;
      }
    }
  }

  // received a full String -> split it
  if (ended) {

    // split the received command
    split_command(&Input, command, argument);

    // clear the input String
    Input = "";

    return true;
  }
  // no full command received
  else {

    return false;
  }
}

bool split_command(String *Input_pointer, String *first_substring, String *second_substring) {

  // search for a whitespace as delimieter
  String Input = *Input_pointer;

  int first_space = Input.indexOf(' ');

  if (first_space != -1) {
    // command
    *first_substring = Input.substring(0, first_space);

    // argument
    *second_substring = Input.substring(first_space + 1, sizeof(Input) / sizeof(char));

    // maybe there are more substrings
    return false;
  }
  else {
    // no substring found -> last substring
    *first_substring = Input;
    *second_substring = "";
    return true;
  }
}
