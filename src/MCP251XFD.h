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

struct CANFrame {
  uint32_t id;
  uint8_t data[64];
  uint8_t len;
  bool isFD;
  bool isExtended;
};

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
    void readRegister(uint16_t address, uint8_t* data)
    {
        this->readRegister(address, data, 1);
    }
    void readRegister(uint16_t address, uint8_t* data, uint8_t length)
    {
        uint8_t len = length;
        SPI.beginTransaction(this->spi_settings);
        digitalWrite(this->port_cs, CS_ENABLE_LOW);
        SPI.transfer((cINSTRUCTION_READ << 12) | (address >> 8) & 0x3F); // Address high byte
        SPI.transfer(address & 0xFF); // Address low byte
        while (len--) {
            *data++ = SPI.transfer(0x00); // Dummy byte to read data
        }
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);
        SPI.endTransaction();
    }
    void writeRegister(uint16_t address, uint8_t data)
    {
        this->writeRegister(address, &data, 1);
    }
    void writeRegister(uint16_t address, const uint8_t* data, uint8_t length)
    {
        uint8_t len = length;
        SPI.beginTransaction(this->spi_settings);
        digitalWrite(this->port_cs, CS_ENABLE_LOW);
        SPI.transfer((cINSTRUCTION_WRITE << 12) | (address >> 8) & 0x3F); // Address high byte
        SPI.transfer(address & 0xFF); // Address low byte
        while (len--) {
            SPI.transfer(*data++); // Write data byte
        }
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);
        SPI.endTransaction();
    }
    void Reset()
    {
        SPI.beginTransaction(this->spi_settings);
        digitalWrite(this->port_cs, CS_ENABLE_LOW);
        SPI.transfer((cINSTRUCTION_RESET << 12)); // RESET instruction
        SPI.transfer(0x00);
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);
        SPI.endTransaction();
    }

};
#endif //_MCP251XFD_H_
