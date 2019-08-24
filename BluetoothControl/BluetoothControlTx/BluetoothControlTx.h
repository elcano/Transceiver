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
} TransmitterData;
