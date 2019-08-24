#include <RH_RF69.h>        // External; documented at http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF69.html
#include <SPI.h>
#include "RadioControl_rf69.h"

#include <mcp_can_dfs.h>    // <---- Import from another library: Seeed-Studio/CAN-BUS-Shield
#include <mcp_can.h>        //       (install Library from Arduino, search for "Can-Bus-Shield")

#define TRANSMITTER 1       // set false to compile receiver code
#define DEBUG 1             /*prints debugging info to serialUSB, can impact loop time
                            WARNING: when true (!0), you must connect USB to allow hardware reset, otherwise
                            SAMD21 Arduino will do nothing*/

MCP_CAN CAN(3);            // chip selection pin for CAN. 53 for mega, 49 for our new low level board
RH_RF69 driver(SS_PIN, INTERRUPT_PIN);

DataFromTransmitter txData; // sent from transmitter
unsigned char lenTx = sizeof(txData);
DataFromReceiver rxData;    // sent from receiver
unsigned char lenRx = sizeof(rxData);

const unsigned long BACKOFF = 20; // backoff controls maximum waiting time for a reply and ultimately maximum packets / second
unsigned long timeSent = 0UL;
unsigned long timeRecv = 0UL;

void setup()
{
  // configure pins
  pinMode(TURN_PIN, INPUT);
  pinMode(THROTTLE_PIN, INPUT);
  pinMode(AUTO_PIN, INPUT_PULLUP);  /* internal pullup keeps values from floating */
  pinMode(EBRAKE_PIN, INPUT_PULLUP);
  pinMode(TX_LED_LINK, OUTPUT);
  analogReadResolution(12);
  // SerialUSB connects to serial monitor or tty
  SerialUSB.begin(UART_BAUDRATE);
  if (DEBUG)
    while (!SerialUSB)
    {
      ;
    }

  if (!driver.init())
  {
    if (DEBUG)
      SerialUSB.println("init failed");
  }
  else
  {
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    driver.setFrequency(RF69_FREQ_MHZ);
    driver.setTxPower(20, true);
    driver.setEncryptionKey(key);
  }
  
  if (DEBUG)
  {
    SerialUSB.println("Setup Complete: ");
    if (TRANSMITTER)
    {
      SerialUSB.println("transmitter mode");
    }
    else
    {
      SerialUSB.println("receiver mode");
    }
  }

  // Inital CAN bus with 500KBPS baud rate (CAN_500KBPS is the baud rate)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
      if (DEBUG)
      {
        SerialUSB.println("CAN BUS Shield init fail");
        SerialUSB.println(" Init CAN BUS Shield again");
        delay(100);
      }
    }
    if (DEBUG)
    {
     SerialUSB.println("CAN BUS Shield init OK!");
    }
}

void loop()
{
#if TRANSMITTER
  txloop();
#else
  rxloop();
#endif
}

void readInputs()
{
  txData.turn = analogRead(TURN_PIN);
  txData.throttle = analogRead(THROTTLE_PIN);
  txData.autonomous = digitalRead(AUTO_PIN);
  txData.ebrake = digitalRead(EBRAKE_PIN);
}

static bool received = false;
void setLights()
{
  digitalWrite(TX_LED_LINK, received);
}

void txloop()
{
  readInputs();
  // Send a message to receiver unit
  driver.send((unsigned char *)(&txData), lenTx);
  driver.waitPacketSent();
  timeSent = millis();
  // Now wait for a reply
  if (driver.waitAvailableTimeout(BACKOFF))
  {
    // we have a reply
    if (driver.recv((unsigned char *)(&rxData), &lenRx))
    {
      // payload is the right size
      received = true;
      txData.rssi = driver.rssiRead();
      setLights();
      timeRecv = millis();
      if (DEBUG)
      {
        SerialUSB.println(String(rxData.rssi / 2.0));
      }

      unsigned long interval = timeRecv - timeSent;
      unsigned long msDelay = BACKOFF - interval;
      if (msDelay < BACKOFF)
      {
        delay(msDelay);
      }
      else
      {
        delay(BACKOFF);
      }
    }

    else
    {
      // payload is not the right size
      received = false;
      setLights();
      if (DEBUG)
        SerialUSB.println("!");
    }
  }
  else
  {
    // we did not get a reply
    received = false;
    setLights();
    if (DEBUG)
      SerialUSB.println("?");
  }
}

