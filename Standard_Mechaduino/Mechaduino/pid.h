#ifndef __PID_H__
#define __PID_H__

class PID{
	private:
		int Kp;
		int Ki;
		int Kd
		
		int get_Kp(){
			return Kp;
		}
		
		int get_Ki(){
			return Ki;
		}
		
		int get_Kd(){
			return Kd;
		}
		
		void set_Kp(int input){
			Kp = input;
		}
		
		void set_Ki(int input){
			Ki = input;
		}
		
		void set_Kd(int input){
			Kd = input;
		}
}

// construct the class called small_PID
PID small_PID;




#endif