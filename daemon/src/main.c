#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "74hc595.h"
#include "matrix.h"
#include "graphics.h"
#include "tools.h"

#include <config.h>

#define COMMAND_LEN 4
#define MATRIX_MODE_MONOCHROME 1
#define MATRIX_MODE_GREYSCALE  2
#define MATRIX_EXIT 3

#define REMOTE_RET_ERROR 0xffffffff

static const char hello_str[] = "Hello, this matrixd "
    MATRIXD_VERSION_MAJOR "." MATRIXD_VERSION_MINOR
    " (sources: " MATRIXD_GIT_BRANCH "-" MATRIXD_GIT_COMMIT_HASH ")";

static int sockfd;
static struct sockaddr_in servaddr, cliaddr;
static socklen_t len;

static pthread_t greyscale_thread;

int matrix_mode = MATRIX_MODE_MONOCHROME;

int matrix_cmd(const uint32_t cmd)
{
	int retval = 0;
	uint32_t remote_retval = cmd;
	
	printf("Received Command 0x%08x\n", cmd);
	switch (cmd) {
		case MATRIX_MODE_MONOCHROME:
			if (matrix_mode == MATRIX_MODE_MONOCHROME) {
				puts("Already in Monochrome mode!");
			} else {
				// Stop thread here
				pthread_cancel(greyscale_thread);
				matrix_mode = MATRIX_MODE_MONOCHROME;
				puts("Switched to Monochrome mode!");
			}
			break;
		case MATRIX_MODE_GREYSCALE:
			if (matrix_mode == MATRIX_MODE_GREYSCALE) {
	            puts("Already in Greyscale mode!");
			} else {
				matrix_mode = MATRIX_MODE_GREYSCALE;
				// Start Thread here.
				pthread_create(&greyscale_thread, NULL, matrix_run,
					       NULL);
	            puts("Switched to Greyscale mode!");
			}
			break;
		case MATRIX_EXIT:
			// Stop thread here, if greyscale.
			if (matrix_mode == MATRIX_MODE_GREYSCALE) {
				pthread_cancel(greyscale_thread);
			}
			retval = 1;
			break;
		default:
			remote_retval = REMOTE_RET_ERROR;
	        puts("Unknown command.");
			break;
	}

	sendto(sockfd, &remote_retval, sizeof(uint32_t), 0,
	       (struct sockaddr *)&cliaddr, sizeof(cliaddr));

	return retval;
}

void matrix_main_loop()
{
	// Initialize frames
	frame_t *cur, *next;
	cur = malloc(sizeof(frame_t));
	next = malloc(sizeof(frame_t));

	bzero(cur, sizeof(frame_t));
	bzero(next, sizeof(frame_t));

	matrix_setFrame(cur);

	for (;;) {
		unsigned char cmd;
		ssize_t n;
		n = recvfrom(sockfd, (void *)next, sizeof(frame_t), 0,
					 (struct sockaddr *)&cliaddr, &len);

		if (n == COMMAND_LEN) {	// We just received a command
			cmd = *(uint32_t *) next;
			if (matrix_cmd(cmd))
				break;
		} else if (n == sizeof(picture_t)) {
			if (matrix_mode == MATRIX_MODE_MONOCHROME) {
				matrix_update((picture_t*)next);
			} else {
                puts("Matrix not in Monochrome mode!");
			}
		} else if (n == sizeof(frame_t)) {
			if (matrix_mode == MATRIX_MODE_GREYSCALE) {
				matrix_setFrame(next);
				// Swap cur <-> next
				void *tmp = next;
				next = cur;
				cur = tmp;
			} else {
                puts("Matrix not in Greyscale mode!");
			}
		} else {
            printf("UDP packet size mismatch: %d\n", n);
		}
	}

	free(cur);
	free(next);
}

int main(void)
{
	puts(hello_str);

    puts("Initializing Hardware....");
	matrix_init();

    puts("Clear Screen...");
	picture_t *start = picture_alloc();
	matrix_update(start);
	picture_free(start);

	puts("Initializing Networking...");
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
        printf("%s\n", strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(1337);
	bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	len = sizeof(cliaddr);

	puts("Getting ready to fire phasers...");
	matrix_main_loop();

    puts("Shutting down...");
	matrix_close();

	return 0;
}
