void setup()
{
    Serial1.begin(9600); // Default communication rate of the Bluetooth module
    SerialUSB.begin(9600);
    while (!SerialUSB)
    {
        ;
    }
    SerialUSB.println("TX --- Setup Completed");
}
void loop()
{
    if (Serial1.available() > 0)
    {
        // Checks whether data is comming from the serial port
        int state = Serial1.read(); // Reads the data from the serial port
        String mess = "Received: " + state;
        SerialUSB.println(mess);
    }

    // sending data
    Serial1.write(1025);
    Serial1.write(66);
    Serial1.write(67);
    SerialUSB.println(1025);
    delay(10);
}

/**
 *  Currently Working with the TX-TEST 
 */