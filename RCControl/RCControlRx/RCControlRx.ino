/*
    --- References and Acknowledgements ---
    Combine and Update RC control code written by Joe Breithaupt from path: elcano/RadioControl/RadioControl.ino
    https://www.instructables.com/id/How-to-control-Arduino-with-RC-receiver-in-10-minu/
    https://www.arduino.cc/en/Reference/Servo
    https://www.sparkfun.com/tutorials/348
*/
#include "RCControlRX.h"      // <---- Local .h file
// #include <mcp_can.h>          // <---- Import from another library: Seeed-Studio/CAN-BUS-Shield
// #include <mcp_can_dfs.h>      //       (install Library from Arduino, search for "Can-Bus-Shield")
#include <mcp2515_can.h>        // <---- Import from library: Seed-Studio/CAN-BUS-Shield
#include <mcp2515_can_dfs.h>    // <---- Import from library: Seed-Studio/CAN-BUS-Shield
#include <Can_Protocol.h>     // <---- Import from path: elcano/Elcano_C2_LowLevel/Can_Protocol.h
#include <SettingsTemplate.h> // <---- Import from path: elcano/libraries/Settings/SettingsTemplate.h

mcp2515_can CAN(CAN_PIN); // chip selection pin for CAN. 53 for mega, 49 for our new low level board

String logger;
unsigned long now = 0;
unsigned long currTime = 0;
unsigned long duration = 0;
ReceiverData receiverData;
volatile bool RC_Done[RC_NUM_SIGNALS];
volatile unsigned long RC_rise[RC_NUM_SIGNALS];
volatile unsigned long RC_elapsed[RC_NUM_SIGNALS];
unsigned long rc_throttle, rc_turn, rc_reverse, rc_rudd, rc_ebrake, rc_auto;

// Pre-declare functions
void waitInputPWM();
void debugPacket();
void sendToCanBus(ReceiverData message);
void receiveFromCanBus();

// Pre-define attachInterrupt functions
void ISR_TURN_rise();
void ISR_AUTO_rise();
void ISR_ESTOP_rise();
void ISR_GO_rise();
void ISR_REVS_rise();
void ISR_RUDD_rise();
void ISR_GO_fall();
void ISR_TURN_fall();
void ISR_AUTO_fall();
void ISR_ESTOP_fall();
void ISR_REVS_fall();
void ISR_RUDD_fall();


/***********************************************
 *************** SETUP FUNCTIONS ***************
 ***********************************************/
