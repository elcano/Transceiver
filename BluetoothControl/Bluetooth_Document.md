# Bluetooth Modules:

###Bluetooth Transmitter
**Hardware Setup:**
1. Sparkfun SAMD21 Mini Breakout
1. HC-05 Bluetooth Modules (Master)
1. Parallax 2-Axis Joystick
1. Power supply
1. USB cable

| SPARKFUN SAMD21 MINI BREAKOUT  | PARALLAX 2-AXIS JOYSTICK | HC-05 Bluetooth  | 
| :----------------------------: | :----------------------: | :--------------: | 
| Pin 1 (TX)                     |                          | RX               | 
| Pin 0 (RX)                     |                          | TX               | 
| GND (ground)                   | GND (ground              | GND (ground)     | 
| VCC (5V)                       | L/R+ && U/D+ (5V)        | VCC (5v)         | 
| D8 (8)                         |                          | STATE            | 
| D9 (9)                         |                          | EN               | 
| A2                             | U/D                      |                  |
| A3                             | L/R                      |                  |

![Bluetooth Transmitter Image](Isolated.png "Transmitter")

**Software Setup:**
Bluetooth Transmitter .ino file
```c++
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
```

Bluetooth Transmitter .h file
```c++
// Arduino Serial Port 
#define UART_BAUDRATE 9600 // The current TX has to be at 9600 Baurd rate so that the RX can read the data at 38400 Buard rate --- This is off, but it works for now.
#define BUFFER_LIMIT 20
#define ACK_LIMIT 5

// transmitter input pins
#define TURN_PIN A3     // joystick 2 L/R
#define THROTTLE_PIN A2 // joystick 1 U/D
#define INTERRUPT_PIN 2
#define AUTO_PIN 5
#define EBRAKE_PIN 4
#define REVERSE_PIN 20

#define NO_PACKAGE -1 // No package received
#define DEBUG 1       // Prints debugging info to serialUSB, can impact loop time                        
                      // WARNING: when true (!0), you must connect USB to allow hardware reset, otherwise
                      // SAMD21 Arduino will do nothing*/

typedef struct
{
    unsigned int throttle;
    unsigned int turn;
    bool ebrake;
    bool autonomous;
    bool reverse; // Future Stuff
} transmitterData;
```

###Bluetooth Receiver
**Hardware Setup:**
1. Sparkfun SAMD21 Mini Breakout
1. HC-05 Bluetooth Modules (Slave)
1. Power supply
1. USB cable

|  HC-05 Bluetooth | SPARKFUN SAMD21 MINI BREAKOUT  |
| ---------------- | :----------------------------: |
| RX               | Pin 1 (TX)                     |
| TX               | Pin 0 (RX)                     |
| GND (ground)     | GND (ground)                   |
| VCC (5v)         | VCC (5V)                       |
| STATE            |                                |
| EN               |                                |

**Software Setup:**
Bluetooth Transmitter .ino file
```c++
#include "BluetoothControlRx.h"
//#include "Can_Protocol.h"
//#include <mcp_can.h> // using mcp_can lib from Seeed Studio

// Core Variables
receiverData receiverDat;
//MCP_CAN CAN(49); // chip selection pin for CAN. 53 for mega, 49 for our new low level board

// Utilities variables
unsigned long currTime = 0;
int turnCounter = 0;
int throttleCounter = 0;

// Data/Buffer variables
String logger;
char turnBuffer[BUFFER_LIMIT]; // char is similar to unsigned char --> which is a byte
char throttleBuffer[BUFFER_LIMIT];
char ackBuffer[ACK_LIMIT];
char dataChar;
char dividerChar;

void setup()
{
//    logger = "ACK@";
//    logger.toCharArray(ackBuffer, ACK_LIMIT);

    // Init hardware serial port --> PIN: 0 (RX) | 1 (TX)
    Serial1.begin(UART_BAUDRATE);
    SerialUSB.begin(UART_BAUDRATE); // Serial Monitor

    if (DEBUG)
        // wait until SerialUSB initialize
        while (!SerialUSB)
            ;

    // Inital CAN bus with 500KBPS baud rate (CAN_500KBPS is the baud rate)
    //    while (CAN_OK != CAN.begin(CAN_500KBPS))
    //    {
    //        if (DEBUG)
    //        {
    //            SerialUSB.println("CAN BUS Shield init fail");
    //            SerialUSB.println("Re-initializing...");
    //        }
    //        delay(1000);
    //    }

    if (DEBUG)
    {
        SerialUSB.println("CAN BUS init ok!");
        String initMesg = "** RecX Init at " + String(UART_BAUDRATE);
        SerialUSB.println(initMesg);
        SerialUSB.println("Setup Complete!");
    }
}

void loop()
{
    // Checks whether data is comming from the serial port
    while (Serial1.available() > 20)
    {
        processComingData();
        if (DEBUG)
        {
            String str = "Parsed value: T" + String(receiverDat.turn) + " F" + String(receiverDat.throttle) + " A" + String(receiverDat.autonomous) + " E" + String(receiverDat.ebrake) + " R" + String(receiverDat.reverse) + " @";
            SerialUSB.println(str);
        }
             transferToCanBus();
             //    ackMessage();
    }

    
}

/**
 * --------------------- Helper Functions --------------------------------
 */

// Gather/Process data and put it into dataBuffer buffer for parsing
void processComingData()
{
    dataChar = Serial1.read(); // Reads the data from the serial port
    dataParser();
}

// Parse the completed data buffer from the transmitter
void dataParser()
{

    if (dataChar == 'T' || dataChar == 'F' || dataChar == 'A' || dataChar == 'E' || dataChar == 'R' || dataChar == '@')
    {
        dividerChar = dataChar;
    }

    // Start parsing data byte
    if ((dividerChar == '@') || (dividerChar != dataChar))
    {
        switch (dividerChar)
        {
        case 'T': // Parsing Turn
            if (DEBUG_PARSE)
                SerialUSB.println("T - " + String(dataChar));
            turnBuffer[turnCounter] = dataChar;
            turnCounter++;
            turnBuffer[turnCounter] = '\0'; // Keep the string NULL terminated
            break;
        case 'F': // Parsing Throttle
            if (DEBUG_PARSE)
                SerialUSB.println("F - " + String(dataChar));
            throttleBuffer[throttleCounter] = dataChar;
            throttleCounter++;
            throttleBuffer[throttleCounter] = '\0'; // Keep the string NULL terminated
            break;
        case 'A': // Parsing Autonomous
            if (DEBUG_PARSE)
                SerialUSB.println("A - " + String(dataChar));
            if (dataChar == '1')
                receiverDat.autonomous = true;
            else
                receiverDat.autonomous = false;
            break;
        case 'E': // Parsing Ebrake
            if (DEBUG_PARSE)
                SerialUSB.println("E - " + String(dataChar));
            if (dataChar == '1')
                receiverDat.ebrake = true;
            else
                receiverDat.ebrake = false;
            break;
        case 'R': // Parsing Reverse
            if (DEBUG_PARSE)
                SerialUSB.println("R - " + String(dataChar));
            if (dataChar == '1')
                receiverDat.reverse = true;
            else
                receiverDat.reverse = false;
            break;
        case '@': // Reaching the end of line
            if (dataChar == '@')
            {
                if (DEBUG_PARSE)
                    SerialUSB.println("EOT - " + String(dataChar));

                // update turn and throttle struct
                receiverDat.turn = atoi(turnBuffer);
                receiverDat.throttle = atoi(throttleBuffer);

                // reset all counter and buffer
                turnCounter = 0;
                throttleCounter = 0;
                turnBuffer[turnCounter] = NULL;
                throttleBuffer[throttleCounter] = NULL;
            }
            break;
        default:
            if (DEBUG_PARSE)
                SerialUSB.println("Default - " + String(dataChar));
            break;
        }
    }
}

// Process the data over the CAN BUS protocal
void transferToCanBus()
{
    if (DEBUG)
    {
        SerialUSB.println("Sending data to CAN BUS...");
    }

    //    CAN.sendMsgBuf(RCDrive_CANID, 0, BUFFER_LIMIT, (char *)&receiverDat); // RCDrive_CANID or RCStatus_CANID i dont know...
    //    wait(100);
}

// ACKNOWLEDGE the data received
void ackMessage()
{
    SerialUSB.println();
    if (Serial1.available())
    {
        Serial1.print("ACK");
    }
}

// Better version of delay() in Arduino Lib
void wait(long interval)
{
    currTime = millis();
    while (millis() < currTime + interval)
    {
        ; // Wait for "interval" ms to ensure packages are received
    }
}

```

Bluetooth Receiver .h file
```c++
// Arduino Serial Port 
#define UART_BAUDRATE 38400
#define BUFFER_LIMIT 20
#define ACK_LIMIT 5

// Arduino Debugger
#define NO_PACKAGE -1 // No package received
#define DEBUG_PARSE 0 // This debug only for process coming data function
#define DEBUG 1       // Prints debugging info to serialUSB, can impact loop time                        
                      // WARNING: when true (!0), you must connect USB to allow hardware reset, otherwise
                      // SAMD21 Arduino will do nothing*/

// Data struct of packages being sent
typedef struct
{
    unsigned int throttle;
    unsigned int turn;
    bool ebrake;
    bool autonomous;
    bool reverse; // Future Stuff
} receiverData;

#define RCStatus_CANID 0x50
#define HiStatus_CANID 0x100
#define LowStatus_CANID 0x200
#define RCDrive_CANID 0x300
#define HiDrive_CANID 0x350
#define Actual_CANID 0x500
```

### References/Tip
1. Sparkfun SAMD21 Mini Breakout hook up guide: https://learn.sparkfun.com/tutorials/samd21-minidev-breakout-hookup-guide/samd21-mini-breakout-overview
1. If the receiver gets a reverse question mark, it might because of both the receiver and the transmitter Baud Rate are not configured at the same rate. Please try Baud Rate: 38400 (on the Receiver) and 9600 (on the Transmitter) and vice versa to see if the outcome is any better.