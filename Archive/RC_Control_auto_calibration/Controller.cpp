#include "Controller.h"
#include "ControllerParameters.h"

Controller::Controller(
  ControllerParameters accelerationIn, 
  ControllerParameters brakingIn, 
  ControllerParameters steeringIn, 
  ControllerParameters gearIn,
  ControllerParameters accelerationOut, 
  ControllerParameters brakingOut, 
  ControllerParameters steeringOut, 
  ControllerParameters gearOut)
{
  this->accelerationIn = accelerationIn;
  this->brakingIn = brakingIn;
  this->steeringIn = steeringIn;
  this->gearIn = gearIn;
  this->accelerationOut = accelerationOut;
  this->brakingOut = brakingOut;
  this->steeringOut = steeringOut;
  this->gearOut = gearOut;
}

double Controller::accelerationToPercent(int input)
{
	//align input with the minimum allowable acceleration input from RC
	double output = (double)(input - accelerationIn.getTrueMin());
	//if output is less than zero, the input is not outside of the dead zone.
	if (output < 0){
		return 0;
	}

	//calculate percentage
	output /= (double)accelerationIn.getRange();
        if(output > 1){
          output = 1;
        }else if (output < 0){
          output = 0;
        }
	return output;
}

double Controller::brakingToPercent(int input){
	//align input with the minimum allowable brake input from RC
	double output = brakingIn.getTrueMin() - input;
	//if output is less than zero, the input is not outside of the dead zone.
	if (output < 0){
		return 0;
	}

	//calculate percentage
	output /= (double)brakingIn.getRange();
        if(output > 1){
          output = 1;
        }else if (output < 0){
          output = 0;
        }
	return output;
}

double Controller::steeringToPercent(int input)
{
	double output = (double)(input - steeringIn.min);
	int mid = ((steeringIn.max - steeringIn.min) / 2) + steeringIn.min;

	if(input > mid - steeringIn.deadZone && input < mid + steeringIn.deadZone){
		return 0.5;
	}
	
	if(input > steeringIn.max){
		return 1.0;
	}
	
	if(input < steeringIn.min){
		return 0.0;
	}
	
	if(input < mid - steeringIn.deadZone){
		output /= ((double)(mid - steeringIn.deadZone - steeringIn.min))*2;
	}else{
		output /= ((double)(steeringIn.max - mid + steeringIn.deadZone)*2);
	}
        if(output > 1){
          output = 1;
        }else if (output < 0){
          output = 0;
        }
	return output;
}

int Controller::getAccelerationOutput(double percent){
        int output = accelerationOut.getRange() * percent + accelerationOut.getTrueMin();
  	return output;
}
int Controller::getBrakingOutput(double percent){
        int output = brakingOut.getRange() * percent + brakingOut.getTrueMin();
  	return output;
}
int Controller::getSteeringOutput(double percent){
        int output = steeringOut.getRange() * percent + steeringOut.getTrueMin();;
        return output;
}

Controller::GEAR Controller::getGear(int input){

	int interval = gearIn.getRange() / 3;

	if (input <= (interval + gearIn.getTrueMin())){
		return REVERSE;
	}
	else if (input <= (interval * 2 + (gearIn.getTrueMin()))){
		return NEUTRAL;
	}

	return FORWARD;
}



Controller::~Controller()
{
}
