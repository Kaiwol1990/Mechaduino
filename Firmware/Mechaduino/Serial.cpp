#include "Serial.h"
#include "Utils.h"
#include <SPI.h>
#include "board.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "State.h"
#include "analogFastWrite.h"
#include "A4954.h"
#include "Encoder.h"
#include "board.h"
#include "Language.h"
#include "Cmd.h"

void init_menu() {

  cmdAdd(help_command, Serial_menu);
  cmdAdd(calibrate_command, calibration);
  cmdAdd(diagnostics_command, readEncoderDiagnostics);
  cmdAdd(state_command, state);
  cmdAdd(stream_command,  set_streaming);
  cmdAdd(parameter_command, parameterEdit);
  cmdAdd(setpoint_command, setpoint );
  cmdAdd(reset_command, SoftReset);
  cmdAdd(dirac_command, dirac);
  cmdAdd(step_response_command, step_response);
  cmdAdd(error_command, error_reg);
  cmdAdd(autotune_command, PID_autotune);
  cmdAdd(downhill_command, downhill_simplex);
  cmdAdd(test_command, start_testmove);
  cmdAdd(interrupt_command, set_TC);

}

void state(int arg_cnt, char **args) {
  enabled = check_argument(args,  arg_cnt, "-off");
  enabled = check_argument(args,  arg_cnt, "-on");
  SerialUSB.println(state_header);
  SerialUSB.print(" Motor state = ");
  SerialUSB.println(enabled);
}



void SoftReset(int arg_cnt, char **args) {
  SerialUSB.println(reset_header);
  NVIC_SystemReset();      // processor software reset
}


void set_TC(int arg_cnt, char **args) {
  SerialUSB.println(interrupt_header);

  bool TC5_bool = return_bool_argument(args, arg_cnt, "-TC5", true);
  bool TC4_bool = return_bool_argument(args, arg_cnt, "-TC4", true);

  if (TC5_bool) {
    SerialUSB.println("TC5 enabled!");
    SerialUSB.println("TC4 enabled!");
    enableTC5Interrupts();
  }
  else {
    SerialUSB.println("TC5 disabled!");
    disableTC5Interrupts();
  }

  if (!TC4_bool) {
    SerialUSB.println("TC4 disabled!");
    disableTC4Interrupts();
  }

}


void Serial_menu(int arg_cnt, char **args) {

  SerialUSB.println(help_header);
  SerialUSB.println(help_command " - " help_menu);
  SerialUSB.println(calibrate_command " - " calibrate_menu);
  SerialUSB.println(diagnostics_command " - " diagnostics_menu);
  SerialUSB.println(state_command " - " state_menu);
  SerialUSB.println(stream_command " - " stream_menu);
  SerialUSB.println(parameter_command " - " parameter_menu);
  SerialUSB.println(error_command " - " error_menu);
  SerialUSB.println(dirac_command " - " dirac_menu);
  SerialUSB.println(step_response_command " - " step_response_menu);
  SerialUSB.println(error_command " - " error_menu);
  SerialUSB.println(autotune_command " - " autotune_menu);
  SerialUSB.println(downhill_command " - " downhill_menu);
  SerialUSB.println(test_command " - " test_menu);
  SerialUSB.println(interrupt_command " - " interrupt_menu);

}


void setpoint(int arg_cnt, char **args) {
  SerialUSB.println(setpoint_header);

  float new_angle = return_float_argument(args,  arg_cnt, "-set", (r / 100.0));
  step_target = ( (new_angle * 100.0) / stepangle) + 0.5;

  SerialUSB.print("Current target = ");
  SerialUSB.println((r / 100.0));
  SerialUSB.print("Current position = ");
  SerialUSB.println((y / 100.0));
}




