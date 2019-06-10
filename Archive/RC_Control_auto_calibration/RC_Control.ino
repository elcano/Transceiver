#include "Controller.h"
#include <SPI.h>

/**
  Right stick X range
*/
const int RC_RIGHT_STICK_X_DEAD_ZONE = 50;
const int RC_RIGHT_STICK_X_MIN = 1223; //hard left
const int RC_RIGHT_STICK_X_MAX = 2036; //hard right

/**
  Right stick Y range
*/
const int RC_RIGHT_STICK_Y_DEAD_ZONE = 150;
const int RC_RIGHT_STICK_Y_MIN = 1460;
const int RC_RIGHT_STICK_Y_MAX = 2074;

/**
  Left stick Y range
*/
const int RC_LEFT_STICK_Y_DEAD_ZONE = 273;
const int RC_LEFT_STICK_Y_MIN = 983;
const int RC_LEFT_STICK_Y_MAX = 1900;

/**
  Braking input range
*/
const int RC_BRAKE_MIN = RC_RIGHT_STICK_Y_MIN;
const int RC_BRAKE_MAX =
  ((RC_RIGHT_STICK_Y_MAX - RC_RIGHT_STICK_Y_MIN) / 2) + RC_RIGHT_STICK_Y_MIN;
const int RC_BRAKE_DEFAULT = RC_BRAKE_MIN;

/**
  Acceleration input range
*/
// minimum acceleration can be considered BRAKE_MIN + 2*DEAD_ZONE
const int RC_ACC_MIN = ((RC_RIGHT_STICK_Y_MAX - RC_RIGHT_STICK_Y_MIN) / 2) + RC_RIGHT_STICK_Y_MIN;
const int RC_ACC_MAX = RC_RIGHT_STICK_Y_MAX;
const int RC_ACC_DEFAULT = RC_ACC_MIN;
/**
  Steering input range
*/
const int RC_STEER_MIN = RC_RIGHT_STICK_X_MIN; //left
const int RC_STEER_MAX = RC_RIGHT_STICK_X_MAX; //right
const int RC_STEER_DEFAULT = (RC_RIGHT_STICK_X_MAX - RC_RIGHT_STICK_X_MIN) / 2;

/**
  Gear input range
*/
const int RC_GEAR_MIN = RC_LEFT_STICK_Y_MIN; //left
const int RC_GEAR_MAX = RC_LEFT_STICK_Y_MAX; //right
const int RC_GEAR_DEFAULT = (RC_LEFT_STICK_Y_MAX - RC_LEFT_STICK_Y_MIN) / 2;

/**
  Acceleration output range
*/
const int OUT_ACC_MIN = 40;
const int OUT_ACC_MAX = 227;
const int OUT_ACC_DEFAULT = 40;

/**
  Braking output range
*/
const int OUT_BRAKE_MIN = 167;
const int OUT_BRAKE_MAX = 207;  

/**
  Steering output range
*/
const int OUT_STEER_MIN = 126;
const int OUT_STEER_MAX = 250;

/**
  Input pins
*/
const int RC_RIGHT_STICK_X_PIN_IN = 2;
const int RC_RIGHT_STICK_Y_PIN_IN = 3;
const int RC_LEFT_STICK_Y_PIN_IN = 4;

/**
  Output pins
*/
const int OUT_ACC_PIN = 5;
const int OUT_STEER_PIN = 6;
const int OUT_BRAKE_PIN = 7;
const int OUT_REVERSE_PIN = 8;

const int SelectCD = 49;  // Select IC 3 OUT (channels C and D)
const int SelectAB = 53;  // Select IC 2 OUT (channels A and B)

ControllerParameters accIn;
ControllerParameters brakeIn;
ControllerParameters steerIn;
ControllerParameters gearIn;

ControllerParameters accOut;
ControllerParameters brakeOut;
ControllerParameters steerOut;
ControllerParameters gearOut;

Controller controller;

void setup() {
    accIn = ControllerParameters(RC_ACC_MIN, RC_ACC_MAX, RC_RIGHT_STICK_Y_DEAD_ZONE, RC_RIGHT_STICK_Y_PIN_IN, RC_ACC_DEFAULT);
    brakeIn = ControllerParameters(RC_BRAKE_MIN, RC_BRAKE_MAX, RC_RIGHT_STICK_Y_DEAD_ZONE, RC_RIGHT_STICK_Y_PIN_IN, RC_BRAKE_DEFAULT);
    steerIn = ControllerParameters(RC_STEER_MIN, RC_STEER_MAX, RC_RIGHT_STICK_X_DEAD_ZONE, RC_RIGHT_STICK_X_PIN_IN, RC_STEER_DEFAULT);
    gearIn = ControllerParameters(RC_GEAR_MIN, RC_GEAR_MAX, RC_LEFT_STICK_Y_DEAD_ZONE, RC_LEFT_STICK_Y_PIN_IN, RC_GEAR_DEFAULT);
    
    accOut = ControllerParameters(OUT_ACC_MIN, OUT_ACC_MAX, 0, OUT_ACC_PIN, RC_ACC_DEFAULT);
    brakeOut = ControllerParameters(OUT_BRAKE_MIN, OUT_BRAKE_MAX, 0, OUT_BRAKE_PIN, RC_BRAKE_DEFAULT);
    steerOut = ControllerParameters(OUT_STEER_MIN, OUT_STEER_MAX, 0, OUT_STEER_PIN, RC_STEER_DEFAULT);
    gearOut = ControllerParameters(0, 0, RC_LEFT_STICK_Y_DEAD_ZONE, OUT_REVERSE_PIN, 0);
    
    controller = Controller(accIn, brakeIn, steerIn, gearIn, accOut, brakeOut, steerOut, gearOut);
    
    pinMode(OUT_ACC_PIN, OUTPUT);
    pinMode(OUT_BRAKE_PIN, OUTPUT);
    pinMode(OUT_STEER_PIN, OUTPUT);
    pinMode(OUT_REVERSE_PIN, OUTPUT);
    // SPI: set the slaveSelectPin as an output:
    pinMode (SelectAB, OUTPUT);
    pinMode (SelectCD, OUTPUT);
    pinMode (10, OUTPUT); //what is pin 10 being used for? AY
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    // initialize SPI:
    // The following line should not be neccessary. It uses a system library.
    PRR0 &= ~4;  // Steer off PRR0.PRSPI bit so power isn't off
    SPI.begin();
    for (int channel = 0; channel < 4; channel++)
        OUT_Write (channel, 0);   // reset did not clear previous states

    Serial.begin(9600);
}

