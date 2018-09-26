#include "74hc595.h"

int shift_init(const int speed)
{
	wiringPiSetup();

	pinMode(RCK_PIN, OUTPUT);
	pinMode(SCL_PIN, OUTPUT);

	RCK_LOW();
	SCL_HIGH();

	SPI_setup(speed);

	return 0;
}

void shift_close()
{
    SPI_close();
}
