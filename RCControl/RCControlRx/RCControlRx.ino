/*
    REFERENCES: 
    https://www.instructables.com/id/How-to-control-Arduino-with-RC-receiver-in-10-minu/
    https://www.arduino.cc/en/Reference/Servo
    https://www.sparkfun.com/tutorials/348
*/
#include "RCControlRX.h"
#include "Settings.h"
#include "ElcanoSerial.h"
#include <RH_ASK.h> // external library

using namespace elcano;

int channel1, channel2, channel3, channel4, channel5, channel6;
unsigned long currTime = 0;
RH_ASK driver;

// declare functions
void setupReceiver(void);
void loopReceiver(void);
void waitInputPWM(int timeout);
void waitInputByte(int timeout);
void sendElcanoSerial(void);
void debugPacket(unsigned long timer);

void setup()
{
    Serial.begin(UART_BAUDRATE_SERIAL_USB);
    SerialUSB.begin(UART_BAUDRATE); // Serial Monitor
}

void loop()
{
    // Read the pulse width of each channel
    channel1 = pulseIn(THROTTLE_PIN, HIGH, 25000);
    channel2 = pulseIn(TURN_PIN, HIGH, 25000);
    channel3 = pulseIn(INTERRUPT_PIN, HIGH, 25000);
    channel4 = pulseIn(AUTO_PIN, HIGH, 25000);
    channel5 = pulseIn(EBRAKE_PIN, HIGH, 25000);
    channel6 = pulseIn(REVERSE_PIN, HIGH, 25000);

    if (DEBUG)
        channelDebugger();

    waitForProcess(100);
}

/***********************************************
    HELPER FUNCTIONS...
************************************************/
void channelDebugger()
{
    SerialUSB.print("C1: ");
    SerialUSB.println(channel1);
    SerialUSB.print("C2: ");
    SerialUSB.println(channel2);
    SerialUSB.print("C3: ");
    SerialUSB.println(channel3);
    SerialUSB.print("C4: ");
    SerialUSB.println(channel4);
    SerialUSB.print("C5: ");
    SerialUSB.println(channel5);
    SerialUSB.print("C6: ");
    SerialUSB.println(channel6);
}

void debugPacket(int throttle, int angle, int data, unsigned long timer)
{
    // this is useful for identifying problems at the transmitter
#define separator F(",\t")
    PC.print(timer, DEC);
    PC.print(separator);
    PC.print(throttle, DEC);
    PC.print(separator);
    PC.print(angle, DEC);
    PC.print(separator);
    PC.println(data, DEC);
}

void setupReceiver()
{
    driver.init();
    driver.setModeRx();
    PC.begin(baudrate);
    ESERIAL.begin(baudrate);
}

void waitForProcess(long interval)
{
    currTime = millis();
    while (millis() < currTime + interval)
    {
        ; // Wait for 10 ms to ensure packages are received
    }
}