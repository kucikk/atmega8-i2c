/*
 * i2c.cpp
 *
 * Created: 4. 1. 2015 21:20:59
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */

#define F_CPU 1000000UL//1MHz - ATMega8 default
#define SCL_CLOCK 50000UL//I2C clock 50kHz

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "twi/twi.h"

#define LM75A_ADDR 0b01001000//7bitů!
#define LEDBAR_ADDR 0b0100000

#define STATUS_LED PB1
#define STATUS_LED_DDR DDRB
#define STATUS_LED_PORT PORTB

#define ERROR 1
#define SUCCESS 0

/**
 * Pokusí se přečíst teplotu z čidla
 */
uint8_t getTemp (uint8_t *temp_hi, uint8_t *temp_lo) {
	
	twi::start();// S - Start
	if (!twi::statusIs(TW_START))// je I2C nastartováno?
		return 1;
	
	twi::write(LM75A_ADDR << 1);// pošle SLA+W (slave address + write bit = poslední bit je nula)
	if (!twi::statusIs(TW_MT_SLA_ACK))
		return 2;
	
	twi::write(0x00);// Data - nastavení pointeru na 0x00 = čtení teploty
	if (!twi::statusIs(TW_MT_DATA_ACK))
		return 3;
	
	twi::start();// Rs - Repeated start
	if (!twi::statusIs(TW_REP_START))
		return 4;
	
	twi::write((LM75A_ADDR << 1) | 1);// pošle SLA+R  (slave address + read bit = poslední bit je nula)
	if (!twi::statusIs(TW_MR_SLA_ACK))
		return 5;
	
	*temp_hi = twi::readACK();// načte MSB (důležitější bajt) - celá část teploty = poměrně zajímavé číslo
	if (!twi::statusIs(TW_MR_DATA_ACK))
		return 6;
	
	*temp_lo = twi::readNACK();//načte LSB (méně důležitý bajt) - desetinná část teploty
	if (!twi::statusIs(TW_MR_DATA_NACK))
		return 7;
	
	twi::stop();
	
	return SUCCESS;
}


/**
 * Komunikace s LEDBarem
 */
uint8_t sendToLEDBar (uint8_t data) {
	
	twi::start();//S -start
	if (!twi::statusIs(TW_START))
		return 1;
	
	twi::write(LEDBAR_ADDR << 1);// pošle SLA+W (slave address + write bit)
	if (!twi::statusIs(TW_MT_SLA_ACK))
		return 2;
	
	twi::write(data);// pošle data k zobrazení
	if (!twi::statusIs(TW_MT_DATA_NACK))
		return 3;
	
	twi::stop();
	
	return SUCCESS;
}


/**
 * Nastaví piny pro ledky jako výstup
 */
void LEDInit (void) {
	STATUS_LED_DDR |= (1 << STATUS_LED);
	STATUS_LED_PORT &= ~(1 << STATUS_LED);
}

/**
 * Postupně zosvítí a zhasne všechny ledky
 */
void LEDTest (void) {
	STATUS_LED_PORT |= 1 << STATUS_LED;
	_delay_ms(500);
	STATUS_LED_PORT &= ~(1 << STATUS_LED);
}


int main(void) {
	uint8_t temp_hi = 0;
	uint8_t temp_lo = 0;
	uint8_t status;
	
	LEDInit();
	LEDTest();
	
	twi::init(F_CPU, SCL_CLOCK);
	
	while(1) {
		
		// získání dat z teplotního čidla
		status = getTemp(&temp_hi, &temp_lo);
		if (status) {
			twi::stop();
			for (uint8_t i = 0; i < status; i++) {
				STATUS_LED_PORT |= (1 << STATUS_LED);
				_delay_ms(250);
				
				STATUS_LED_PORT &= ~(1 << STATUS_LED);
				_delay_ms(250);
			}
			continue;
		}
		
		
		// příprava dat pro ledbar
		uint8_t data = 0x00;
		
		if (temp_hi >= 20) {
			switch (temp_hi - 20) {
				case 0x00: data = 0b00000000; break;
				case 0x01: data = 0b00000001; break;
				case 0x02: data = 0b00000011; break;
				case 0x03: data = 0b00000111; break;
				case 0x04: data = 0b00001111; break;
				case 0x05: data = 0b00011111; break;
				case 0x06: data = 0b00111111; break;
				case 0x07: data = 0b01111111; break;
				default:   data = 0b11111111; break;
			}
		}
		
		// poslání dat na ledbar
		status = sendToLEDBar(data);
		if (status) {
			twi::stop();
			for (uint8_t i = 0; i < status; i++) {
				STATUS_LED_PORT |= (1 << STATUS_LED);
				_delay_ms(250);
				
				STATUS_LED_PORT &= ~(1 << STATUS_LED);
				_delay_ms(250);
			}
		}
    }
}
