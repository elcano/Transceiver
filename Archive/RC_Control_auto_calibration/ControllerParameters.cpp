#include "ControllerParameters.h"

ControllerParameters::ControllerParameters(){
}

ControllerParameters::ControllerParameters(int min, int max, int deadZone, int pin, int defaultValue)
{
	this->min = min;
	this->max = max;
	this->deadZone = deadZone;
	this->pin = pin;
	this->inverse = false;
        this->defaultValue = defaultValue;
}

ControllerParameters::ControllerParameters(int min, int max, int deadZone, int pin, int defaultValue, bool inverse)
{
	this->min = min;
	this->max = max;
	this->deadZone = deadZone;
	this->pin = pin;
        this->defaultValue = defaultValue;
	this->inverse = inverse;
}


int ControllerParameters::getRange(){
	if(inverse){
		return this->getTrueMin() - this->getTrueMax();
	}
	return this->getTrueMax() - this->getTrueMin();
}

int ControllerParameters::getTrueMin(){
	if(inverse){
		return (this->max - this->deadZone);
	}
	return (this->min + this->deadZone);
}

int ControllerParameters::getTrueMax(){
	if(inverse){
		return this->min;
	}
	return this->max;
}

ControllerParameters::~ControllerParameters()
{
}
