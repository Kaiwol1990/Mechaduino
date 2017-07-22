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
#include "lookup_table.h"
#include "board.h"
#include "Language.h"


void serialCheck() {


  String Command = "";
  String argument = "";

  if (read_serialcommand(500, &Command, &argument)) {
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
      step_response();
    }
    else if (Command.indexOf(dirac_command) == 0 && Command.length() == dirac_command.length()) {
      dirac();
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
    else if (Command.indexOf(check_lookup_command) == 0 && Command.length() == check_lookup_command.length()) {
      check_lookup(true);
    }
    else if (Command.indexOf(stream_start_command) == 0 && Command.length() == stream_start_command.length()) {
      streaming = true;
    }
    else if (Command.indexOf(stream_stop_command) == 0 && Command.length() == stream_stop_command.length()) {
      streaming = false;
    }
    else if (Command.indexOf(load_param_command) == 0 && Command.length() == load_param_command.length()) {
      send_param();
    }
    else if (Command.indexOf(disableTC5) == 0 && Command.length() == disableTC5.length()) {
      disableTC5Interrupts();
    }
    else if (Command.indexOf(enableTC5) == 0 && Command.length() == enableTC5.length()) {
      enableTC5Interrupts();
    }
    else if (Command.indexOf(print_error_command) == 0 && Command.length() == print_error_command.length()) {
      print_error_register();
    }
    else if (Command.indexOf(reset_error_command) == 0 && Command.length() == reset_error_command.length()) {
      reset_error_register();
    }
    else if (Command.indexOf(twiddle_command) == 0 && Command.length() == twiddle_command.length()) {
      twiddle();
    }
    else if (Command.indexOf(test_command) == 0 && Command.length() == test_command.length()) {
      test();
    }
    else if (Command.indexOf(downhill_command) == 0 && Command.length() == downhill_command.length()) {
      downhill_simplex();
    }
    else {
      SerialUSB.println("unknown command send 'help'");
    }

    SerialUSB.println("");
    SerialUSB.println(":>");
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
  SerialUSB.print(dir_header);
  SerialUSB.print(dir);
  SerialUSB.println(" ----");

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

  SerialUSB.println(print_error_command + " - " + print_error_menu);
  SerialUSB.println(reset_error_command + " - " + reset_error_menu);
  SerialUSB.println(diagnostics_command + " - " + diagnostics_menu);
  SerialUSB.println(check_lookup_command + " - " + check_lookup_menu);

  SerialUSB.println(enable_command + " - " + enable_menu);
  SerialUSB.println(disable_command + " - " + disable_menu);

  SerialUSB.println(parameter_command + " - " + parameter_menu);
  SerialUSB.println(editparam_command + " - " + editparam_menu);

  SerialUSB.println(read_command + " - " + read_menu);
  SerialUSB.println(set_command + " - " + set_menu);

  SerialUSB.println(getstate_command + " - " + getstate_menu);

  SerialUSB.println(calibrate_command + " - " + calibrate_menu);
  SerialUSB.println(autotune_command + " - " + autotune_menu);
  SerialUSB.println(step_response_command + " - " + step_response_menu);
  SerialUSB.println(noise_command + " - " + noise_menu);

  SerialUSB.println(reset_command + " - " + reset_menu);

}


void setpoint(String arg) {
  float new_angle = 0;
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

      new_angle = arg.toFloat();

      step_target = ( (new_angle * 100.0) / stepangle) + 0.5 ;
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
  SerialUSB.print("identifier = ");
  SerialUSB.println(identifier);

  SerialUSB.print("steps_per_revolution = ");
  SerialUSB.println(steps_per_revolution);

  SerialUSB.print("microstepping = ");
  SerialUSB.println(microstepping);

  SerialUSB.print("iMAX = ");
  SerialUSB.println(iMAX);

  SerialUSB.print("error_led_value = ");
  SerialUSB.println(error_led_value);

  SerialUSB.print("M_max = ");
  SerialUSB.println(M_max);

  SerialUSB.print("I_rated = ");
  SerialUSB.println(I_rated);

  SerialUSB.print("Kp = ");
  SerialUSB.println(int_Kp / 1000.0, 5);

  SerialUSB.print("Ki = ");
  SerialUSB.println(int_Ki / 1000.0, 5);

  SerialUSB.print("Kd = ");
  SerialUSB.println(int_Kd / 1000.0, 5);

  SerialUSB.print("D_Term_LPF = ");
  SerialUSB.println(D_Term_LPF);

  SerialUSB.print("effort_LPF = ");
  SerialUSB.println(u_LPF);

  SerialUSB.print("mm_rev = ");
  SerialUSB.println(mm_rev);

  SerialUSB.print("USE_ENABLE_PIN = ");
  SerialUSB.println(USE_ENABLE_PIN);

  SerialUSB.print("INVERT = ");
  SerialUSB.println(INVERT);
}


void parameterEdit(String arg) {
  unsigned long start_millis = millis();
  int time_out = 10000;
  if (arg == "") {
    SerialUSB.println(editparam_header);
    SerialUSB.println(cancle_header);

    SerialUSB.print("a ----- identifier = ");
    SerialUSB.println(identifier);

    SerialUSB.print("n ----- steps_per_revolution = ");
    SerialUSB.println(steps_per_revolution);

    SerialUSB.print("m ----- microstepping = ");
    SerialUSB.println(microstepping);

    SerialUSB.print("c ----- iMAX = ");
    SerialUSB.println(iMAX);

    SerialUSB.print("e ----- error_led_value = ");
    SerialUSB.println(error_led_value);

    SerialUSB.print("f ----- M_max = ");
    SerialUSB.println(M_max);

    SerialUSB.print("g ----- I_rated = ");
    SerialUSB.println(I_rated);

    SerialUSB.print("p ----- Kp = ");
    SerialUSB.println(int_Kp / 1000.0);

    SerialUSB.print("i ----- Ki = ");
    SerialUSB.println(int_Ki / 1000.0);

    SerialUSB.print("d ----- Kd = ");
    SerialUSB.println(int_Kd / 1000.0);

    SerialUSB.print("k ----- D_Term_LPF = ");
    SerialUSB.println(D_Term_LPF);

    SerialUSB.print("x ----- u_LPF = ");
    SerialUSB.println(u_LPF);

    SerialUSB.print("b ----- mm_rev = ");
    SerialUSB.println(mm_rev);

    SerialUSB.print("o ----- USE_ENABLE_PIN = ");
    SerialUSB.println(USE_ENABLE_PIN);

    SerialUSB.print("q ----- INVERT = ");
    SerialUSB.println(INVERT);
  }

  SerialUSB.read();


  while (1) {
    //delay(10);

    if (timed_out(start_millis, time_out)) return;

    if (canceled()) return;

    char inChar2 = (char)SerialUSB.read();


    switch (inChar2) {
      case 'p': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new Kp = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              Kp = SerialUSB.parseFloat();
              SerialUSB.println(Kp);

              int_Kp = (1024 * Kp) + 0.5;
              return;
            }

          }
        }
        break;
      case 'i': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new Ki = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              Ki = SerialUSB.parseFloat();
              SerialUSB.println(Ki);

              int_Ki = (1024 * Ki) + 0.5;
              return;
            }
          }
        }
        break;
      case 'd': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new Kd = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              Kd = SerialUSB.parseFloat();
              SerialUSB.println(Kd);

              int_Kd = (1024 * Kd) + 0.5;
              return;
            }
          }
        }
        break;
      case 'a': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new identifier [char] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              identifier = SerialUSB.read();
              SerialUSB.println(identifier);

              return;
            }
          }
        }
        break;
      case 'n': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new steps per revolution = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              steps_per_revolution = SerialUSB.parseInt();
              SerialUSB.println(steps_per_revolution);
              phase_multiplier = ((10 * steps_per_revolution / 4) / 100);
              PA = ((36000 / steps_per_revolution));

              return;
            }
          }
        }
        break;
      case 'm': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new microstepping = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              int temp_microstepping = pow(2, (floor(log2(SerialUSB.parseInt()) + 0.5)));

              if (temp_microstepping > 256) {
                SerialUSB.println();
                SerialUSB.println("Error microstepping can't be higher than 256!");
                SerialUSB.print("microstepping =");
              }
              else {
                microstepping = temp_microstepping;
              }

              SerialUSB.println(microstepping);
              step_add = 512 / microstepping;;

              return;
            }
          }
        }
        break;
      case 'r': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new maximal current [mA] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              iMAX = SerialUSB.parseInt();
              SerialUSB.println(iMAX);
              uMAX = ((512.0 * iMAX * 10 * rSense) / (1000 * 3.3));
              ITerm_max = (uMAX * 1024) + 0.5;

              return;
            }
          }
        }
        break;
      case 'e': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new error led Value [mm] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              error_led_value = SerialUSB.parseFloat();
              SerialUSB.println(error_led_value);
              max_e = (36000 * error_led_value) / mm_rev;

              return;
            }
          }
        }
        break;
      case 'f': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new Motor torque [Nm] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              M_max = SerialUSB.parseFloat();
              SerialUSB.println(M_max);

              return;
            }
          }
        }
        break;
      case 'g': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new Motor Current [mA] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              I_rated = SerialUSB.parseInt();
              SerialUSB.println(I_rated);

              return;
            }
          }
        }
        break;
      case 'k': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new frequenzy for D_Term_LPF [Hz] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              D_Term_LPF = SerialUSB.parseInt();
              SerialUSB.println(D_Term_LPF);

              D_Term_LPFa = ((128.0 * exp(D_Term_LPF * -2.0 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
              D_Term_LPFb = 128 - D_Term_LPFa;

              return;
            }
          }
        }
        break;
      case 'x': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new frequenzy for u_LPF [Hz] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              u_LPF = SerialUSB.parseInt();
              SerialUSB.println(u_LPF);

              u_LPFa = ((128.0 * exp(u_LPF * -2.0 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
              u_LPFb = 128 - u_LPFa;

              return;
            }
          }
        }
        break;
      case 'b': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new mm per revolution [mm] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              mm_rev = SerialUSB.parseInt();
              SerialUSB.println(mm_rev);

              max_e = (36000 * error_led_value) / mm_rev;

              return;
            }
          }
        }
        break;
      case 'o': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new enable pin setting [1/0] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              USE_ENABLE_PIN = SerialUSB.parseInt();
              SerialUSB.println(USE_ENABLE_PIN);

              if (USE_ENABLE_PIN) {
                enaInterrupt();
              }
              else {
                enabled = true;
              }

              return;
            }
          }
        }
        break;
      case 'q': {
          SerialUSB.read();
          start_millis = millis();
          SerialUSB.print("enter new Inverting setting [1/0] = ");

          while (1) {
            if (timed_out(start_millis, time_out)) return;
            delay(10);

            if (SerialUSB.available()) {
              INVERT = SerialUSB.parseInt();
              SerialUSB.println(INVERT);

              dirInterrupt();

              return;
            }
          }
        }
        break;
    }
  }

  //parameterQuery();
}


