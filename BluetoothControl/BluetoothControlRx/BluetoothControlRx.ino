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
