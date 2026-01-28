#ifndef _MCP251XFD_H_
#define _MCP251XFD_H_
#include <Arduino.h>
#include <SPI.h>
#include "drv_canfdspi_defines.h"
#include "drv_canfdspi_register.h"

#define CS_ENABLE_LOW        (LOW)  // CS enable level
#define CS_DISABLE_HIGH      (HIGH) // CS disable level

// for spresense add-on spi port support
#if defined(ARDUINO_ARCH_SPRESENSE)
#define SPI SPI5
#endif

typedef struct _CANFRAME {
  uint16_t id; // 11bit
  uint8_t fdf; // 0: CAN, 1: CAN FD
  uint8_t brs; // 0: no BRS, 1: BRS
  uint8_t dlc; // Data Length Code
  uint8_t data[64]; // Data field
} CANFRAME;

// DLC -> byte length table (index = dlc & 0x0F)
const uint8_t DLCLENGTH[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64 };


class MCP251XFD
{  
private:
    SPISettings spi_settings;
    uint8_t port_cs = 0;

public:
    MCP251XFD(const SPISettings& spi_settings, uint8_t csp)
    {
        this->port_cs = csp;
        this->spi_settings = spi_settings;
    }
    ~MCP251XFD()
    {

    }
    void setSPISettings(const SPISettings& spi_settings)
    {
        this->spi_settings = spi_settings;
    }
    void readRegister(uint16_t address, uint8_t data)
    {
        this->readRegister(address, &data, 1);
    }
    void readRegister(uint16_t address, uint8_t* data, uint8_t length)
    {
        uint8_t len = length;
        uint8_t frame[2 + len];  // (inst + addr)(2) + data(N)
        frame[0] = (cINSTRUCTION_READ << 4) | ((address >> 8) & 0x3F); // Address high byte
        frame[1] = address & 0xFF; // Address low byte
        memcpy(data, 0, len);

        SPI.beginTransaction(this->spi_settings);
        digitalWrite(this->port_cs, CS_ENABLE_LOW);
        SPI.transfer(frame, 2 + len); // Transfer inst+addr+dummy, receive inst+addr+data
        SPI.endTransaction();
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);
        memcpy(data, frame + 2, len);
    }
    void writeRegister(uint16_t address, uint8_t data)
    {
        this->writeRegister(address, &data, 1);
    }
    void writeRegister(uint16_t address, const uint8_t* data, uint8_t length)
    {
        uint8_t len = length;
        uint8_t frame[2 + len];  // (inst + addr)(2) + data(N)
        frame[0] = (cINSTRUCTION_WRITE << 4) | ((address >> 8) & 0x3F); // Address high byte
        frame[1] = address & 0xFF; // Address low byte
        memcpy(frame + 2, data, len);

        SPI.beginTransaction(this->spi_settings);
        digitalWrite(this->port_cs, CS_ENABLE_LOW);
        SPI.transfer(frame, 2 + len); // Transfer inst+addr+dummy, receive inst+addr+data
        SPI.endTransaction();
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);
    }
    void Reset()
    {
        SPI.beginTransaction(this->spi_settings);
        digitalWrite(this->port_cs, CS_ENABLE_LOW);
        SPI.transfer16((cINSTRUCTION_RESET << 12)); // RESET instruction
        SPI.endTransaction();
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);
    }

};
#undef SPI
#endif //_MCP251XFD_H_
