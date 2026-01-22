#ifndef _EBSAC_H_
#define _EBSAC_H_

#include <Arduino.h>
#include <SPI.h>
#include "MCP251XFD.h"

#define PORT_EBSAC_PWR_D25   (25) // default power on/off port.
#define PORT_EBSAC_PWR_D26   (26) // if chnaged mounting from R3 to R1 jumper register.
#define PORT_EBSAC_PWR       (PORT_EBSAC_PWR_D25) // power on/off port.
#define PWR_ON_HIGH          (HIGH) // power on level
#define PWR_OFF_LOW          (LOW)  // power off level

#define PORT_EBSAC_CS_D19    (19) // default CS port
#define PORT_EBSAC_CS_D24    (24) // if chnaged mounting from R2 to R5 jumper register.
#define PORT_EBSAC_CS        (PORT_EBSAC_CS_D19) // CS enable port.
#define CS_ENABLE_LOW        (LOW)  // CS enable level
#define CS_DISABLE_HIGH      (HIGH) // CS disable level

#define STARTUP_DELAY_MS     (100)


class EBSAC
{
private:
    uint8_t port_pwr = 0;
	uint8_t port_cs = 0;

public:

    EBSAC()
    {
    }
    ~EBSAC()
    {
        this->end();
    }

    void begin(uint8_t pp = PORT_EBSAC_PWR, uint8_t csp = PORT_EBSAC_CS)
    {
        this->port_pwr = pp;
        this->port_cs = csp;

        //setting power port direction
        pinMode(this->port_pwr, OUTPUT);
        digitalWrite(this->port_pwr, PWR_OFF_LOW);

        //setting cs port direction
        pinMode(this->port_cs, OUTPUT);
        digitalWrite(this->port_cs, CS_DISABLE_HIGH);

        this->powerOn();
        delay(STARTUP_DELAY_MS);
    }
    void end()
    {
        this->powerOff();
    }
    void powerOn()
    {
        digitalWrite(this->port_pwr, PWR_ON_HIGH);
    }
    void powerOff()
    {
        digitalWrite(this->port_pwr, PWR_OFF_LOW);
    }
    int isPower()
    {
        return (digitalRead(this->port_pwr) == PWR_ON_HIGH) ? 1 : 0;
	}
    uint8_t getCSPort()
    {
        return this->port_cs;
    }

};

#endif
