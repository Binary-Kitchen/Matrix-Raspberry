#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

#include "matrix.h"
#include "74hc595.h"

#define BLANK_PIN       4 // gpio23
#define PIXEL_CLOCK_PIN 5 // gpio24

#define BLANK_HIGH() digitalWrite(BLANK_PIN, 1)
#define BLANK_LOW()  digitalWrite(BLANK_PIN, 0)
#define PIXEL_HIGH() digitalWrite(PIXEL_CLOCK_PIN, 1)
#define PIXEL_LOW()  digitalWrite(PIXEL_CLOCK_PIN, 0)

#define SPI_SPEED 50000000
//#define SPI_SPEED 4000
//Absolutes Minimum!
#define NOPS_BETWEEN_PIXELCLOCK 100

static ll_frame_t *cur_frame = NULL;

static const unsigned int timing[MATRIX_PICTURES_PER_FRAME] =
{
	100,
	500,
	3000,
};

void matrix_update(ll_picture_t *picture)
{
	int bitno;
	unsigned int i;

	BLANK_LOW(); // Turn off matrix
	for (bitno = 0; bitno < BITS_PER_PANEL; bitno++) { // Iterate over all bits per panel
		shift_out((*picture) + NUM_SHIFTERS * bitno, NUM_SHIFTERS); // Output corresponding bits

		PIXEL_LOW(); // Toggel for accepting bits
		for (i = 0; i < NOPS_BETWEEN_PIXELCLOCK; i++) /* !FIXIT This is ugly! */
			asm volatile
			 ("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop");

		PIXEL_HIGH();
		for (i = 0; i < NOPS_BETWEEN_PIXELCLOCK; i++)
			asm volatile
			 ("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop");
	}
	BLANK_HIGH(); // Turn on matrix
}

void matrix_init(void)
{
	shift_init(SPI_SPEED);

	pinMode(BLANK_PIN, OUTPUT);
	pinMode(PIXEL_CLOCK_PIN, OUTPUT);

	BLANK_HIGH(); // Turn on matrix
}

void matrix_close(void)
{
	shift_close();
}

void matrix_setFrame(ll_frame_t *frame)
{
	cur_frame = frame;
}

void* matrix_run(void* arg)
{
	(void)arg;
	for (;;) {
		int i;
		for (i = 0; i < MATRIX_PICTURES_PER_FRAME; i++) {
			matrix_update((*cur_frame) + i);
			usleep(timing[i]);
		}
	}
	return (void *)42;
}
