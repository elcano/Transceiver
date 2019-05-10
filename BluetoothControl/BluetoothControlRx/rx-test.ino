//int state = 0;
//
//void setup()
//{
//    Serial1.begin(38400); // Default communication rate of the Bluetooth module
//    SerialUSB.begin(38400);
//    while (!SerialUSB)
//    {
//        ;
//    }
//    SerialUSB.println("RX --- Setup Completed");
//}
//void loop()
//{
//    if (Serial1.available() > 0)
//    {                           // Checks whether data is comming from the serial port
//        state = Serial1.read(); // Reads the data from the serial port
//        SerialUSB.print((char)state);
//    }
//}
