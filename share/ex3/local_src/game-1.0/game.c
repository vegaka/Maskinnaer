#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>
#include <unistd.h>

#define GAMEPAD_NAME "/dev/TDT4258"

void driver_signal_handler(int signum)
{
	printf("Recieved signal: %d\n", signum);
}


int main(int argc, char *argv[])
{
	/* printf("Hello World, I'm game!\n"); */

	int gamepad_desc;
	int file_flags;

	gamepad_desc = open(GAMEPAD_NAME, 0);

	if (gamepad_desc < 0) {
		printf("Can't open gamepad driver\n");
		exit(-1);
	}

	signal(SIGIO, driver_signal_handler);
	fcntl(gamepad_desc, F_SETOWN, getpid());
	file_flags = fcntl(gamepad_desc, F_GETFL);
	fcntl(gamepad_desc, F_SETFL, file_flags | FASYNC);
	
	while(1);

	exit(EXIT_SUCCESS);
}
