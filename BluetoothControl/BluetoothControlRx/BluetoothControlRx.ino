#include "BluetoothControlRx.h"
//#include <mcp_can.h>      // <---- Import from another library: Seeed-Studio/CAN-BUS-Shield
//#include <mcp_can_dfs.h>  //       (install Library from Arduino, search for "Can-Bus-Shield")
#include <mcp2515_can.h>        // <---- Import from library: Seed-Studio/CAN-BUS-Shield
#include <mcp2515_can_dfs.h>    // <---- Import from library: Seed-Studio/CAN-BUS-Shield
#include <Can_Protocol.h> // <---- Import from path: elcano/Elcano_C2_LowLevel/Can_Protocol.h

// Core Variables
ReceiverData receiverDat;
mcp2515_can CAN(49); // chip selection pin for CAN. 53 for mega, 49 for our new low level board

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

// Pre-declare Functions
void processComingData();
void dataParser();
void ackMessage();
void sendToCanBus(ReceiverData message); 
void receiveFromCanBus();

void setup()
{
    // Init hardware serial port --> PIN: 0 (RX) | 1 (TX)
    Serial1.begin(UART_BAUDRATE);

    if (DEBUG)
    {
        Serial3.begin(UART_BAUDRATE); // Serial Monitor
        while (!Serial3)
        {
            // wait until Serial3 initialize
        }
        Serial3.println("** RecX Init at " + String(UART_BAUDRATE));
        Serial3.println("Setup Complete!");
    }

    // Inital CAN bus with 500KBPS baud rate (CAN_500KBPS is the baud rate)
    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        if (DEBUG)
        {
            Serial3.println("CAN BUS Shield init fail!!!");
            Serial3.println("Re-initializing...");
        }
        delay(100);
    }
    if (DEBUG)
    {
        Serial3.println("CAN BUS Shield init OK!");
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
            Serial3.println(str);
        }
        sendToCanBus(receiverDat);
        receiveFromCanBus();
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
                Serial3.println("T - " + String(dataChar));
            turnBuffer[turnCounter] = dataChar;
            turnCounter++;
            turnBuffer[turnCounter] = '\0'; // Keep the string NULL terminated
            break;
        case 'F': // Parsing Throttle
            if (DEBUG_PARSE)
                Serial3.println("F - " + String(dataChar));
            throttleBuffer[throttleCounter] = dataChar;
            throttleCounter++;
            throttleBuffer[throttleCounter] = '\0'; // Keep the string NULL terminated
            break;
        case 'A': // Parsing Autonomous
            if (DEBUG_PARSE)
                Serial3.println("A - " + String(dataChar));
            if (dataChar == '1')
                receiverDat.autonomous = true;
            else
                receiverDat.autonomous = false;
            break;
        case 'E': // Parsing Ebrake
            if (DEBUG_PARSE)
                Serial3.println("E - " + String(dataChar));
            if (dataChar == '1')
                receiverDat.ebrake = true;
            else
                receiverDat.ebrake = false;
            break;
        case 'R': // Parsing Reverse
            if (DEBUG_PARSE)
                Serial3.println("R - " + String(dataChar));
            if (dataChar == '1')
                receiverDat.reverse = true;
            else
                receiverDat.reverse = false;
            break;
        case '@': // Reaching the end of line
            if (dataChar == '@')
            {
                if (DEBUG_PARSE)
                    Serial3.println("EOT - " + String(dataChar));

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
                Serial3.println("Default - " + String(dataChar));
            break;
        }
    }
}

// ACKNOWLEDGE the data received
void ackMessage()
{
    if (DEBUG)
    {
        Serial3.println();
    }

    if (Serial1.available())
    {
        Serial1.print("ACK");
    }
}

// Process the data over the CAN BUS protocal
// NOTE: This function is not tested yet...
void sendToCanBus(ReceiverData message)
{
    if (DEBUG)
    {
        Serial3.println("Sending data to CAN BUS...");
    }

    // send CAN message to CAN BUS
    CAN.MCP_CAN::sendMsgBuf(Actual_CANID, 0, 8, (uint8_t *)&message);
    delay(1000); // a proper delay here is necessay, CAN bus need a time to clear the buffer. delay could be 100 minimum

    if (DEBUG)
    {
        Serial3.println("Messages SENT!");
    }
}

// Receive data from the other high/low level board
// NOTE: This function is not tested yet...
void receiveFromCanBus()
{
    unsigned char len = 0;      // message length
    unsigned char msgBuffer[8]; //8 Bytes buffer to store CAN message
    unsigned int canID = 0;

    // Check if received anything
    if (CAN_MSGAVAIL == CAN.checkReceive())
    {
        CAN.readMsgBuf(&len, msgBuffer); // put the data read into buffer and length
        canID = CAN.getCanId();

        if (canID = HiDrive_CANID) // <--- Change the CAN ID according to where this receiver transmitted the CAN message to.
        {
            if (DEBUG)
            {
                Serial3.print("HiDrive_CANID received: ");
                Serial3.println(canID, HEX);
            }
        }
        else if (canID == HiStatus_CANID)
        {
            if (DEBUG)
            {
                Serial3.print("HiStatus_CANID received: ");
                Serial3.println(canID, HEX);
            }
        }
        else if (canID == RCStatus_CANID)
        {
            if (DEBUG)
            {
                Serial3.print("RCStatus_CANID received: ");
                Serial3.println(canID, HEX);
            }
        }
        else if (canID == LowStatus_CANID)
        {
            if (DEBUG)
            {
                Serial3.print("LowStatus_CANID received: ");
                Serial3.println(canID, HEX);
            }
        }
        else if (canID == RCDrive_CANID)
        {
            if (DEBUG)
            {
                Serial3.print("RCDrive_CANID received: ");
                Serial3.println(canID, HEX);
            }
        }
        else if (canID == Actual_CANID)
        {
            if (DEBUG)
            {
                Serial3.print("Actual_CANID received: ");
                Serial3.println(canID, HEX);
            }
        }
        else
        {
            if (DEBUG)
            {
                Serial3.print("Unexpected CAN ID received: ");
                Serial3.println(canID, HEX);
            }
        }

        // Process incoming data (This step should be in the if/else statement above according to each if/else...)
        int resultFromCanBUS = (unsigned int)(msgBuffer[3] << 24) | (msgBuffer[2] << 16) | (msgBuffer[1] << 8) | (msgBuffer[0]);
        if (DEBUG)
        {
            Serial3.print("ACK msg from CAN BUS: ");
            Serial3.println(resultFromCanBUS, DEC);
            Serial3.println("Message received from the CAN BUS! Finished...");
        }
    }
}
