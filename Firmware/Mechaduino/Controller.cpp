//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include "State.h"
#include "Utils.h"
#include "A4954.h"
#include "board.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "Encoder.h"
#include "Serial.h"



// ----- gets called with FPID -----
// ----- calculates the target velocity and PID settings -----
void TC5_Handler() {

  //int32_t domega_target;

  //----- Variables -----
  static int32_t ITerm;
  int32_t DTerm;
  static int32_t IntDTerm;
  int32_t PTerm;

  static int32_t e_1;        // last error term
  static int32_t omega_target_1;

  int16_t phase_advanced;

  int32_t Derror = 0;

  static int u_pid;
  int32_t u_ff;
  int32_t u_acc;

  static uint16_t serial_loop_counter;

  //----- Calculations -----
  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt
    serial_loop_counter++;

    //domega_target = omega_target - omega_target_1;

    // start the calculations only if the motor is enabled
    if (enabled) {

      // ----------- PID loop -----------------------------------------

      // Integrational
      ITerm = ITerm + (int_Ki * error) + (int_Kb * (u - u_pid));
      ITerm = ITerm + ((int_Kff * int_Ki * error) / (1024 * constrain(1000 * abs(omega), 1, 1000000)));
      // ITerm = constrain(ITerm, -ITerm_max, ITerm_max);


      // Derivation

      Derror = (((error_LPFa * e_1) + (error_LPFb * error)) / 2048);
      DTerm = (int_Kd * (Derror - e_1) * constrain(abs(omega), 1, 1000))  / (100 * int_Kacc);


      DTerm = (DTerm - IntDTerm) * 100;
      IntDTerm = IntDTerm + (DTerm / 5000);

      // Proportional
      PTerm  = (int_Kp * error);

      // calculate the whole PID effort
      u_pid = (PTerm + ITerm + DTerm) / 1024;


      // constrain the effort to the user spedified maximum
      if (countPEAK < countPEAKMax) {

        // allow peak current
        if (u_pid > uPEAK) {
          u = uPEAK;
          countPEAK ++;
          countPEAKDEAD = 0;
        }
        else if (u_pid < -uPEAK) {
          u = -uPEAK;
          countPEAK ++;
          countPEAKDEAD = 0;
        }
        else {
          u = u_pid;
          countPEAKDEAD ++;
        }
      }
      else {

        // peak current limit is active
        if (u_pid > uMAX) {
          u = uMAX;
        }
        else if (u_pid < -uMAX) {
          u = -uMAX;
        }
        else {
          u = u_pid;
          countPEAKDEAD ++;
        }

      }
      countPEAK = constrain(countPEAK, 0, 100000);
      countPEAKDEAD = constrain(countPEAKDEAD, 0, 100000);

      if (countPEAKDEAD == countPEAKDEADMax) {
        countPEAKDEAD = 0;
        countPEAK = 0;
      }





    }
    else {
      omega_target = 0;
      step_target = y / stepangle;
      u = 0;
      ITerm = 0;
    }





    // calculate the phase advance term
    phase_advanced = sign(u) * PA;

 
    // calculate the electrical angle for the motor coils
    electric_angle = y + phase_advanced;

    // write the output
    output(electric_angle, u);


    // buffer the variables for the next loop
    e_1 = Derror;

    if (streaming) {

      if (serial_loop_counter >= max_serial_counter) {

        fifo_counter++;

        byte fifo_position = mod(fifo_counter, 49);

        fifo[0][fifo_position] = (1000000 * max_serial_counter) / FPID;
        fifo[1][fifo_position] = r;
        fifo[2][fifo_position] = y;
        fifo[3][fifo_position] = error;
        fifo[4][fifo_position] = omega_target;
        fifo[5][fifo_position] = omega;
        fifo[6][fifo_position] = u;
        fifo[7][fifo_position] = electric_angle;

        serial_loop_counter = 0;
      }
    }

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}








const int16_t omega_LPF = 150;
const int16_t omega_LPFa = ((2048.0 * exp(omega_LPF * -2.0 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
const int16_t omega_LPFb = 2048 - omega_LPFa;



const int16_t y_LPF = 500;
const int16_t y_LPFa = ((2048.0 * exp(omega_LPF * -2.0 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
const int16_t y_LPFb = 2048 - omega_LPFa;



// ----- reads the current shaft angle with 2*FPID -----
// ----- Oversamples the shaft angle to reduce noise -----
void TC4_Handler() {

  static int32_t last_step_target = 0;
  static int32_t y_1 = 0;
  static int32_t r_1;

  static int32_t r_raw_1 = 0;
  static int16_t r_counter = 0;
  int32_t dr = 0;
  static int16_t dt = 0;


  // Matlab Code for filter design:
  /*
    [b,a]=butter(3,3000/10000);
    b = round(b*512,0)
    a = round(a*512,0)
  */
  // 3. Ordnung
  const int32_t a[4] = {512, -595, 356, -71};
  const int32_t b[4] = {25, 76, 76, 25};

  static int32_t omega_buffer[4] = {0};
  static int32_t omegaRaw_buffer[4] = {0};



  if (TC4->COUNT16.INTFLAG.bit.OVF == 1) {

    // read the current angle
    y = ((y_LPFa * readAngle()) + (y_LPFb * y)) / 2048;

    omegaRaw_buffer[0] = (((omega_LPFa * omega) + (omega_LPFb * ((y - y_1) * Fs))) / 2048);


    omega_buffer[0] = (((b[0] * omegaRaw_buffer[0]) + (b[1] * omegaRaw_buffer[1]) + (b[2] * omegaRaw_buffer[2]) + (b[3] * omegaRaw_buffer[3])) - ((a[1] * omega_buffer[1]) + (a[2] * omega_buffer[2]) + (a[3] * omega_buffer[3]))) / 512;


    // calculate the current velocity
    omega = omega_buffer[0];





    // calculate the current target from the received steps and the angle per step and interpolate if no step was received
    r_counter = r_counter + 1;

    if (step_target != last_step_target) {
      r = step_target * stepangle;

      dr = r - r_raw_1;
      dt = r_counter;

      r_raw_1 = r;
      r_counter = 0;

    }
    else {
      if (dt > 0) {
        r = r + r_counter * (dr / dt);
      }
    }

    // stand still
    if (r_counter > 15) {
      dr = 0;
    }

    omega_target = ((omega_LPFa * omega_target) + (omega_LPFb * (r - r_1) * Fs)) / 2048;


    //calculate the error
    error = r - y;


    // wrap the angle arround to allow for inf travel
    if (y < -1000000) {

      y = mod(y, 36000);
      r = mod(r, 36000);
      step_target = y / stepangle;
    }
    else if (y > 1000000) {

      y = mod(y, 36000);
      r = mod(r, 36000);
      step_target = y / stepangle;
    }




    // shift back
    omegaRaw_buffer[3] = omegaRaw_buffer[2];
    omegaRaw_buffer[2] = omegaRaw_buffer[1];
    omegaRaw_buffer[1] = omegaRaw_buffer[0];

    omega_buffer[3] = omega_buffer[2];
    omega_buffer[2] = omega_buffer[1];
    omega_buffer[1] = omega_buffer[0];

    y_1 = y;
    r_1 = r;

    TC4->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}
