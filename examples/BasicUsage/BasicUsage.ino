// ---------------------------------------------------------------------------
// Basic usage example for the EB-SAC library
// This example demonstrates how to use the EB-SAC library to communicate
// with the EB-SAC module.

#include "EB-SAC.h"

EBSAC ebsac;
MCP251XFD *canfd;
SPISettings spi_settings(10000000, MSBFIRST, SPI_MODE0); // 10MHz

// MCP251XFD configration data
const uint8_t osc[]      = { 0x00, 0x00, 0x00, 0x00 }; // OSC: 40MHz, no PLL
const uint8_t nbtcfg[]   = { 0x0F, 0x0F, 0x3E, 0x00 }; // CiNBTCFG: SJW=16, TSEG1=63, TSEG2=16, BRP=1
const uint8_t dbtcfg[]   = { 0x03, 0x03, 0x0E, 0x00 }; // CiDBTCFG: SJW=4, TSEG1=4, TSEG2=15, BRP=1
const uint8_t tdc[]      = { 0x00, 0x0F, 0x02, 0x00 }; // CiTDC: TDCV=0, TDCO=15, TDCMOD=2

// FIFO 1
const uint8_t fifocon1[] = { 0x00, 0x04, 0x60, 0xE7 }; // CiFIFOCON1: RxFIFO, clear, retry, 64byte, 8 Message deep
const uint8_t filtcon0[] = { 0x81, 0x00, 0x00, 0x00 }; // CiFLTCON0: Filter enable, Filter0 to store in FIFO1

