#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>

#define GAMEPAD_NAME "/dev/TDT4258"

#define WIDTH 320
#define HEIGHT 240
#define BIT_DEPTH 16

int gamepad_desc;
short buttons;

int fb_desc;
char *fb_mmap;

void driver_signal_handler(int signum)
{
	printf("Recieved signal: %d\n", signum);
	int res = read(gamepad_desc, &buttons, sizeof(short));
	printf("Read bytes: %d\n", res);
	printf("Read buttons: %d\n", buttons);
}


int main(int argc, char *argv[])
{
	/* printf("Hello World, I'm game!\n"); */

	int file_flags;
	int i, j, r, g, b;

	gamepad_desc = open(GAMEPAD_NAME, 0);

	if (gamepad_desc < 0) {
		printf("Can't open gamepad driver\n");
		exit(-1);
	}

	signal(SIGIO, driver_signal_handler);
	fcntl(gamepad_desc, F_SETOWN, getpid());
	file_flags = fcntl(gamepad_desc, F_GETFL);
	fcntl(gamepad_desc, F_SETFL, file_flags | FASYNC);
	

	fb_desc = open("/dev/fb0", O_RDWR);
	if (fb_desc < 0) {
		printf("Can't open framebuffer\n");
		exit(-1);
	}

	fb_mmap = (char *) mmap(NULL, WIDTH * HEIGHT * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fb_desc, 0);
	if ((int) fb_mmap == -1) {
        	printf("Error: failed to map framebuffer device to memory\n");
        	printf("Error number: %d\n", errno);
        	exit(-1);
    	}

    	for (i = 0; i < HEIGHT; i++) {
    		for (j = 0; j < WIDTH; j++) {
    			r = 0b11111;
    			g = 0b111111;
    			b = 0b11111;
    			unsigned short color = (r << 11) | (g << 5) | b;
    			*(fb_mmap + (i * WIDTH + j) * 2) = color;
    		}
    	}

    	struct fb_copyarea rect;
    	rect.dx = 0;
    	rect.dy = 0;
    	rect.width = WIDTH;
    	rect.height = HEIGHT;
    	ioctl(fb_desc, 0x4680, &rect);

	while(1);

	munmap(fb_mmap, WIDTH * HEIGHT * 2);
	close(fb_desc);
	close(gamepad_desc);

	exit(EXIT_SUCCESS);
}
