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

// 32 bits = 4 bytes
// 8 bits = 1 byte
typedef union {
    struct
    {
        uint32_t throttle;
        uint32_t turn;
        uint8_t ebrake;
        uint8_t autonomous;
        uint8_t reverse; // Future Stuff
    };
    uint8_t bytes[11]; // The length of this byte buffer is dependded on the struct above
} ReceiverData;