void step_response() {

  SerialUSB.print(step_response_header);

  int response_steps = 0;
  int frequency = 0;
  int last_step_target = step_target;

  unsigned const int time_out = 5000;
  unsigned int start_millis = millis();

  // get the number of steps for the step response
  while (response_steps == 0) {
    if (timed_out(start_millis, time_out)) return;
    response_steps = SerialUSB.parseInt();
  }

  SerialUSB.println(response_steps);
  SerialUSB.print(" Sample rate = ");

  // get the frequency for data recording
  while (frequency == 0) {
    if (timed_out(start_millis, time_out)) return;
    frequency = SerialUSB.parseInt();
  }

  if (frequency < 200) {
    // limit the lower edge of the frequency to 200 Hz
    frequency = 200;
    SerialUSB.println(frequency);
    SerialUSB.println(" lower frequency limited to 200 Hz");

  }
  else if (frequency > 10000) {
    // limit the upper frequency to 10 kHz
    frequency = 10000;
    SerialUSB.println(frequency);
    SerialUSB.println(" upper frequency limited to 10 kHz");
  }
  else {
    SerialUSB.println(frequency);
  }

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
  SerialUSB.println(frequency);


  while (counter < 1000) {
    current_time = micros();

    if (current_time >= next_time) {

      if (counter == 200) {
        step_target = step_target + (response_steps * step_add);
      }

      next_time = current_time + dt;
      answer[counter] = y;
      target[counter] = r;
      omega_answer[counter] = omega;
      counter += 1;
    }
  }


  for (int i = 0; i < 999; i++) {
    omega_target[i] = target[i + 1] - target[i] ;
  }
  omega_target[999] = omega_target[998];


  for (int i = 0; i < 1000; i++) {
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
      next_time = current_time + 50;
      step_target -= 1;
    }
  }



  // set parameters back to the values before the response
  enabled = last_enabled;
  dir = last_dir;
}


