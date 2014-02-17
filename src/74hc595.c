#include <wiringPi.h>

#include "74hc595.h"
#include "SPI.h"

#define SCL_HIGH() digitalWrite(SCL_PIN, 1)
#define SCL_LOW()  digitalWrite(SCL_PIN, 0)
#define RCK_HIGH() digitalWrite(RCK_PIN, 1)
#define RCK_LOW()  digitalWrite(RCK_PIN, 0)

int shift_init(int speed)
{
	wiringPiSetup();

	pinMode(RCK_PIN, OUTPUT);
	pinMode(SCL_PIN, OUTPUT);

	RCK_LOW();
	SCL_HIGH();

	SPI_setup(speed);

	return 0;
}

inline void shift_out(unsigned char *data, size_t length)
{
	SPI_write(data, length);

    // fire
	RCK_HIGH();
    RCK_LOW();
}

void shift_close()
{
    SPI_close();
}
