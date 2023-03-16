#include "BluetoothControlTx.h"

TransmitterData transmitterDat;
unsigned long currTime = 0;
unsigned int counter = 0;
String logger;
String dataSent;
char ackMessage;
char dataBuffer[BUFFER_LIMIT];
char ackBuffer[ACK_LIMIT];

// Pre-declare functions
void readInputs();

void setup()
{
    // Init hardware serial port --> PIN: 0 (RX) | 1 (TX)
    Serial1.begin(UART_BAUDRATE);
    if (DEBUG)
    {
        Serial3.begin(UART_BAUDRATE); // Serial Monitor
        while (!Serial3)
        {
            // Wait until Serial3 initialize
        }
        Serial3.println("** TranX Init at " + String(UART_BAUDRATE));
        Serial3.println("Setup Complete!");
    }
}

void loop()
{
    readInputs();

    // Send the values to through Bluetooh module
    String str = "Turn: " + String(transmitterDat.turn) + " - Throttle: " + String(transmitterDat.throttle) + " - Autonomous: " + String(transmitterDat.autonomous) + " - EBrake: " + String(transmitterDat.ebrake) + " - Reverse: " + String(transmitterDat.reverse) + '@';
    str.toCharArray(dataBuffer, BUFFER_LIMIT);

    for (int i = 0; i < BUFFER_LIMIT; i++)
    {
        Serial1.write(dataBuffer[i]);

        if (DEBUG)
        {
            Serial3.write(dataBuffer[i]);
        }
    }

    if (DEBUG)
    {
        Serial3.println();
    }
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
            Serial3.print(logger);
            if (ackMessage == '@')
            {
                break;
            }
        }
    }
    counter = 0;
    delay(100);
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