void parameterEdit(int arg_cnt, char **args) {


  bool get_param = check_argument(args, arg_cnt, "-get");
  bool gui = check_argument(args, arg_cnt, "-gui");
  bool set_param = check_argument(args, arg_cnt, "-set");

  if (set_param) {
    SerialUSB.println(parameter_header);
    // check for calld arguments
    identifier = return_char_argument(args, arg_cnt, "-identifier", identifier);
    Kp = return_float_argument(args, arg_cnt, "-Kp", Kp, 0, 100);
    Ki = return_float_argument(args, arg_cnt, "-Ki", Ki, 0, 100);
    Kd = return_float_argument(args, arg_cnt, "-Kd", Kd, 0, 100);
    Kff = return_float_argument(args, arg_cnt, "-Kff", Kff, 0, 1);
    Kacc = return_float_argument(args, arg_cnt, "-Kacc", Kacc, 0, 1);
    PA = return_float_argument(args, arg_cnt, "-steps_per_revolution", PA, 90, 180);
    microstepping = return_float_argument(args, arg_cnt, "-microsteps", microstepping, 1, 128);
    iMAX = return_float_argument(args, arg_cnt, "-iMax", iMAX, 100, 2000);
    error_led_value = return_float_argument(args, arg_cnt, "-error_led_value", error_led_value, 1, 100);
    M_max = return_float_argument(args, arg_cnt, "-M_Max", M_max, 0.05, 5.0);
    I_rated = return_float_argument(args, arg_cnt, "-I_rated", I_rated, 1, 5000);
    D_Term_LPF = return_float_argument(args, arg_cnt, "-D_Term_LPF", D_Term_LPF, 100, 5000);
    u_LPF = return_float_argument(args, arg_cnt, "-effort_LPF", u_LPF, 100, 5000);
    mm_rev = return_float_argument(args, arg_cnt, "-mm_rev", mm_rev, 1, 500);
    USE_ENABLE_PIN = return_bool_argument(args, arg_cnt, "-USE_ENABLE_PIN",  USE_ENABLE_PIN);
    INVERT = return_bool_argument(args, arg_cnt, "-INVERT",  INVERT);


    // calculate all depending parameters new
    int_Kp = (1024 * Kp) + 0.5;
    int_Ki = (1024 * Ki) + 0.5;
    int_Kd = (1024 * Kd) + 0.5;
    int_Kff = (1024 * Kff) + 0.5;
    int_Kacc = (1024 * Kacc) + 0.5;

    step_add = 512 / microstepping;
    uMAX = ((512.0 * iMAX * 10 * rSense) / (1000 * 3.3));
    ITerm_max = (uMAX * 1024) + 0.5;
    D_Term_LPFa = ((128.0 * exp(D_Term_LPF * -2.0 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
    D_Term_LPFb = 128 - D_Term_LPFa;
    u_LPFa = ((128.0 * exp(u_LPF * -2.0 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
    u_LPFb = 128 - u_LPFa;
    max_e = (36000 * error_led_value) / mm_rev;
    if (USE_ENABLE_PIN) {
      enaInterrupt();
    }
    else {
      enabled = true;
    }
    dirInterrupt();
  }

  if (get_param) {
    SerialUSB.println(parameter_header);
    paramter_streamer(true, false);
  }

  if (gui) {
    paramter_streamer(false, true);
  }

}


void step_response(int arg_cnt, char **args) {
  SerialUSB.println(step_response_header);

  int response_steps = return_float_argument(args, arg_cnt, "-s", 1000, 200, 5000);

  int frequency = return_float_argument(args, arg_cnt, "-f", 5000, 1000, 10000);

  SerialUSB.print("Steps = ");
  SerialUSB.println(response_steps);
  SerialUSB.print("Frequency = ");
  SerialUSB.println(frequency);


  int last_step_target = step_target;
  bool last_enabled = enabled;
  bool last_dir = dir;
  dir = true;
  enabled = true;


  int answer[1000];
  int target[1000];

  int omega_answer[1000];
  int omega_target[1000];

  int counter = 0;

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  unsigned int dt = 1000000 / frequency;


  // calculate the target vector
  target[0] = step_target;
  for (int i = 1; i < 1000; i++) {
    target[i] = target[i - 1];

    if (i == 200) {
      target[i] = target[i - 1]  + (response_steps * step_add);
    }
  }

  while (counter < 1000) {
    current_time = micros();

    if (current_time >= next_time) {
      next_time = current_time + dt;

      answer[counter] = y;
      omega_answer[counter] = omega;
      step_target = target[counter];

      counter += 1;
    }
  }

  for (int i = 0; i < 999; i++) {
    omega_target[i] = target[i + 1] - target[i] ;
  }
  omega_target[999] = omega_target[998];

  for (int i = 0; i < 1000; i++) {
    target[i] = target[i] * stepangle;
  }

  for (int i = 0; i < 1000; i++) {
    SerialUSB.print('t');
    SerialUSB.print(',');
    SerialUSB.print(target[i]);
    SerialUSB.print(';');
    SerialUSB.print(answer[i]);
    SerialUSB.print(';');
    SerialUSB.print(omega_target[i]);
    SerialUSB.print(';');
    SerialUSB.println(omega_answer[i]);

  }

  // move back to last position
  while (step_target > last_step_target) {
    current_time = micros();
    if (current_time >= next_time) {
      next_time = current_time + 5;
      step_target -= 1;
    }
  }



  // set parameters back to the values before the
  enabled = last_enabled;
  dir = last_dir;
}


void dirac(int arg_cnt, char **args) {
  SerialUSB.println(dirac_header);


  int frequency = return_float_argument(args, arg_cnt, "-f", 5000, 1000, 10000);

  SerialUSB.print("Frequency = ");
  SerialUSB.println(frequency);


  int last_step_target = step_target;
  bool last_enabled = enabled;
  bool last_dir = dir;

  dir = true;
  enabled = true;

  int answer[1000];
  int target[1000];

  int omega_answer[1000];
  int omega_target[1000];

  int counter = 0;

  //unsigned int start_time = micros();
  unsigned int current_time = micros();
  unsigned int next_time = current_time;


  unsigned int dt = 1000000 / frequency;

  while (counter < 1000) {
    current_time = micros();

    if (current_time >= next_time) {

      if (counter == 200) {
        step_target = step_target + (1000 * step_add);
      }
      else if (counter == 201) {
        step_target = last_step_target;
      }

      next_time = current_time + dt;
      answer[counter] = y;
      target[counter] = r;
      omega_answer[counter] = omega;
      counter += 1;
    }
  }

  // calculate the velocity profiles
  for (int i = 0; i < 999; i++) {
    omega_target[i] = target[i + 1] - target[i] ;
  }
  omega_target[999] = omega_target[998];



  for (int i = 0; i < 1000; i++) {
    SerialUSB.print('t');
    SerialUSB.print(',');
    SerialUSB.print(target[i]);
    SerialUSB.print(';');
    SerialUSB.print(answer[i]);
    SerialUSB.print(';');
    SerialUSB.print(omega_target[i]);
    SerialUSB.print(';');
    SerialUSB.println(omega_answer[i]);
  }

  // set parameters back to the values before the response
  enabled = last_enabled;
  dir = last_dir;
}



void readEncoderDiagnostics(int arg_cnt, char **args) {
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

  if (angleTemp & (1 << 14))  SerialUSB.println("  Error occurred");
  if (angleTemp & (1 << 2))   SerialUSB.println("  parity error");
  if (angleTemp & (1 << 1))   SerialUSB.println("  invalid register");
  if (angleTemp & (1 << 0))   SerialUSB.println("  framing error");

  if (!((angleTemp & (1 << 14)) | (angleTemp & (1 << 2)) | (angleTemp & (1 << 1)) | (angleTemp & (1 << 0)))) {
    SerialUSB.println("Looks good!");
  }

  digitalWrite(chipSelectPin, HIGH);

  delay(1);
  enableTC5Interrupts();
}



void error_reg(int arg_cnt, char **args) {

  bool reset = check_argument(args, arg_cnt, "-reset");

  if (reset) {
    error_register = 0B1000000000000000;
  }

  SerialUSB.println(error_header);
  SerialUSB.println();

  SerialUSB.println(error_register & 0B1111111111111111, BIN);

  if (error_register & (1 << 0))    SerialUSB.println("  Timing error occured!");
  if (error_register & (1 << 1))    SerialUSB.println("  Maximal angular error exceeded!");
  if (error_register & (1 << 2))    SerialUSB.println("  Maximal coil current reached!");
  if (error_register & (1 << 3))    SerialUSB.println("  The lookup table has some failure!");

  if ((error_register & 0B1111111111111111) == 0B1000000000000000) SerialUSB.println("Looks good!");

}




void set_streaming(int arg_cnt, char **args) {
  SerialUSB.println(stream_header);

  float frequency = (FPID / max_serial_counter);

  frequency = return_float_argument(args, arg_cnt, "-f", frequency, 1, 500);

  bool temp_streaming = streaming;

  if (check_argument(args, arg_cnt, "-on")) {
    temp_streaming = true;
  }
  else if (check_argument(args, arg_cnt, "-off")) {
    temp_streaming = false;
  }

  max_serial_counter = ((float)FPID / frequency) + 0.5;
  float used_frequency = (float)FPID / (float)max_serial_counter;

  SerialUSB.print("state = ");
  SerialUSB.print(temp_streaming);
  SerialUSB.print(",f = ");
  SerialUSB.print(used_frequency);
  SerialUSB.print(" Hz");


  streaming = temp_streaming;


}


void streaming_handler() {

  byte buffer_length = fifo_counter - serial_counter;

  if (streaming && (buffer_length > 0)) {

    byte serial_position = mod(serial_counter, 99);

    SerialUSB.print("s,");
    for (int i = 0; i < 8; i++) {
      SerialUSB.print(fifo[i][serial_position]);
      SerialUSB.print(';');
    }

    SerialUSB.println(buffer_length);

    serial_counter++;
  }
}



void start_testmove(int arg_cnt, char **args) {
  SerialUSB.println(test_header);

  int F_Sample = return_float_argument(args, arg_cnt, "-f", 5000, 200, 10000);
  int velocity = return_float_argument(args, arg_cnt, "-v", 60, 10, 100);
  bool output = check_argument(args, arg_cnt, "-o");

  SerialUSB.print("Samplerate = ");
  SerialUSB.print(F_Sample);
  SerialUSB.println(" Hz");
  SerialUSB.print("velocity = ");
  SerialUSB.print(velocity);
  SerialUSB.println(" mm/s");

  int steps = (velocity * steps_per_revolution * step_add * microstepping) / (mm_rev * F_Sample);

  test_move(steps, F_Sample, output, 't');

}


