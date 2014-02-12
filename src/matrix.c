#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include <wiringPi.h>

#include "matrix.h"
#include "74hc595.h"
#include "tools.h"

#define BLANK_HIGH() digitalWrite(BLANK_PIN, 1)
#define BLANK_LOW()  digitalWrite(BLANK_PIN, 0)
#define PIXEL_HIGH() digitalWrite(PIXEL_CLOCK_PIN, 1)
#define PIXEL_LOW()  digitalWrite(PIXEL_CLOCK_PIN, 0)

frame_t cur_frame;

void matrix_set_frame(frame_t *frame) {
    memcpy(cur_frame, *frame, sizeof(frame_t));
}

void matrix_update(picture_t *picture) {
    int bitno;
    unsigned int i;
    
    BLANK_LOW(); // Matrix aus
    for(bitno = 0 ; bitno < BITS_PER_PANEL ; bitno++) { // Iteriere über die Anzahl der Bits Pro Panel
        shift_out( (*picture)+NUM_SHIFTERS*bitno, NUM_SHIFTERS); // Gib das jeweilige Bit für alle Panels aus

        PIXEL_LOW(); // Toggel an der Matrix und übernimm
        for(i=0;i<NOPS_BETWEEN_PIXELCLOCK;i++)
            asm volatile("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop");
        
        PIXEL_HIGH();
        for(i=0;i<NOPS_BETWEEN_PIXELCLOCK;i++)
            asm volatile("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop");
    }
    BLANK_HIGH(); // Matrix 
}

void matrix_init(void) {
  shift_init(SPI_SPEED);
  
  memset(cur_frame, 0, sizeof(frame_t));
  
  pinMode(BLANK_PIN, OUTPUT);
  pinMode(PIXEL_CLOCK_PIN, OUTPUT);

  BLANK_HIGH(); // Matrix an
}

void matrix_close(void) {
  shift_close();
}

unsigned int timetable[NUM_PICTURES_PER_FRAME] = {5000, 1000};
