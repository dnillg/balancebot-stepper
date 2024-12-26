#ifndef CONFIG_H // include guard
#define CONFIG_H

#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240
#define BUFFER_SIZE 256

// ----------------------------------------------------------------------------
// Bluetooth
// ----------------------------------------------------------------------------

#define BLUETOOTH_NAME "balance-robot"

// ----------------------------------------------------------------------------
// Control Serial
// ----------------------------------------------------------------------------

#define CONTROL_SERIAL_BAUD 921600
#define CONTROL_SERIAL_PORT 2
#define CONTROL_SERIAL_ENABLED true

// ----------------------------------------------------------------------------
// Pins
// ----------------------------------------------------------------------------

#define PIN_DISPLAY_SCL 18
#define PIN_DISPLAY_SDA 23
#define PIN_DISPLAY_DC 2
#define PIN_DISPLAY_CS 5
#define PIN_DISPLAY_RST 4

#define PIN_MP3_SERIAL_RX 22
#define PIN_MP3_SERIAL_TX 19

#define PIN_CARD_CS 21
#define PIN_CARD_MOSI 13
#define PIN_CARD_SCK 14
#define PIN_CARD_MISO 25


#define PIN_SERIAL_RX 17
#define PIN_SERIAL_TX 16

#endif /* CONFIG_H */