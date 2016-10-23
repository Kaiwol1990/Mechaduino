//Contains the declaration of the state variables for the control loop


//interrupt vars
volatile float r = 0.0;  //setpoint
volatile float y = 0.0;  // measured angle
volatile float y_1 = 0;
volatile float yw = 0.0;

volatile float PA = 1.8;  //

volatile float u = 0.0;  //real control effort (not abs)

volatile float ITerm = 0.0; // Integral term

volatile long wrap_count = 0;  //keeps track of how many revolutions the motor has gone though (so you can command angles outside of 0-360)


volatile float e = 0.0;
volatile float e_1 = 0.0;
volatile float e_2 = 0.0;

volatile float raw_0 = 0.0; // raw_0 = r-y (raw measured error)
volatile float raw_1 = 0.0;
volatile float raw_2 = 0.0;

volatile bool enabled = false;

const float iMAX = 0.6;  //Be careful adjusting this.  While the A4954 driver is rated for 2.0 Amp peak currents, it cannot handle these currents continuously.  Depending on how you operate the Mechaduino, you may be able to safely raise this value...please refer to the A4954 datasheet for more info
const float iPEAK = 1.6;
const float rSense = 0.150;

volatile int uMAX = (255 / 3.3) * (iMAX * 10 * rSense);
volatile int uPEAK = (255 / 3.3) * (iPEAK * 10 * rSense);

volatile int PEAKCounter = 0;
int maxPEAKCounter = 3000;
int PEAKSPERSECOND = 10;
int uSTEP = maxPEAKCounter / PEAKSPERSECOND ;

//___________________________________

int val1 = 0;
int val2 = 0;

unsigned long next_millis = 0;

