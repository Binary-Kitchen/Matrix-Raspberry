#include <stdlib.h>
#include <string.h>

#include <wiringPi.h>

#include "74hc595.h"
#include "SPI.h"

#define SCL_HIGH() digitalWrite(SCL_PIN, 1)
#define SCL_LOW()  digitalWrite(SCL_PIN, 0)
#define RCK_HIGH() digitalWrite(RCK_PIN, 1)
#define RCK_LOW()  digitalWrite(RCK_PIN, 0)

int shift_init(int speed) {
  wiringPiSetup();

  pinMode(RCK_PIN, OUTPUT);
  pinMode(SCL_PIN, OUTPUT);

  RCK_LOW();
  SCL_HIGH();

  SPI_setup(speed);

  return 0;
}

inline int shift_out(unsigned char *data, size_t length) {
  // !FIXIT _massivst_ inperformant

  // Bits auf's Schieberegister
  //wiringPiSPIDataW(0, data, length);
  //RCK_LOW();
  SPI_write(data,length);
  //RCK_HIGH();

  // Phaser feuern.
  RCK_HIGH();
  RCK_LOW();

  //free(tmp);
  return 0;
}

int shift_close() {
    SPI_close();
    return 0;
}
