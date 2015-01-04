/*
 * twi.c
 *
 * Created: 22. 11. 2014 10:09:26
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */
#include <avr/io.h>
#include <util/twi.h>
#include "twi.h"

/**
 * Inicializace I2C rozhraní. Volá se pouze jednou.
 * @param cpu frekvence čipu
 * @param clock frekvence rozhranní
 */
void twi::init (uint32_t cpu, uint32_t clock) {
	// set SCL to 400kHz
	TWSR = 0x00;
	TWBR = ((cpu/clock)-16)/2;
	// enable TWI
	TWCR = (1 << TWEN);
}

/**
 * Nastaví začátek okna a počká až bude zařízení připraveno
 */
void twi::start (void) {
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/**
 * Ukončí přenos a počká až bude zařízení připraveno
 */
void twi::stop (void) {
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
	while ((TWCR & (1 << TWSTO)));
}

/**
 * Pošle bajt dat po I2C a počká až bude zařízení připraveno
 * @param data bajt, který se má odeslat
 */
void twi::write (uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/**
 * Přečte bajt z I2C a počká až bude zažízení připraveno
 * @return přečtený bajt
 */
uint8_t twi::readACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/**
 * Přečte bajt z I2C a počká až bude zařízení připraveno. načítání je
 * následováno stopem
 * @return přečtený bajt
 */
uint8_t twi::readNACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/**
 * Vrátí status I2C
 * @return vrátí stav komunikace
 */
uint8_t twi::getStatus (void) {
	uint8_t status;
	//mask status
	status = TWSR & 0xf8;
	return status;
}

/**
 * Porovnání stavu s očekáváním
 * @param očekávaný stav
 * @return Odpovídá stav očekávání?
 */
bool twi::statusIs (uint16_t expected) {
	return twi::getStatus() == expected;
}