void rxloop()
{
  if (driver.available())
  {
    // a packet is availible
    if (driver.recv((unsigned char *)(&txData), &lenTx))
    {
      // payload is the right size
      if (timeRecv > 0)
        timeSent = timeRecv;
      timeRecv = millis();
      rxData.rssi = driver.rssiRead();
      driver.send((unsigned char *)(&rxData), lenRx); // packet sent to transmitter
      driver.waitPacketSent();

      if (DEBUG) 
      {
        SerialUSB.println("txData: - Throttle: " + String(txData.throttle) + " - Turn: " + String(txData.turn));
      }
      // sendToCanBus(txData); // Send the whole data struct to Can Bus
      // receiveFromCanBus();
      
      if (DEBUG)
      {
        printTx(false);
        unsigned long duration = timeRecv - timeSent;
        SerialUSB.println("," + String(duration));
      }
    }
    else
    {
      // payload was the wrong size
      if (DEBUG)
        SerialUSB.println("!");
    }
  }
}

void printTx(bool newline)
{
  // output formatted as CSV for ease of review/logging
  String packet = String(rxData.rssi / 2.0) 
        + "," + String(txData.throttle) 
        + "," + String(txData.turn) 
        + ",";

  if (txData.ebrake)
  {
    packet += "E";
  }
  packet += ",";

  if (txData.autonomous)
  {
    packet += "A";
  }
  packet += ",";

  if (newline)
  {
        if (DEBUG) {
        SerialUSB.println(packet);
      }
    }
  else
  {
        if (DEBUG) {
        SerialUSB.print(packet);
      }
    }
}


//// Process the data over the CAN BUS protocal
//// NOTE: This function is not tested yet...
//void sendToCanBus(DataFromTransmitter message)
//{
//    if (DEBUG)
//    {
//        SerialUSB.println("Sending data to CAN BUS...");
//    }
//
//    // send CAN message to CAN BUS
//    CAN.sendMsgBuf(Actual_CANID, 0, 8, (uint8_t *)&message);
//    delay(1000); // a proper delay here is necessay, CAN bus need a time to clear the buffer. delay could be 100 minimum
//
//    if (DEBUG)
//    {
//        SerialUSB.println("Messages SENT!");
//    }
//}
//
//// Receive data from the other high/low level board
//// NOTE: This function is not tested yet...
//void receiveFromCanBus()
//{
//    unsigned char len = 0;      // message length
//    unsigned char msgBuffer[8]; //8 Bytes buffer to store CAN message
//    unsigned int canID = 0;
//
//    // Check if received anything
//    if (CAN_MSGAVAIL == CAN.checkReceive())
//    {
//        CAN.readMsgBuf(&len, msgBuffer); // put the data read into buffer and length
//        canID = CAN.getCanId();
//
//        if (canID = HiDrive_CANID) // <--- Change the CAN ID according to where this receiver transmitted the CAN message to.
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("HiDrive_CANID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//        else if (canID == HiStatus_CANID)
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("HiStatus_CANID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//        else if (canID == RCStatus_CANID)
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("RCStatus_CANID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//        else if (canID == LowStatus_CANID)
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("LowStatus_CANID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//        else if (canID == RCDrive_CANID)
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("RCDrive_CANID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//        else if (canID == Actual_CANID)
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("Actual_CANID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//        else
//        {
//            if (DEBUG)
//            {
//                SerialUSB.print("Unexpected CAN ID received: ");
//                SerialUSB.println(canID, HEX);
//            }
//        }
//
//        // Process incoming data (This step should be in the if/else statement above according to each if/else...)
//        int resultFromCanBUS = (unsigned int)(msgBuffer[3] << 24) | (msgBuffer[2] << 16) | (msgBuffer[1] << 8) | (msgBuffer[0]);
//        if (DEBUG)
//        {
//            SerialUSB.print("ACK msg from CAN BUS: ");
//            SerialUSB.println(resultFromCanBUS, DEC);
//            SerialUSB.println("Message received from the CAN BUS! Finished...");
//        }
//    }
//}