#ifndef HC595_H
#define HC595_H

#include <stddef.h>

#define RCK_PIN 2 // GPIO 27
#define SCL_PIN 3 // GPIO 22

int shift_init(int speed);
void shift_close();

inline void shift_out(unsigned char* data, size_t len);

#endif /* HC595_H */
