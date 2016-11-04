#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>

#define GAMEPAD_NAME "/dev/TDT4258"

void driver_signal_handler(int signum)
{
	printf("Recieved signal: %d\n", signum);
}


int main(int argc, char *argv[])
{
	/* printf("Hello World, I'm game!\n"); */

	int gamepad_desc;

	gamepad_desc = open(GAMEPAD_NAME, 0);

	if (gamepad_desc < 0) {
		printf("Can't open gamepad driver\n");
		exit(-1);
	}

	signal(SIGUSR1, driver_signal_handler);
	
	while(1);

	exit(EXIT_SUCCESS);
}
