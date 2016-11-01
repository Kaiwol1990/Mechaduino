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

/*
  volatile float ITerm = 0.0; // Integral term

  volatile float e_0 = 0.0; // error term
  volatile float e_1 = 0.0;
*/
volatile int ITerm = 0; // Integral term

volatile int e_0 = 0; // error term
volatile int e_1 = 0;

volatile int yw_0 = 0; //filtered wraped angle
volatile int y = 0;

volatile bool dir = true;
volatile bool enabled = false;


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


int val1 = 0;
int val2 = 0;

int stepNumber = 0; // step index for cal routine

int step_target = 0;

bool frequency_test = false;

extern const float Pi = 3.1415926535897932384626433832795;

const PROGMEM int sine_lookup[] = {  //A sine lookup table is faster than using the built in sin() function)
  0  , 100  , 200  , 300  , 400  , 500  , 600  , 699  , 799  , 899  , 998  , 1098  , 1197  , 1296  , 1395  , 1494  , 1593  , 1692  , 1790  , 1889  , 1987  , 2085  , 2182  , 2280  , 2377  , 2474  , 2571  , 2667  , 2764  , 2860  , 2955  , 3051  , 3146  , 3240  , 3335  , 3429  , 3523  , 3616  , 3709  , 3802  , 3894  , 3986  , 4078  , 4169  , 4259  , 4350  , 4439  , 4529  , 4618  , 4706  , 4794  , 4882  , 4969  , 5055  , 5141  , 5227  , 5312  , 5396  , 5480  , 5564  , 5646  , 5729  , 5810  , 5891  , 5972  , 6052  , 6131  , 6210  , 6288  , 6365  , 6442  , 6518  , 6594  , 6669  , 6743  , 6816  , 6889  , 6961  , 7033  , 7104  , 7174  , 7243  , 7311  , 7379  , 7446  , 7513  , 7578  , 7643  , 7707  , 7771  , 7833  , 7895  , 7956  , 8016  , 8076  , 8134  , 8192  , 8249  , 8305  , 8360  , 8415  , 8468  , 8521  , 8573  , 8624  , 8674  , 8724  , 8772  , 8820  , 8866  , 8912  , 8957  , 9001  , 9044  , 9086  , 9128  , 9168  , 9208  , 9246  , 9284  , 9320  , 9356  , 9391  , 9425  , 9458  , 9490  , 9521  , 9551  , 9580  , 9608  , 9636  , 9662  , 9687  , 9711  , 9735  , 9757  , 9779  , 9799  , 9819  , 9837  , 9854  , 9871  , 9887  , 9901  , 9915  , 9927  , 9939  , 9949  , 9959  , 9967  , 9975  , 9982  , 9987  , 9992  , 9995  , 9998  , 9999  , 10000  , 10000  , 9998  , 9996  , 9992  , 9988  , 9982  , 9976  , 9969  , 9960  , 9951  , 9940  , 9929  , 9917  , 9903  , 9889  , 9874  , 9857  , 9840  , 9822  , 9802  , 9782  , 9761  , 9738  , 9715  , 9691  , 9666  , 9640  , 9613  , 9585  , 9556  , 9526  , 9495  , 9463  , 9430  , 9396  , 9362  , 9326  , 9290  , 9252  , 9214  , 9174  , 9134  , 9093  , 9051  , 9008  , 8964  , 8919  , 8874  , 8827  , 8780  , 8731  , 8682  , 8632  , 8581  , 8529  , 8477  , 8423  , 8369  , 8314  , 8258  , 8201  , 8143  , 8085  , 8026  , 7966  , 7905  , 7843  , 7781  , 7718  , 7654  , 7589  , 7523  , 7457  , 7390  , 7322  , 7254  , 7185  , 7115  , 7044  , 6973  , 6901  , 6828  , 6755  , 6681  , 6606  , 6530  , 6454  , 6378  , 6300  , 6222  , 6144  , 6065  , 5985  , 5904  , 5823  , 5742  , 5660  , 5577  , 5494  , 5410  , 5325  , 5240  , 5155  , 5069  , 4983  , 4896  , 4808  , 4720  , 4632  , 4543  , 4454  , 4364  , 4274  , 4183  , 4092  , 4001  , 3909  , 3817  , 3724  , 3631  , 3538  , 3444  , 3350  , 3255  , 3161  , 3066  , 2970  , 2875  , 2779  , 2683  , 2586  , 2489  , 2392  , 2295  , 2198  , 2100  , 2002  , 1904  , 1806  , 1708  , 1609  , 1510  , 1411  , 1312  , 1213  , 1114  , 1014  , 915  , 815  , 715  , 616  , 516  , 416  , 316  , 216  , 116  , 16  ,
};


char pointer = 0;
long sum = 0;

