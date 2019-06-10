#ifndef SERVO_H
#define SERVO_H

#define UART_BAUDRATE 38400

// transmitter input pins
#define TURN_PIN A3     // joystick 2 L/R
#define THROTTLE_PIN 7 // joystick 1 U/D
#define INTERRUPT_PIN 2
#define AUTO_PIN 5
#define EBRAKE_PIN 4
#define REVERSE_PIN 20
#define DEBUG 1       // Prints debugging info to serialUSB, can impact loop time                         
                      // WARNING: when true (!0), you must connect USB to allow hardware reset, otherwise 
                      // SAMD21 Arduino will do nothing*/

#define TX_LED_LINK 6 // LED pins on the transmitter
#define NO_PACKAGE -1 // LED pins on the transmitter

// other outputs
#define SS_PIN 10
/* pins 11 - 13 are reserved for SPI */

// The encryption key has to be the same for transmitter and receiver
uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};

typedef struct
{
    unsigned int throttle;
    unsigned int turn;
    bool ebrake;
    bool autonomous;
    bool reverse; // Future Stuff
} receiverData;

#endif