void applyGear(Controller::GEAR gear){
  switch(gear){
    case Controller::FORWARD:
      digitalWrite(gearOut.pin, LOW);
      break;
    case Controller::REVERSE:
      digitalWrite(gearOut.pin, HIGH);
    default:
      break;
  }
}

void loop() {
  //Gear
  int input = pulseIn(gearIn.pin, HIGH, 25000);
  Controller::GEAR gear = controller.getGear(input);
  Serial.print("gear raw: ");
  Serial.print(input);
  Serial.print(", gear: ");
  Serial.println(gear);
  applyGear(gear);
  
  input = pulseIn(brakeIn.pin, HIGH, 25000);
  double percent = controller.brakingToPercent(input);
  int output = controller.getBrakingOutput(percent);
  Serial.print("brake raw input: ");
  Serial.print(input);
  Serial.print(", percent: ");
  Serial.print(percent);
  Serial.print(", output: ");
  Serial.println(output);
  rawBrake(output);
  
  percent = controller.accelerationToPercent(input);
  output = controller.getAccelerationOutput(percent);
  Serial.print("acceleration percent: ");
  Serial.print(percent);
  Serial.print(", output: ");
  Serial.println(output);
  rawAccelerate(output);
  
  input = pulseIn(steerIn.pin, HIGH, 25000);
  percent = controller.steeringToPercent(input);
  output = controller.getSteeringOutput(percent);
  Serial.print("steer raw input: ");
  Serial.print(input);
  Serial.print(", percent: ");
  Serial.print(percent);
  Serial.print(", output: ");
  Serial.println(output);
  rawSteer(output);
  
  delay(500);
}

void rawSteer(int pos)
{
  analogWrite(OUT_STEER_PIN, pos);
}

void rawBrake(int amount)
{
  analogWrite(OUT_BRAKE_PIN, amount);
}

void rawAccelerate(int acc)
{
    /* Observed behavior on ElCano #1 E-bike no load (May 10, 2013, TCF)
      0.831 V at rest       52 counts
      1.20 V: nothing       75
      1.27 V: just starting 79
      1.40 V: slow, steady  87
      1.50 V: brisker       94
      3.63 V: max          227 counts
      255 counts = 4.08 V
      */
   OUT_Write(0, acc);
}
/*---------------------------------------------------------------------------------------*/
/* OUT_Write applies value to address, producing an analog voltage.
// address: 0 for chan A; 1 for chan B; 2 for chan C; 3 for chan D
// value: digital value converted to analog voltage
// Output goes to mcp 4802 Digital-Analog Converter Chip via SPI
// There is no input back from the chip.
*/
void OUT_Write(int address, int value)

/*
REGISTER 5-3: WRITE COMMAND REGISTER FOR MCP4802 (8-BIT OUT)
A/B  —  GA  SHDN  D7 D6 D5 D4 D3 D2 D1 D0 x x x x
bit 15                                       bit 0

bit 15   A/B: OUTA or OUTB Selection bit
         1 = Write to OUTB
         0 = Write to OUTA
bit 14   — Don’t Care
bit 13   GA: Output Gain Selection bit
         1 = 1x (VOUT = VREF * D/4096)
         0 = 2x (VOUT = 2 * VREF * D/4096), where internal VREF = 2.048V.
bit 12   SHDN: Output Shutdown Control bit
         1 = Active mode operation. VOUT is available.
         0 = Shutdown the selected OUT channel. Analog output is not available at the channel that was shut down.
         VOUT pin is connected to 500 k (typical)
bit 11-0 D11:D0: OUT Input Data bits. Bit x is ignored.


With 4.95 V on Vcc, observed output for 255 is 4.08V.
This is as documented; with gain of 2, maximum output is 2 * Vref

*/

{
  int byte1 = ((value & 0xF0)>>4) | 0x10; // acitve mode, bits D7-D4
  int byte2 = (value & 0x0F)<<4;           // D3-D0
  if (address < 2)
  {
      // take the SS pin low to select the chip:
      digitalWrite(SelectAB,LOW);
      if (address >= 0)
      {
        if (address == 1)
          byte1 |= 0x80;  // second channnel
        SPI.transfer(byte1);
        SPI.transfer(byte2);
       }
      // take the SS pin high to de-select the chip:
      digitalWrite(SelectAB,HIGH);
  }
  else
  {
      // take the SS pin low to select the chip:
      digitalWrite(SelectCD,LOW);
      if (address <= 3)
      {
        if (address == 3)
          byte1 |= 0x80;  // second channnel
        SPI.transfer(byte1);
        SPI.transfer(byte2);
      }
       // take the SS pin high to de-select the chip:
      digitalWrite(SelectCD,HIGH);
  }
}
