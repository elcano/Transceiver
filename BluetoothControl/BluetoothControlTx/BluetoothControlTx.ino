#include "BluetoothControlTx.h"

// initialize package
transmitterData transmitterDat;
// Utilities variables
unsigned long currTime = 0;
unsigned int counter = 0;
// Data/Buffer variables
String logger;
String dataSent;
char ackMessage; // received from the RX when it got the package
char dataBuffer[BUFFER_LIMIT];
char ackBuffer[ACK_LIMIT];

void setup()
{
    // Init hardware serial port --> PIN: 0 (RX) | 1 (TX)
    Serial1.begin(UART_BAUDRATE);
    SerialUSB.begin(UART_BAUDRATE); // Serial Monitor
    if (DEBUG)
        // wait until SerialUSB initialize
        while (!SerialUSB)
            ;

    String initMesg = "** TranX Init at " + String(UART_BAUDRATE);
    SerialUSB.println(initMesg);
    SerialUSB.println("Setup Complete!");
}

void loop()
{
    readInputs();

    // Send the values to through Bluetooh module
    String str = 'T' + String(transmitterDat.turn) + 'F' + String(transmitterDat.throttle) 
    + 'A' + String(transmitterDat.autonomous) + 'E' + String(transmitterDat.ebrake) 
    + 'R' + String(transmitterDat.reverse) + '@';
    str.toCharArray(dataBuffer, BUFFER_LIMIT);

    for (int i = 0; i < BUFFER_LIMIT; i++)
    {
        Serial1.write(dataBuffer[i]);
        SerialUSB.write(dataBuffer[i]);
    }
    SerialUSB.println();
    // Read from Bluetooh module and send to Arduino Serial Monitor
    // (Receiving ACK message from the RX)
    while (Serial1.available() > 0)
    {
        if (DEBUG)
        {
            ackMessage = Serial1.read();
            ackBuffer[counter] = ackMessage;
            counter++;
            dataBuffer[counter] = '\0'; // Keep the string NULL terminated
            SerialUSB.print(logger);
            if (ackMessage == '@') {
                break;
            }
        }
    }
    counter = 0;
    waitForProcess(100);
}

/**
 * --------------------- Helper Functions --------------------------------
 */
void readInputs()
{
    transmitterDat.turn = analogRead(TURN_PIN);
    transmitterDat.throttle = analogRead(THROTTLE_PIN);
    transmitterDat.autonomous = digitalRead(AUTO_PIN);
    transmitterDat.ebrake = digitalRead(EBRAKE_PIN);
    transmitterDat.reverse = digitalRead(REVERSE_PIN);
}

// Better version of delay() in Arduino Lib
void waitForProcess(long interval)
{
    currTime = millis();
    while (millis() < currTime + interval)
    {
        ; // Wait for "interval" ms to ensure packages are received
    }
}