void setup()
{
    if (DEBUG)
    {
        Serial3.begin(UART_BAUDRATE);
        while (!Serial3)
        {
            // wait until Serial3 initialize
        }
        Serial3.println("** RecX Init at " + String(UART_BAUDRATE));
        Serial3.println("Setup Complete!");
    }

    // Initialize arrays to hold data coming from the RC
    receiverData.newData = false;
    for (int i = 0; i < RC_NUM_SIGNALS; i++)
    {
        RC_rise[i] = INVALID_DATA;
        RC_elapsed[i] = INVALID_DATA;
        RC_Done[i] = 0;
    }

    // Initialize attachInterrupt functions to take care of changes from the RC-controller (similar to Async)
    attachInterrupt(digitalPinToInterrupt(IRPT_AUTO), ISR_AUTO_rise, RISING);
    attachInterrupt(digitalPinToInterrupt(IRPT_REVS), ISR_REVS_rise, RISING);
    attachInterrupt(digitalPinToInterrupt(IRPT_TURN), ISR_TURN_rise, RISING);
    attachInterrupt(digitalPinToInterrupt(IRPT_ESTOP), ISR_ESTOP_rise, RISING);
    attachInterrupt(digitalPinToInterrupt(IRPT_THROTTLE), ISR_GO_rise, RISING);
    // attachInterrupt(digitalPinToInterrupt(IRPT_RUDD), ISR_RUDD_rise, RISING); // <---- Commented out if needed to use this pin

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

/***********************************************
 *************** LOOP FUNCTIONS ****************
 ***********************************************/
void loop()
{
    // check for new data
    waitInputPWM();

    // NOTE: This is temporary and should be used somewhere wherever the data is consumed
    // (data transmitted from this RC-receiver to other Board, processed there and transmitted back here.
    // This is the process the DEBUG below is trying to simulate)
    if (DEBUG)
    {
        if (receiverData.newData == true)
        {
            if (receiverData.ebrakeOn == true)
            {
                rc_ebrake = 1;
            }
            else
            {
                rc_ebrake = 0;
            }

            if (receiverData.autoOn == true)
            {
                rc_auto = 1;
            }
            else
            {
                rc_auto = 0;
            }

            if (receiverData.reverse == true)
            {
                rc_reverse = 1;
            }
            else
            {
                rc_reverse = 0;
            }
            rc_throttle = receiverData.throttle;
            rc_turn = receiverData.turnAngle;
            debugPacket(); // Print out to the screen

            // NOTE: THIS IS IMPORTANT LINE BELOW, SHOULD NOT BE UNDER DEBUG!!!! NEED TO BE USED IN CONSUMER CODE!!!!
            // This is where the Receiver gets the ACK signal from the other boards that used this data and response back through CAN BUS Protocal
            receiveFromCanBus();
            receiverData.newData = false;
        }
    }
}

/***********************************************
 *************** HELPER FUNCTIONS **************
 ***********************************************/

// Wait until the data comes back from the attachInterrupt().
// This function is going to process all the data gather and parse it into a struct: receiverData
void waitInputPWM()
{
    // NOTE: Data received from the RC will be fluctuated from 1000 to 1800.
    // By having threshold, we know when the switches are flipped
    const int THRESHOLD = 1500;
    const int REVS_THRESHOLD = 2090;
    if (RC_Done[RC_ESTP] == true)
    {
        receiverData.ebrakeOn = (RC_elapsed[RC_ESTP] > THRESHOLD ? true : false);
        receiverData.newData = true;
        // Reset everything to pick up new pulses
        RC_Done[RC_ESTP] = false;
    }

    if (RC_Done[RC_AUTO] == true)
    {
        receiverData.autoOn = (RC_elapsed[RC_AUTO] > THRESHOLD ? true : false);
        receiverData.newData = true;
        // Reset everything to pick up new pulses
        RC_Done[RC_AUTO] = false;
    }

    if (RC_Done[RC_THROTTLE] == true)
    {
        receiverData.throttle = RC_elapsed[RC_THROTTLE];
        receiverData.newData = true;
        // Reset everything to pick up new pulses
        RC_Done[RC_THROTTLE] = false;
    }

    if (RC_Done[RC_TURN] == true)
    {
        receiverData.turnAngle = RC_elapsed[RC_TURN];
        receiverData.newData = true;
        // Reset everything to pick up new pulses
        RC_Done[RC_TURN] = false;
    }

    if (RC_Done[RC_REVS] == true)
    {
        receiverData.reverse = (RC_elapsed[RC_REVS] >= REVS_THRESHOLD ? true : false);
        receiverData.newData = true;
        // Reset everything to pick up new pulses
        RC_Done[RC_REVS] = false;
    }

    sendToCanBus(receiverData); // Send current data formatted to CAN BUS protocol
}

void debugPacket()
{
    if (DEBUG)
    {
        logger = "THRO: " + String(rc_throttle);
        logger += " -- TURN: " + String(rc_turn);
        logger += " -- AUTO: " + String(rc_auto);
        logger += " -- E-BRAKE: " + String(rc_ebrake);
        logger += " -- REVERSE: " + String(rc_reverse);
        Serial3.println();
        Serial3.println(logger);
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

/***********************************************
 ************** RISING INTERRUPTS **************
 ***********************************************/
void ISR_TURN_rise()
{
    noInterrupts();
    ProcessRiseOfINT(RC_TURN);
    attachInterrupt(digitalPinToInterrupt(IRPT_TURN), ISR_TURN_fall, FALLING);
    interrupts();
}

void ISR_AUTO_rise()
{
    noInterrupts();
    ProcessRiseOfINT(RC_AUTO);
    attachInterrupt(digitalPinToInterrupt(IRPT_AUTO), ISR_AUTO_fall, FALLING);
    interrupts();
}

void ISR_ESTOP_rise()
{
    noInterrupts();
    ProcessRiseOfINT(RC_ESTP);
    attachInterrupt(digitalPinToInterrupt(IRPT_ESTOP), ISR_ESTOP_fall, FALLING);
    interrupts();
}

void ISR_GO_rise()
{
    noInterrupts();
    ProcessRiseOfINT(RC_THROTTLE);
    attachInterrupt(digitalPinToInterrupt(IRPT_THROTTLE), ISR_GO_fall, FALLING);
    interrupts();
}

void ISR_REVS_rise()
{
    noInterrupts();
    ProcessRiseOfINT(RC_REVS);
    attachInterrupt(digitalPinToInterrupt(IRPT_REVS), ISR_REVS_fall, FALLING);
    interrupts();
}

void ISR_RUDD_rise()
{
    noInterrupts();
    ProcessRiseOfINT(RC_RUDD);
    attachInterrupt(digitalPinToInterrupt(IRPT_RUDD), ISR_RUDD_fall, FALLING);
    interrupts();
}

/***********************************************
 ************** FALLING INTERRUPTS *************
 ***********************************************/
void ISR_GO_fall()
{
    noInterrupts();
    ProcessFallOfINT(RC_THROTTLE);
    RC_Done[RC_THROTTLE] = 1;
    attachInterrupt(digitalPinToInterrupt(IRPT_THROTTLE), ISR_GO_rise, RISING);
    interrupts();
}

void ISR_TURN_fall()
{
    noInterrupts();
    ProcessFallOfINT(RC_TURN);
    RC_Done[RC_TURN] = 1;
    attachInterrupt(digitalPinToInterrupt(IRPT_TURN), ISR_TURN_rise, RISING);
    interrupts();
}

void ISR_AUTO_fall()
{
    noInterrupts();
    ProcessFallOfINT(RC_AUTO);
    RC_Done[RC_AUTO] = 1;
    attachInterrupt(digitalPinToInterrupt(IRPT_AUTO), ISR_AUTO_rise, RISING);
    interrupts();
}

void ISR_ESTOP_fall()
{
    noInterrupts();
    ProcessFallOfINT(RC_ESTP);
    RC_Done[RC_ESTP] = 1;
    attachInterrupt(digitalPinToInterrupt(IRPT_ESTOP), ISR_ESTOP_rise, RISING);
    interrupts();
}

void ISR_REVS_fall()
{
    noInterrupts();
    ProcessFallOfINT(RC_REVS);
    RC_Done[RC_REVS] = 1;
    attachInterrupt(digitalPinToInterrupt(IRPT_REVS), ISR_REVS_rise, RISING);
    interrupts();
}

void ISR_RUDD_fall()
{
    noInterrupts();
    ProcessFallOfINT(RC_RUDD);
    RC_Done[RC_RUDD] = 1;
    attachInterrupt(digitalPinToInterrupt(IRPT_REVS), ISR_RUDD_rise, RISING);
    interrupts();
}