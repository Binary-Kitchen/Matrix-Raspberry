#ifndef SPI_H
#define	SPI_H

void SPI_setup(int speed);
void SPI_close();

inline void SPI_write(unsigned char* data, int len);

#endif /* SPI_H */
