/*
 Send command to mechaduino to start an step responde test and plot the recieved data
 */

import processing.serial.*;
Serial myPort; //creates a software serial port on which you will listen to Arduino
int max = 8;

String val;
long start_time = 0;

float last_target = 0.0;
float last_position = 0.0;

boolean flag = true;
boolean first_event = true;
boolean finished = false;
int x = 2;
int lastheight_1 = 0;
int lastheight_2 = 0;
int lastheight_3 = 0;

float [] target_buffer = new float[2000];
float [] y_buffer = new float[2000];
float [] y_diff_1 = new float[2000];
float [] y_diff_2 = new float[2000];
long [] time_buffer = new long[2000];

void setup()
{
  size(1280, 720);
  String portName = Serial.list()[2]; 
  //CAUTION: your Arduino port number is probably different! Mine happened to be 1. Use a "handshake" sketch to figure out and test which port number your Arduino is talking on. A "handshake" establishes that Arduino and Processing are listening/talking on the same port.
  //Here's a link to a basic handshake tutorial: https://processing.org/tutorials/overview/
  for (int i=0; i<2000; i=i+1) {
    target_buffer[i]= 0;
    y_buffer[i] = 0;
    y_diff_1[i] = 0;
    y_diff_2[i] = 0;
  }


  myPort = new Serial(this, portName, 250000); //set up your port to listen to the serial port

  background(255, 255, 255);
  myPort.write('s'); // send s over serial to start step response
  println("send");
}

void serialEvent(Serial myPort) {
  if (first_event) {
    start_time= millis();
    first_event =false;
  }

  time_buffer[x] =(int)( millis()-start_time);

  String inString = myPort.readStringUntil('\n'); //The newline separator separates each Arduino loop. We will parse the data by each newline separator. 

  if (inString!= null) { //We have a reading! Record it.
    inString = trim(inString);

    float sensorVals[] = float(split(inString, ',')); 

    float target = sensorVals[0];
    float temp_1 =  map(target, -max, max, -(0.5*height), (0.5*height));
    target_buffer[x] = temp_1;

    float y = sensorVals[1];
    float temp_2 =  map(y, -max, max, -(0.5*height), (0.5*height));
    y_buffer[x] = temp_2;
    y_diff_1[x] = temp_2 - y_buffer[x-1];
    y_diff_2[x] = y_diff_1[x] -y_diff_1[x-1]; 

    x++;
  }
}



void draw() {
  if (millis()>5000) {
    float scale = ((float)width)/time_buffer[x-1];
    for (int i=1; i<x; i=i+1) {   
      stroke(0, 255, 0);     //stroke color
      strokeWeight(1);        //stroke wider
      line(scale*time_buffer[i-1], lastheight_1, scale*time_buffer[i], (0.5*height) - target_buffer[i]); 
      //line((i-1)*scale, lastheight_1, i*scale, (0.5*height) - target_buffer[i]); 
      lastheight_1 = (int)(0.5*height) - (int)target_buffer[i];

      stroke(255, 0, 0);     //stroke color
      strokeWeight(1);        //stroke wider
      line(scale*time_buffer[i-1], lastheight_2, scale*time_buffer[i], (0.5*height)  - y_buffer[i]); 
      lastheight_2 = (int)(0.5*height) -  (int)y_buffer[i];

      /*
      stroke(0, 0, 255);     //stroke color
       strokeWeight(1);        //stroke wider
       line(scale*time_buffer[i-1], lastheight_3, scale*time_buffer[i],   (0.5*height)  - (y_diff_1[i])); 
       lastheight_3 = (int)(0.5*height) -  (int)(y_diff_1[i]);
       */
    }
  }
}