#pragma once
class ControllerParameters
{
public:
	int min;
	int max;
	int deadZone;
	int pin;
        int defaultValue;
	bool inverse;

	ControllerParameters();
	ControllerParameters(int min, int max, int deadzone, int pin, int defaultValue);
	ControllerParameters(int min, int max, int deadzone, int pin, int defaultValue, bool inverse);

	int getRange();

	int getTrueMin();
	
	int getTrueMax();
	
	~ControllerParameters();
};

