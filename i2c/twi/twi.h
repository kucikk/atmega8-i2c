/*
 * twi.h
 *
 * Created: 22. 11. 2014 10:09:35
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */ 


#ifndef TWI_H_
#define TWI_H_

namespace twi {
	void init (uint32_t, uint32_t);
	void start (void);
	void stop (void);
	void write (uint8_t);
	uint8_t readACK (void);
	uint8_t readNACK (void);
	uint8_t getStatus (void);
	bool statusIs (uint16_t);
};

#endif /* TWI_H_ */
