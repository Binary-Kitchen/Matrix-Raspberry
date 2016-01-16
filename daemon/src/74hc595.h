#ifndef HC595_H
#define HC595_H

#include <stddef.h>
#include <wiringPi.h>

#include "SPI.h"

#define RCK_PIN 2 // GPIO 27
#define SCL_PIN 3 // GPIO 22

#define SCL_HIGH() digitalWrite(SCL_PIN, 1)
#define SCL_LOW()  digitalWrite(SCL_PIN, 0)
#define RCK_HIGH() digitalWrite(RCK_PIN, 1)
#define RCK_LOW()  digitalWrite(RCK_PIN, 0)

int shift_init(const int speed);
void shift_close();

static inline void shift_out(unsigned char* data, size_t len)
{
	SPI_write(data, len);
	// fire
	RCK_HIGH();
	RCK_LOW();
}

#endif /* HC595_H */
