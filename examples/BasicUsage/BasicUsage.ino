// ---------------------------------------------------------------------------
// Basic usage example for the EB-SAC library
// This example demonstrates how to use the EB-SAC library to communicate
// with the EB-SAC module.

#include "EB-SAC.h"

EBSAC ebsac;
MCP251XFD *canfd;
SPISettings spi_settings(10000000, MSBFIRST, SPI_MODE0); // 10MHz

void setup()
{
    // Initialize EB-SAC.
    ebsac.begin(PORT_EBSAC_PWR_D25, PORT_EBSAC_CS_D19); 

    // EB-SAC uses SPI5 on Spresense. NOT USE SPI/SPI1.
    // EB-SAC does not initialize SPI. The user must initialize SPI5, because other add-on boards may also use SPI5.
    SPI5.begin();// must be use SPI5 for Spresense EB-SAC add-on board.

    // Initialize MCP251XFD CAN FD controller via EB-SAC
    canfd = new MCP251XFD( spi_settings, ebsac.getCSPort() ); // Initialize CAN FD controller

    // Misc setup
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    Serial.begin(115200, SERIAL_8N1); // PC <--> Spresene
    Serial.println("EB-SAC BasicUsage.");
    
}

void loop()
{
    const uint16_t maxlen = 8;
    uint8_t res[maxlen];
    uint16_t len = 0;

    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);

    // Menu for user input
    Serial.println("\033[32m");
    Serial.println("  1: Reset MCP251XFD.");
    Serial.println("  0: exit.");
    Serial.print("Press a key to select command: ");
    Serial.print("\033[0m");

    // Wait for user input
    while (Serial.available() == 0) {
        delay(1); // for CPU load reduction
    }

    // Read user input
    digitalWrite(LED1, HIGH);
    char c = Serial.read();
    Serial.println(c);
    switch (c)
    {
        case '1':
            // Reset MCP251XFD
            Serial.println("Resetting MCP251XFD...");
            canfd->Reset();
            break;
        case '0':
            ebsac.end();
            Serial.println("Bye...");
            exit(0);
            break;

        default:
            // Not implemented or unsupported command
            Serial.println("Command not implemented or unsupported.");
            break;
    }


    // Turn off LED after operation
    delay(500);
    Serial.println("");
    digitalWrite(LED1, LOW);

}
