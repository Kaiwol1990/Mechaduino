//Contains the declaration of the state variables for the control loop


#include <Wire.h>
#include "State.h"
#include "Parameters.h"

//interrupt vars
volatile int r = 0;  //setpoint

volatile int raw_0 = 0;  // measured angle
volatile int raw_1 = 0;
volatile int raw_diff = 0;


volatile int reading = 0;


volatile const int PA = (36000 / steps_per_revolution);


volatile int u = 0;  //real control effort

volatile int ITerm = 0; // Integral term

volatile int e_0 = 0; // error term
volatile int e_1 = 0;

volatile int yw_0 = 0; //filtered wraped angle
volatile int y = 0;

volatile bool dir = true;
volatile bool enabled = true;


//----current settings-----
const float rSense = 0.150;
const int uMAX = ((255 * iMAX * 10 * rSense) / 3.3);

//---- Step settings -----
const int counts_per_revolution = 16384;
const float angle_per_step = 360.0 / steps_per_revolution;
const int stepangle = 36000 / (steps_per_revolution*microstepping);

//---- Pins -----
const int IN_4 = 6;		//PA20
const int IN_3 = 5;		//PA15
const int VREF_2 = 4;	//PA08
const int VREF_1 = 9;	//PA07
const int IN_2 = 7;		//PA21
const int IN_1 = 8;		//PA06
const int ledPin = 13;	//PA17
const int chipSelectPin = A2; //PB08
const int dir_pin = 0;	//PORT_PA11
const int step_pin = 1;	//Port_PA10
const int ena_pin = 2;  //PORT_PA14


int val1 = 0;
int val2 = 0;

int stepNumber = 0; // step index for cal routine

int step_target = 0;

bool frequency_test = false;

extern const float Pi = 3.1415926535897932384626433832795;


const PROGMEM int sine_lookup[] = {0, 10, 20, 31, 41, 51, 61, 72, 82, 92, 102, 112, 123, 133, 143, 153, 163, 173, 183, 193, 203, 213, 223, 233, 243, 253, 263, 273, 283, 293, 303, 312, 322, 332, 341, 351, 361, 370, 380, 389, 399, 408, 418, 427, 436, 445, 455, 464, 473, 482, 491, 500, 509, 518, 526, 535, 544, 553, 561, 570, 578, 587, 595, 603, 612, 620, 628, 636, 644, 652, 660, 667, 675, 683, 690, 698, 705, 713, 720, 727, 735, 742, 749, 756, 763, 769, 776, 783, 789, 796, 802, 808, 815, 821, 827, 833, 839, 845, 850, 856, 862, 867, 873, 878, 883, 888, 893, 898, 903, 908, 913, 917, 922, 926, 930, 935, 939, 943, 947, 951, 954, 958, 962, 965, 968, 972, 975, 978, 981, 984, 987, 989, 992, 994, 997, 999, 1001, 1003, 1005, 1007, 1009, 1011, 1012, 1014, 1015, 1017, 1018, 1019, 1020, 1021, 1021, 1022, 1023, 1023, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1023, 1023, 1022, 1022, 1021, 1020, 1019, 1018, 1017, 1015, 1014, 1013, 1011, 1009, 1008, 1006, 1004, 1002, 999, 997, 995, 992, 990, 987, 984, 981, 979, 975, 972, 969, 966, 962, 959, 955, 951, 947, 943, 939, 935, 931, 927, 922, 918, 913, 909, 904, 899, 894, 889, 884, 879, 873, 868, 863, 857, 851, 846, 840, 834, 828, 822, 816, 809, 803, 797, 790, 784, 777, 770, 764, 757, 750, 743, 736, 729, 721, 714, 707, 699, 692, 684, 676, 669, 661, 653, 645, 637, 629, 621, 613, 605, 596, 588, 580, 571, 563, 554, 545, 537, 528, 519, 510, 501, 492, 483, 474, 465, 456, 447, 438, 428, 419, 410, 400, 391, 381, 372, 362, 353, 343, 333, 324, 314, 304, 294, 285, 275, 265, 255, 245, 235, 225, 215, 205, 195, 185, 175, 165, 155, 145, 134, 124, 114, 104, 94, 83, 73, 63, 53, 43, 32, 22, 12, 2,
                                  };

char pointer = 0;
long sum = 0;

