#include <stdio.h>
#include <string.h>

#include "tools.h"

static int frame = 0;

void print_picture(picture_t* pic) {
    unsigned int x,y;
    char line[NUM_COLS*2+1];
    
    puts( "\e[2J" );
    printf("Frame #%d\n", frame++);

    for( y=0; y<NUM_ROWS; y++ ){
        for( x=0; x<NUM_COLS; x++ ){
            line[2*x]   = picture_getPixel( pic, x, y ) ? 'X' : '.';
            line[2*x+1] = 0x20;
        }
        line[2*NUM_COLS] = 0;
        puts(line);
    }
}
