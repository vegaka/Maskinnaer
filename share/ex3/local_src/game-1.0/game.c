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

#define PADDLE_HEIGHT 30
#define PADDLE_WIDTH 10

#define BALL_SIZE 8

enum Direction { STAY, UP, DOWN };

int gamepad_desc;
short buttons;

int fb_desc;
unsigned short *fb_mmap;

struct gameobject
{
	int x;
	int y;
	int w;
	int h;
	int dx;
	int dy;
	int r;
	int g;
	int b;
};

void driver_signal_handler(int signum)
{
	printf("Recieved signal: %d\n", signum);
	int res = read(gamepad_desc, &buttons, sizeof(short));
	printf("Read bytes: %d\n", res);
	printf("Read buttons: %d\n", buttons);
}

void draw_rect(int x, int y, int w, int h, int r, int g, int b)
{
	int i;
	int j;
	unsigned short color = (r << 11) | (g << 5) | b;

	for (i = y; i < y + h; i++) {
    		for (j = x; j < x + w; j++) {
    			*(fb_mmap + (i * WIDTH + j)) = color;
    		}
    	}
}

void clear_object(struct gameobject *object)
{
	draw_rect(object->x, object->y, object->w, object->h, 0, 0, 0);
}


void draw_gameobject(struct gameobject *object)
{
	draw_rect(object->x, object->y, object->w, object->h, object->r, object->g, object->b);
}

void init_game(struct gameobject *player1, struct gameobject *player2, struct gameobject *ball)
{
	player1->x = 20;
	player1->y = (HEIGHT / 2) - (PADDLE_HEIGHT / 2);
	player1->w = PADDLE_WIDTH;
	player1->h = PADDLE_HEIGHT;
	player1->dx = 0;
	player1->dy = 0;
	player1->r = 0xff;
	player1->g = 0;
	player1->b = 0;

	player2->x = WIDTH - 20 - PADDLE_WIDTH;
	player2->y = (HEIGHT / 2) - (PADDLE_HEIGHT / 2);
	player2->w = PADDLE_WIDTH;
	player2->h = PADDLE_HEIGHT;
	player2->dx = 0;
	player2->dy = 0;
	player2->r = 0;
	player2->g = 0;
	player2->b = 0xff;

	ball->x = (WIDTH / 2) - (BALL_SIZE / 2);
	ball->y = (HEIGHT / 2) - (BALL_SIZE / 2);
	ball->w = BALL_SIZE;
	ball->h = BALL_SIZE;
	ball->dx = 0;
	ball->dy = 0;
	ball->r = 0xff;
	ball->g = 0xff;
	ball->b = 0xff;
}

/* Buttons: Last four bits indicate which buttons are pressed for the specific player */
/* A zero means the button is pressed */
void update_player(struct gameobject *player, int buttons)
{
	int up;
	int down;

	up = (~buttons) & (1 << 1);
	down = (~buttons) & (1 << 3);

	if (up && down) {
		player->dy = 0;
	} else if (up) {
		player->dy -= 1;
	} else if (down) {
		player->dy += 1;
	} else {
		player->dy = 0;
	}

	player->y += player->dy;

	if (player->y < 0) {
		player->y = 0;
	} else if (player->y + player->h > HEIGHT) {
		player->y = HEIGHT - player->h;
	}
}

int is_colliding(struct gameobject *obj1, struct gameobject *obj2)
{
	return obj1->x < obj2->x + obj2->w &&
		obj1->x + obj1->w > obj2->x &&
		obj1->y < obj2->y + obj2->h &&
   		obj1->h + obj1->y > obj2->y;
}

void update_ball(struct gameobject *ball, struct gameobject *player1, struct gameobject *player2)
{
	if (is_colliding(ball, player1) || is_colliding(ball, player2)) {
		if (ball->dx > 0) {
			ball->dx += 1;
		} else {
			ball->dx -= 1;
		}

		if (abs(ball->dx) > 9) {
			ball->dx = 9;
		}

    		ball->dx *= -1;
	}

	ball->y += ball->dy;
	ball->x += ball->dx;

	if (ball->y < 0) {
		ball->y = 0;
		ball->dy *= -1;
	} else if (ball->y + ball->h > HEIGHT) {
		ball->y = HEIGHT - ball->h;
		ball->dy *= -1;
	}

	if (ball->x < 0) {
		ball->x = 0;
		ball->dx *= -1;
	} else if (ball->x + ball->w > WIDTH) {
		ball->x = WIDTH - ball->w;
		ball->dx *= -1;
	}
}

int main(int argc, char *argv[])
{
	/* printf("Hello World, I'm game!\n"); */

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
	

	fb_desc = open("/dev/fb0", O_RDWR);
	if (fb_desc < 0) {
		printf("Can't open framebuffer\n");
		exit(-1);
	}

	fb_mmap = (unsigned short *) mmap(NULL, WIDTH * HEIGHT * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fb_desc, 0);
	if ((int) fb_mmap == -1) {
        	printf("Error: failed to map framebuffer device to memory\n");
        	printf("Error number: %d\n", errno);
        	exit(-1);
    	}

    	struct gameobject player1;
    	struct gameobject player2;
    	struct gameobject ball;

    	init_game(&player1, &player2, &ball);

    	ball.dx = 3;
    	ball.dy = 4;

    	struct fb_copyarea rect;

    	/* Game loop */
	while(1) {
		clear_object(&player1);
		clear_object(&player2);
		clear_object(&ball);

		update_player(&player1, buttons);
		update_player(&player2, buttons >> 4);

		update_ball(&ball, &player1, &player2);

		draw_gameobject(&player1);
    		draw_gameobject(&player2);
    		draw_gameobject(&ball);

    		rect.dx = 0;
    		rect.dy = 0;
    		rect.width = WIDTH;
    		rect.height = HEIGHT;
    		ioctl(fb_desc, 0x4680, &rect);
	}

	munmap(fb_mmap, WIDTH * HEIGHT * 2);
	close(fb_desc);
	close(gamepad_desc);

	exit(EXIT_SUCCESS);
}
