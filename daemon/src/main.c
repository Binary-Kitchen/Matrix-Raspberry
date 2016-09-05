#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include <config.h>

#include "74hc595.h"
#include "matrix.h"

#define REMOTE_RET_ERROR 0xff

static const char hello_str[] = "Hello, this Kitchen-Matrix "
	MATRIXD_VERSION_MAJOR "." MATRIXD_VERSION_MINOR
	" (sources: " MATRIXD_GIT_BRANCH "-" MATRIXD_GIT_COMMIT_HASH ")";

enum matrix_cmd {
	MATRIX_MODE_MONOCHROME = 1,
	MATRIX_MODE_GREYSCALE = 2,
	MATRIX_EXIT = 3,
};
static enum matrix_cmd mode = MATRIX_MODE_MONOCHROME;

static int sockfd;
static struct sockaddr_in servaddr, cliaddr;
static socklen_t len;

static pthread_t grayscale_thread;

void matrix_cmd(const enum matrix_cmd cmd)
{
	unsigned char ret = 0;

	switch (cmd) {
	case MATRIX_MODE_MONOCHROME:
		if (mode == MATRIX_MODE_MONOCHROME) {
			puts("Already in Monochrome mode!");
		} else {
			// Stop thread here
			pthread_cancel(grayscale_thread);
			mode = MATRIX_MODE_MONOCHROME;
			puts("Switched to Monochrome mode!");
		}
		break;
	case MATRIX_MODE_GREYSCALE:
		if (mode == MATRIX_MODE_GREYSCALE) {
		puts("Already in Greyscale mode!");
		} else {
			mode = MATRIX_MODE_GREYSCALE;
			// Start Thread here.
			pthread_create(&grayscale_thread, NULL, matrix_run,
				       NULL);
		puts("Switched to Greyscale mode!");
		}
		break;
	case MATRIX_EXIT:
		// Stop thread here, if grayscale.
		if (mode == MATRIX_MODE_GREYSCALE) {
			pthread_cancel(grayscale_thread);
		}
		ret = REMOTE_RET_ERROR;
	default:
		ret = REMOTE_RET_ERROR;
		puts("Unknown command.");
		break;
	}

	sendto(sockfd, &ret, sizeof(ret), 0,
	       (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}

void matrix_main_loop()
{
	enum matrix_cmd cmd;
	ssize_t n;

	// Initialize frames
	ll_frame_t *cur, *next;
	cur = malloc(sizeof(ll_frame_t));
	next = malloc(sizeof(ll_frame_t));

	bzero(cur, sizeof(ll_frame_t));
	bzero(next, sizeof(ll_frame_t));

	matrix_setFrame(cur);

	for (;;) {
		n = recvfrom(sockfd, (void*)next, sizeof(ll_frame_t), 0,
			     (struct sockaddr *)&cliaddr, &len);

		if (n == sizeof(enum matrix_cmd)) {
			cmd = *(enum matrix_cmd*)next;
			matrix_cmd(cmd);
		} else if (n == sizeof(ll_picture_t)) {
			if (mode == MATRIX_MODE_MONOCHROME)
				matrix_update((ll_picture_t*)next);
			else
				puts("Matrix not in Monochrome mode!");
		} else if (n == sizeof(ll_frame_t)) {
			if (mode == MATRIX_MODE_GREYSCALE) {
				matrix_setFrame(next);
				// Swap cur <-> next
				void *tmp = next;
				next = cur;
				cur = tmp;
			} else
				puts("Matrix not in Greyscale mode!");
		} else
			printf("UDP packet size mismatch: %d\n", n);
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
	ll_picture_t picture;
	memset(&picture, 0, sizeof(picture));
	matrix_update(&picture);

	puts("Initializing Networking...");
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket");
		exit(-1);
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
