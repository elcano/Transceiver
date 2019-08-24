#define UART_BAUDRATE 115200
#define CAN_PIN 3
#define DEBUG_EN 1

// MAPPING pins of the RC-receiver - SAMD21
#define AUX_PIN 7      // AUX  - D7
#define GEAR_PIN 8     // GEAR - D8
#define RUDD_PIN 0     // RUDD - D0
#define ELEV_PIN 1     // ELEV - D1
#define AILE_PIN 6     // AILE - D6
#define THROTTLE_PIN 9 // THRO - D9

#define DEBUG 1 // Prints debugging info to serialUSB, can impact loop time                         \
                // WARNING: when true (!0), you must connect USB to allow hardware reset, otherwise \
                // SAMD21 Arduino will do nothing*/

#define TX_LED_LINK 6 // LED pins on the transmitter
#define NO_PACKAGE -1 // LED pins on the transmitter

// #define ProcessFallOfINT(Index) RC_elapsed[Index] = (micros() - RC_rise[Index])
// #define ProcessRiseOfINT(Index) RC_rise[Index] = micros()
#define ProcessFallOfINT(Index) RC_elapsed[Index] = (micros() - RC_rise[Index])
#define ProcessRiseOfINT(Index) RC_rise[Index] = micros()
#define INVALID_DATA 0L  // This is a value that the RC controller can't produce.
#define RC_NUM_SIGNALS 7 // How many RC signals we receive (USED TO BE 7)

// ----- IMPORTED FROM THE SettingsTemplate.h ---------
#define RC_TURN 1
#define RC_AUTO 2
#define RC_REVS 3
#define RC_ESTP 4
#define RC_THROTTLE 5
#define RC_RUDD 6

// The numbers defined below are SAMD21 Pins
#define IRPT_RUDD 1     // RUDD_PIN     - channel 4
#define IRPT_REVS 0     // ELEV_PIN     - channel 3
#define IRPT_AUTO 9     // AUX_PIN      - channel 6
#define IRPT_ESTOP 6    // GEAR_PIN     - channel 5
#define IRPT_THROTTLE 7 // THROTTLE_PIN - channel 1
#define IRPT_TURN 8     // AILE_PIN     - channel 2
// ----- IMPORTED FROM THE SettingsTemplate.h ---------

// ----- IMPORTED FROM THE Can_Protocal.h (path: elcano/Elcano_C2_lowlevel/Can_Protocal.h)---------
#define RCStatus_CANID 0x50
#define HiStatus_CANID 0x100
#define LowStatus_CANID 0x200
#define RCDrive_CANID 0x300
#define HiDrive_CANID 0x350
#define Actual_CANID 0x500
    // ----- IMPORTED FROM THE Can_Protocal.h (path: elcano/Elcano_C2_lowlevel/Can_Protocal.h)---------

    // 32 bits = 4 bytes
    // 8 bits = 1 byte
    typedef union {
    struct
    {
        uint32_t throttle;
        uint32_t turnAngle;
        uint8_t ebrakeOn;
        uint8_t autoOn;
        uint8_t reverse; // Future Stuff
        uint8_t newData;
    };
    uint8_t bytes[12]; // The length of this byte buffer is dependded on the amount of Bytes in the struct above
} ReceiverData;