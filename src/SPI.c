#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "SPI.h"

int spi_fd;

struct spi_ioc_transfer spi;

void SPI_close()
{
	close(spi_fd);
}

void SPI_setup(int speed)
{
	static const uint8_t spiMode = 0;
	static const uint8_t spiBPW = 8;

	if ((spi_fd = open("/dev/spidev0.0", O_RDWR)) < 0) {
		printf("Unable to open SPI device.");
		exit(-1);
	}

	if (ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode) < 0) {
		printf("SPI Mode Change failure.\n");
		exit(-1);
	}

	if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0) {
		printf("SPI BPW Change failure.\n");
		exit(-1);
	}

	if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
		printf("SPI Speed Change failure.\n");
		exit(-1);
	}
	//spi.tx_buf        = (unsigned long)data ;
	spi.rx_buf = (unsigned long)0;
	//spi.len           = len ;
	spi.delay_usecs = 0;
	spi.speed_hz = speed;
	spi.bits_per_word = spiBPW;
}

inline void SPI_write(unsigned char *data, int len)
{
	spi.tx_buf = (unsigned long)data;
	spi.len = len;

	ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
}
