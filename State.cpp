  //Contains the declaration of the state variables for the control loop  


//interrupt vars

volatile float ei = 0.0;
volatile int U = 0;  //control effort (abs)
volatile float r = 0.0;  //setpoint
volatile float y = 0.0;  // measured angle
volatile float yw = 0.0;
volatile float yw_1 = 0.0;
volatile float e = 0.0;  // e = r-y (error)
volatile float p = 0.0;  // proportional effort
volatile float i = 0.0;  // integral effort
volatile float PA = 1.8;  //

volatile float u = 0.0;  //real control effort (not abs)
volatile long counter = 0;

volatile long wrap_count = 0;  //keeps track of how many revolutions the motor has gone though (so you can command angles outside of 0-360)
volatile float y_1 = 0;

volatile int enabled = 1;

const float iMAX = 0.7;  //Be careful adjusting this.  While the A4954 driver is rated for 2.0 Amp peak currents, it cannot handle these currents continuously.  Depending on how you operate the Mechaduino, you may be able to safely raise this value...please refer to the A4954 datasheet for more info
const float iPEAK = 1.4;
const float rSense = 0.150;

volatile int uMAX = (255/3.3)*(iMAX*10*rSense);
volatile int uPEAK = (255 / 3.3) * (iPEAK * 10 * rSense);

volatile int PEAKCounter = 0;
int maxPEAKCounter = 3000;
int PEAKSPERSECOND = 10;
int uSTEP = maxPEAKCounter / PEAKSPERSECOND ;

volatile float ITerm;

volatile char mode;


//___________________________________

const float pi = 3.14159265359;
const int  half = 134;//128;

float new_angle = 0.0; //input angle
float current_angle = 0.0; //current angle
float diff_angle = 0.0;
int val1 = 0;
int val2 = 0;


