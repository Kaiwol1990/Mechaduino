//Contains the declaration of the state variables for the control loop


#include <Wire.h>
#include "State.h"

//interrupt vars
volatile float r = 0.0;  //setpoint

volatile float y = 0.0;  // measured angle
volatile float y_1 = 0;
volatile float y_2 = 0;


volatile const float PA = 1.8;


volatile float u = 0.0;  //real control effort


volatile float ITerm = 0.0; // Integral term


volatile float e = 0.0; // error term
volatile float e_1 = 0.0;

volatile float y_filtered_0 = 0.0; // raw measured wrapped angle
volatile float y_filtered_1 = 0.0;
volatile float y_filtered_2 = 0.0;

volatile float yw = 0.0; //filtered wraped angle
volatile float yw_1 = 0.0;
volatile float yw_2 = 0.0;

volatile bool dir = true;
volatile bool enabled = false;


//----current settings-----
volatile int PEAKCounter = 0;
int maxPEAKCounter = 3000;

const float rSense = 0.150;
const int uMAX = ((255 * iMAX * 10 * rSense) / 3.3);
const int uPEAK = ((255 * iPEAK * 10 * rSense) / 3.3);
const int uSTEP = maxPEAKCounter / PEAKSPERSECOND ;

//---- Step settings -----
const int counts_per_revolution = 16384;
const float angle_per_step = 360.0 / steps_per_revolution;
const float stepangle = 360.0 / (steps_per_revolution*microstepping);

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


int val1 = 0;
int val2 = 0;

int stepNumber = 0; // step index for cal routine

float step_target = 0.0;

bool frequency_test = false;

extern const float Pi = 3.1415926535897932384626433832795;

const PROGMEM float sine_lookup[] = {   //A sine lookup table is faster than using the built in sin() function)
  0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.0699, 0.0799, 0.0899, 0.0998, 0.1098, 0.1197, 0.1296, 0.1395, 0.1494, 0.1593, 0.1692, 0.179, 0.1889, 0.1987, 0.2085, 0.2182, 0.228, 0.2377, 0.2474, 0.2571, 0.2667, 0.2764, 0.286, 0.2955, 0.3051, 0.3146, 0.324, 0.3335, 0.3429, 0.3523, 0.3616, 0.3709, 0.3802, 0.3894, 0.3986, 0.4078, 0.4169, 0.4259, 0.435, 0.4439, 0.4529, 0.4618, 0.4706, 0.4794, 0.4882, 0.4969, 0.5055, 0.5141, 0.5227, 0.5312, 0.5396, 0.548, 0.5564, 0.5646, 0.5729, 0.581, 0.5891, 0.5972, 0.6052, 0.6131, 0.621, 0.6288, 0.6365, 0.6442, 0.6518, 0.6594, 0.6669, 0.6743, 0.6816, 0.6889, 0.6961, 0.7033, 0.7104, 0.7174, 0.7243, 0.7311, 0.7379, 0.7446, 0.7513, 0.7578, 0.7643, 0.7707, 0.7771, 0.7833, 0.7895, 0.7956, 0.8016, 0.8076, 0.8134, 0.8192, 0.8249, 0.8305, 0.836, 0.8415, 0.8468, 0.8521, 0.8573, 0.8624, 0.8674, 0.8724, 0.8772, 0.882, 0.8866, 0.8912, 0.8957, 0.9001, 0.9044, 0.9086, 0.9128, 0.9168, 0.9208, 0.9246, 0.9284, 0.932, 0.9356, 0.9391, 0.9425, 0.9458, 0.949, 0.9521, 0.9551, 0.958, 0.9608, 0.9636, 0.9662, 0.9687, 0.9711, 0.9735, 0.9757, 0.9779, 0.9799, 0.9819, 0.9837, 0.9854, 0.9871, 0.9887, 0.9901, 0.9915, 0.9927, 0.9939, 0.9949, 0.9959, 0.9967, 0.9975, 0.9982, 0.9987, 0.9992, 0.9995, 0.9998, 0.9999, 1.0, 1.0, 0.9998, 0.9996, 0.9992, 0.9988, 0.9982, 0.9976, 0.9969, 0.996, 0.9951, 0.994, 0.9929, 0.9917, 0.9903, 0.9889, 0.9874, 0.9857, 0.984, 0.9822, 0.9802, 0.9782, 0.9761, 0.9738, 0.9715, 0.9691, 0.9666, 0.964, 0.9613, 0.9585, 0.9556, 0.9526, 0.9495, 0.9463, 0.943, 0.9396, 0.9362, 0.9326, 0.929, 0.9252, 0.9214, 0.9174, 0.9134, 0.9093, 0.9051, 0.9008, 0.8964, 0.8919, 0.8874, 0.8827, 0.878, 0.8731, 0.8682, 0.8632, 0.8581, 0.8529, 0.8477, 0.8423, 0.8369, 0.8314, 0.8258, 0.8201, 0.8143, 0.8085, 0.8026, 0.7966, 0.7905, 0.7843, 0.7781, 0.7718, 0.7654, 0.7589, 0.7523, 0.7457, 0.739, 0.7322, 0.7254, 0.7185, 0.7115, 0.7044, 0.6973, 0.6901, 0.6828, 0.6755, 0.6681, 0.6606, 0.653, 0.6454, 0.6378, 0.63, 0.6222, 0.6144, 0.6065, 0.5985, 0.5904, 0.5823, 0.5742, 0.566, 0.5577, 0.5494, 0.541, 0.5325, 0.524, 0.5155, 0.5069, 0.4983, 0.4896, 0.4808, 0.472, 0.4632, 0.4543, 0.4454, 0.4364, 0.4274, 0.4183, 0.4092, 0.4001, 0.3909, 0.3817, 0.3724, 0.3631, 0.3538, 0.3444, 0.335, 0.3255, 0.3161, 0.3066, 0.297, 0.2875, 0.2779, 0.2683, 0.2586, 0.2489, 0.2392, 0.2295, 0.2198, 0.21, 0.2002, 0.1904, 0.1806, 0.1708, 0.1609, 0.151, 0.1411, 0.1312, 0.1213, 0.1114, 0.1014, 0.0915, 0.0815, 0.0715, 0.0616, 0.0516, 0.0416, 0.0316, 0.0216, 0.0116, 0.0016
};

