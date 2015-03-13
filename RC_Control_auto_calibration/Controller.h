#pragma once
#include "ControllerParameters.h"
class Controller
{
public:

	enum GEAR {
		NEUTRAL, FORWARD, REVERSE
	};

	ControllerParameters accelerationIn;
	ControllerParameters brakingIn;
	ControllerParameters steeringIn;
	ControllerParameters brakeIn;
	ControllerParameters gearIn;
	
	ControllerParameters accelerationOut;
	ControllerParameters brakingOut;
	ControllerParameters steeringOut;
	ControllerParameters brakeOut;
	ControllerParameters gearOut;

	Controller(){};
        Controller(
          ControllerParameters accelerationIn, 
          ControllerParameters brakingIn, 
          ControllerParameters steeringIn, 
          ControllerParameters gearIn,
          ControllerParameters accelerationOut, 
          ControllerParameters brakingOut, 
          ControllerParameters steeringOut, 
          ControllerParameters gearOut);
	~Controller();
	
	double accelerationToPercent(int input);
	double brakingToPercent(int input);
	double steeringToPercent(int input);
	
        int getAccelerationOutput(double percent);
	int getBrakingOutput(double percent);
	int getSteeringOutput(double percent);
	
	GEAR getGear(int input);
};