// FIFO 2
const uint8_t fifocon2[] = { 0x80, 0x04, 0x60, 0xE3 }; // CiFIFOCON2: TxFIFO, clear, retry, 64byte, 4 Message deep
const uint8_t c1con[]    = { 0x60, 0x07, 0x00, 0x00 }; // CiCON: STEF=0, TXQEN=0, REQOP=000( Set Normal CAN FD mode)


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
    const uint16_t maxlen = 64;
    uint8_t res[maxlen];
    char buf[maxlen];
    uint16_t len = 0;

    REG_CiTEFSTA tefsta2;
    REG_CiFIFOUA txfifoua2;
    CANFRAME txframe;

    REG_CiFIFOSTA rxifosta1;
    REG_CiFIFOUA rxfifoua1;
    CANFRAME rxframe;

    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);

    // Menu for user input
    Serial.println("\033[32m");
    Serial.println("  1: Reset MCP251XFD.");
    Serial.println("  2: Configration MCP251XFD.");
    Serial.println("  3: Send CAN FD message.");
    Serial.println("  4: Receive CAN FD message.");
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
        case '2':
            // Configration MCP251XFD
            Serial.println("Configration MCP251XFD...");
            canfd->writeRegister(cREGADDR_OSC, osc, 4);
            canfd->writeRegister(cREGADDR_CiNBTCFG, nbtcfg, 4);
            canfd->writeRegister(cREGADDR_CiDBTCFG, dbtcfg, 4);
            canfd->writeRegister(cREGADDR_CiTDC, tdc, 4);
            canfd->writeRegister(cREGADDR_CiFIFOCON + (1*CiFIFO_OFFSET), fifocon1, 4);
            canfd->writeRegister(cREGADDR_CiFLTCON + (0*CiFILTER_OFFSET), filtcon0, 4);
            canfd->writeRegister(cREGADDR_CiFIFOCON + (2*CiFIFO_OFFSET), fifocon2, 4);
            canfd->writeRegister(cREGADDR_CiCON, c1con, 4);

            // Read back and display
            canfd->readRegister(cREGADDR_OSC, res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_OSC, res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiNBTCFG, res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiNBTCFG, res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiDBTCFG, res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiDBTCFG, res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiTDC, res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiTDC, res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiFIFOCON + (1*CiFIFO_OFFSET), res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiFIFOCON + (1*CiFIFO_OFFSET), res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiFLTCON + (0*CiFILTER_OFFSET), res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiFLTCON + (0*CiFILTER_OFFSET), res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiFIFOCON + (2*CiFIFO_OFFSET), res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiFIFOCON + (2*CiFIFO_OFFSET), res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            canfd->readRegister(cREGADDR_CiCON, res, 4);
            sprintf(buf,"%04X: %02X %02X %02X %02X ", cREGADDR_CiCON, res[0], res[1], res[2], res[3]);
            Serial.println(buf);
            break;

        case '3':
            // Send CAN FD message
            Serial.println("Send CAN FD message...");
            for(uint8_t lp = 0; lp < 4; lp++)
            {
                // FIFO buffer status check
                canfd->readRegister(cREGADDR_CiFIFOSTA + (2*CiFIFO_OFFSET), (uint8_t *)(&tefsta2), 4);
                if(tefsta2.bF.TEFNotEmptyIF == 0)
                {
                    Serial.println("TEF Not Empty, cannot send message.");
                    sprintf(buf, "FIFOSTA2: %08X ", tefsta2.word);
                    Serial.println(buf);
                    delay(1000);
                    break;
                }
                // Get Tx FIFO user address
                canfd->readRegister(cREGADDR_CiFIFOUA + (2*CiFIFO_OFFSET), (uint8_t *)(&txfifoua2), 4);
                sprintf(buf, "Stored Tx FIFO Address: %03X ", txfifoua2.bF.UserAddress);
                Serial.println(buf);

                // Prepare CAN FD message
                txframe.id = 0x123; // Standard ID
                txframe.fdf = 1;    // CAN FD
                txframe.brs = 0;    // BRS enable(1) 2Mbps, BRS disable(0) 500kbps.
                txframe.dlc = 15;   // 64 bytes
                for(uint8_t i=0; i<64; i++) {
                    txframe.data[i] = i;
                }
                // Set transmit message to TX FIFO
                // Please check "TRANSMIT MESSAGE OBJECT (TXQ AND TX FIFO)" in the MCP251XFD Family Data Sheet.
                buf[0] = (uint8_t)(txframe.id);               // ID[7:0]
                buf[1] = (uint8_t)((txframe.id >> 8) & 0x07); // ID[10:8]
                buf[2] = 0x00;
                buf[3] = 0x00;
                canfd->writeRegister(cRAMADDR_START + txfifoua2.bF.UserAddress + 0, buf, 4);// must write 4 bytes at once
                buf[0] = (txframe.dlc & 0x0F) | ((txframe.fdf & 0x01) << 7) | ((txframe.brs & 0x01) << 6); // DLC, FDF, BRS
                buf[1] = 0x00;
                canfd->writeRegister(cRAMADDR_START + txfifoua2.bF.UserAddress + 4, buf, 4);// must write 4 bytes at once
                canfd->writeRegister(cRAMADDR_START + txfifoua2.bF.UserAddress + 8, txframe.data, 64);

                // Request to send
                canfd->writeRegister(cREGADDR_CiFIFOCON + (2*CiFIFO_OFFSET) + 1, 0x03); // Set TXREQ, UINC bit

                // Wait until message is sent
                delay(1000);
            }
            break;

        case '4':
            // Receive CAN FD message
            Serial.println("Receive CAN FD message...");
            // Check RX FIFO status
            canfd->readRegister(cREGADDR_CiFIFOSTA + (1*CiFIFO_OFFSET), (uint8_t *)(&rxifosta1), 4);
            if(rxifosta1.rxBF.RxNotEmptyIF == 0)
            {
                Serial.println("No message in RX FIFO.");
                sprintf(buf, "FIFOSTA1: %08X ", rxifosta1.word);
                Serial.println(buf);
                break;
            }

            // Get RX FIFO user address
            canfd->readRegister(cREGADDR_CiFIFOUA + (1*CiFIFO_OFFSET), (uint8_t *)(&rxfifoua1), 4);
            sprintf(buf, "Stored RX FIFO Address: %03X ", rxfifoua1.bF.UserAddress);
            Serial.println(buf);

            // Read received message from RX FIFO
            canfd->readRegister(cRAMADDR_START + rxfifoua1.bF.UserAddress, res, 4);
            rxframe.id = res[1] << 8 | res[0];
            canfd->readRegister(cRAMADDR_START + rxfifoua1.bF.UserAddress + 4, res, 2);
            rxframe.dlc = res[0] & 0x0F;
            rxframe.fdf = (res[0] >> 7) & 0x01;
            rxframe.brs = (res[0] >> 6) & 0x01;
            canfd->readRegister(cRAMADDR_START + rxfifoua1.bF.UserAddress + 8, rxframe.data, DLCLENGTH[rxframe.dlc]); // receive size by DLCLENGTH table

            // Release RX FIFO
            canfd->writeRegister(cREGADDR_CiFIFOCON + (1*CiFIFO_OFFSET) + 1, 0x01); // Set UINC bit
            
            // Display received message
            sprintf(buf, "Received ID: %03X DLC: %d FDF: %d BRS: %d", rxframe.id, rxframe.dlc, rxframe.fdf, rxframe.brs);
            Serial.println(buf);
            Serial.print("Message: ");
            for(uint8_t i=0; i < DLCLENGTH[rxframe.dlc]; i++)
            {
                sprintf(buf, "%02X ", rxframe.data[i]);
                Serial.print(buf);
            }
            Serial.println("");
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