void dirac() {

  SerialUSB.print(dirac_header);

  int response_steps = 0;
  int frequency = 0;
  int last_step_target = step_target;


  response_steps = 1000;

  unsigned const int time_out = 5000;
  unsigned int start_millis = millis();

  SerialUSB.print(" Sample rate = ");

  // get the frequency for data recording
  while (frequency == 0) {
    if (timed_out(start_millis, time_out)) return;
    frequency = SerialUSB.parseInt();
  }

  if (frequency < 200) {
    // limit the lower edge of the frequency to 200 Hz
    frequency = 200;
    SerialUSB.println(frequency);
    SerialUSB.println(" lower frequency limited to 200 Hz");

  }
  else if (frequency > 10000) {
    // limit the upper frequency to 10 kHz
    frequency = 10000;
    SerialUSB.println(frequency);
    SerialUSB.println(" upper frequency limited to 10 kHz");
  }
  else {
    SerialUSB.println(frequency);
  }

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
        step_target = step_target + (response_steps * step_add);
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



void print_error_register() {

  SerialUSB.println(print_error_header);
  SerialUSB.println();

  SerialUSB.println(error_register & 0B1111111111111111, BIN);

  if (error_register & (1 << 0))    SerialUSB.println("  Timing error occured!");
  if (error_register & (1 << 1))    SerialUSB.println("  Maximal angular error exceeded!");
  if (error_register & (1 << 2))    SerialUSB.println("  Maximal coil current reached!");
  if (error_register & (1 << 3))    SerialUSB.println("  The lookup table has some failure!");

  if ((error_register & 0B1111111111111111) == 0B1000000000000000) SerialUSB.println("Looks good!");

}


void reset_error_register() {
  SerialUSB.println(reset_error_header);
  error_register = 0B1000000000000000;
}


int measure_noise(bool serialoutput) {
  disableTC5Interrupts();

  if (serialoutput) {
    SerialUSB.println(noise_header);
  }

  delay(100);
  int counter = 0;
  float points[500] = {0};
  float omega_buf[500] = {0};

  unsigned long now = micros();
  unsigned long last_time = now;
  int dt = ((1000000.0 / FPID) - 1);


  while (counter < 500) {
    now = micros();
    if (now > last_time + dt) {
      last_time = now;

      points[counter] = y;
      omega_buf[counter] = omega;
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

  }


  for (int i = 0; i < 500; i++) {
    SerialUSB.print(omega_buf[i]);
    SerialUSB.print(",");
  }
  enableTC5Interrupts();
  return abs((abs(highest) - abs(lowest)));

}



int measure_setpoint() {
  int setpoint = 0;

  for (int i = 0; i < 1000; i++) {
    setpoint = setpoint + y;
  }

  return setpoint / 1000.0;
}




bool read_serialcommand(int timeout, String * command, String * argument) {
  static String Input = "";
  unsigned long start_time = micros();
  bool ended = false;

  while (ended == false && micros() < start_time + timeout) {

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

bool split_command(String * Input_pointer, String * first_substring, String * second_substring) {

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



void Streaming() {
  if (streaming) {
    static unsigned int next_time;
    unsigned int current_time = millis();

    if (current_time >= next_time) {
      //streaming with 250 Hz
      next_time = current_time + 4;

      SerialUSB.print(streaming);         //print enable status
      SerialUSB.write(';');
      SerialUSB.print(y);               //print current position
      SerialUSB.write(';');
      SerialUSB.print(r);               //print target position
      SerialUSB.write(';');
      SerialUSB.print(error);           //print error
      //SerialUSB.print(omega);           //print error
      SerialUSB.write(';');
      SerialUSB.print(u);               //print effort
      SerialUSB.write(';');
      SerialUSB.print(electric_angle);  //print electric_angle
      SerialUSB.write(';');
      SerialUSB.print(enabled);         //print enable status
      SerialUSB.println();

    }
  }
}

void send_param() {

  SerialUSB.print(identifier);
  SerialUSB.write(';');
  SerialUSB.print(steps_per_revolution);
  SerialUSB.write(';');
  SerialUSB.print(microstepping);
  SerialUSB.write(';');
  SerialUSB.print(iMAX);
  SerialUSB.write(';');
  SerialUSB.print(M_max);
  SerialUSB.write(';');
  SerialUSB.print(I_rated);
  SerialUSB.write(';');
  SerialUSB.print(Kp, 5);
  SerialUSB.write(';');
  SerialUSB.print(Ki, 5);
  SerialUSB.write(';');
  SerialUSB.print(Kd, 5);
  SerialUSB.write(';');
  SerialUSB.print(D_Term_LPF);
  SerialUSB.write(';');
  SerialUSB.print(mm_rev);
  SerialUSB.write(';');
  SerialUSB.print(error_led_value);
  SerialUSB.write(';');
  SerialUSB.print(USE_ENABLE_PIN);
  SerialUSB.write(';');
  SerialUSB.print(INVERT);
  SerialUSB.write(';');
  SerialUSB.print(u_LPF);
  SerialUSB.println();

}





