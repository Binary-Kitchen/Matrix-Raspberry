#ifndef SPI_H
#define	SPI_H

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

// Used in SPI_write
struct spi_ioc_transfer spi;
int spi_fd;

void SPI_setup(const int speed);
void SPI_close();

static inline void SPI_write(unsigned char* data, int len)
{
	spi.tx_buf = (unsigned long)data;
	spi.len = len;
	ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
}

#endif /* SPI_H */
