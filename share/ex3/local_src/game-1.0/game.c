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

#define PADDLE_HEIGHT 30
#define PADDLE_WIDTH 10

#define BALL_SIZE 8
#define MAX_BALL_SPEED 7

int gamepad_desc;
short buttons;

int fb_desc;
unsigned short *fb_mmap;

struct gameobject
{
	int x;
	int oldx;
	int y;
	int oldy;
	int w;
	int h;
	int dx;
	int dy;
	int r;
	int g;
	int b;
};

/*
 *  Handles SIGIO signals from the gamepad driver.
 *  When a signal arrives it reads the button values from the driver.
 *
 *  Arguments: signum: the signal number which will always be SIGIO
 *  Returns: void
 */
void driver_signal_handler(int signum)
{
	/* printf("Recieved signal: %d\n", signum); */
	int res = read(gamepad_desc, &buttons, sizeof(short));
	printf("Read bytes: %d\n", res);
	/* printf("Read buttons: %d\n", buttons); */
}

/*
 *  Draws the rectangle specified by the arguments with a color that is also specifies by the arguments.
 *
 *  Arguments: x: the x position of the upper left corner of the rectangle
 *             y: the y position of the upper left corner of the rectangle
 *             w: the width of the rectangle
 *             h: the height of the rectangle
 *             r: the value for the red color channel, the last 5 bits are used in the color
 *             g: the value for the green color channel, the last 6 bits are used in the color
 *             b: the value for the blue color channel, the last 5 bits are used in the color
 *  Returns: void
 */
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

/*
 *  Clears the gameobject from the framebuffer by drawing a black rectangle over the object
 *
 *  Arguments: object: The object to clear
 *  Returns: void
 */
void clear_object(struct gameobject *object)
{
	draw_rect(object->x, object->y, object->w, object->h, 0, 0, 0);
}

/*
 *  Draws the gameobject in the framebuffer at the position specified by the object,
 *  and with the color specified by the object.
 *
 *  Arguments: object: The object to draw
 *  Returns: void
 */
void draw_gameobject(struct gameobject *object)
{
	draw_rect(object->x, object->y, object->w, object->h, object->r, object->g, object->b);
}

/*
 *  Updates the object on the actual screen and not only in the framebuffers memory.
 *
 *  Arguments: object: The object to draw to the actual screen
 *  Returns: void
 */
void update_fb(struct gameobject *object)
{
	/* Remove the current drawing of the object */
	struct fb_copyarea old_area;
	old_area.dx = object->oldx;
	old_area.dy = object->oldy;
	old_area.width = object->w;
	old_area.height = object->h + 1;

	/* Keep the rectangle within the framebuffer bounds */
	if (old_area.dy + old_area.height > HEIGHT) {
		old_area.height = HEIGHT - old_area.dy;
	}

	ioctl(fb_desc, 0x4680, &old_area);

	/* Paint the object at its new location */
	struct fb_copyarea new_area;
	new_area.dx = object->x;
	new_area.dy = object->y;
	new_area.width = object->w;
	new_area.height = object->h;

	/* Keep the rectangle within the framebuffer bounds */
	if (new_area.dy + new_area.height > HEIGHT) {
		new_area.height = HEIGHT - new_area.dy;
	}

	ioctl(fb_desc, 0x4680, &new_area);
}

/*
 *  Creates a white ball at the middle of the screen.
 *
 *  Arguments: ball: The ball to initialize
 *  Returns: void
 */
void init_ball(struct gameobject *ball)
{
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

/*
 *  Initializes the paddle at the specified x position with the specified color.
 *
 *  Arguments: paddle: the paddle to initialize
 *                  x: the x position of the to left corner of the paddle
 *                  r: the value for the red color channel, the last 5 bits are used in the color
 *                  g: the value for the green color channel, the last 6 bits are used in the color
 *                  b: the value for the blue color channel, the last 5 bits are used in the color
 *  Returns: void
 */
void init_paddle(struct gameobject *paddle, int x, int r, int g, int b)
{
	paddle->x = x;
	paddle->y = (HEIGHT / 2) - (PADDLE_HEIGHT / 2);
	paddle->w = PADDLE_WIDTH;
	paddle->h = PADDLE_HEIGHT;
	paddle->dx = 0;
	paddle->dy = 0;
	paddle->r = r;
	paddle->g = g;
	paddle->b = b;
}

/*
 *  Initializes and draws the initial game state
 *
 *  Arguments: player1: the left paddle
 *             player2: the right paddle
 *                ball: the ball used to play
 *  Returns: void
 */
void init_game(struct gameobject *player1, struct gameobject *player2, struct gameobject *ball)
{
	init_paddle(player1, 20, 0xff, 0, 0);
	init_paddle(player2, WIDTH - PADDLE_WIDTH - 20, 0, 0, 0xff);
	init_ball(ball);

	/* Draw initial gamestate */
	struct fb_copyarea rect;
	rect.dx = 0;
    	rect.dy = 0;
    	rect.width = WIDTH;
    	rect.height = HEIGHT;
	ioctl(fb_desc, 0x4680, &rect);
}

/* Buttons: Last four bits indicate which buttons are pressed for the specific player */
/* A zero means the button is pressed */
void update_player(struct gameobject *player, int buttons)
{
	int up;
	int down;

	player->oldx = player->x;
	player->oldy = player->y;

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

/* AABB collision detection algorithm */
int is_colliding(struct gameobject *obj1, struct gameobject *obj2)
{
	return obj1->x < obj2->x + obj2->w &&
		obj1->x + obj1->w > obj2->x &&
		obj1->y < obj2->y + obj2->h &&
   		obj1->h + obj1->y > obj2->y;
}

void update_ball(struct gameobject *ball, struct gameobject *player1, struct gameobject *player2)
{
	int is_col_p1 = is_colliding(ball, player1);
	int is_col_p2 = is_colliding(ball, player2);

	ball->oldx = ball->x;
	ball->oldy = ball->y;

	if (is_col_p1 || is_col_p2) {
		if (ball->dx > 0) {
			ball->dx += 1;
		} else {
			ball->dx -= 1;
		}

		if (abs(ball->dx) > MAX_BALL_SPEED) {
			if (ball->dx < 0) {
				ball->dx = -MAX_BALL_SPEED;
			} else {
				ball->dx = MAX_BALL_SPEED;
			}
		}

    		ball->dx *= -1;
	}

	ball->y += ball->dy;

	if (is_col_p1) {
		ball->x = player1->x + player1->w;
	} else if (is_col_p2) {
		ball->x = player2->x - ball->w;
	} else {
		ball->x += ball->dx;
	}

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

    		/* Update framebuffer areas */
    		update_fb(&player1);
    		update_fb(&player2);
    		update_fb(&ball);

    		usleep(20 * 1000);
	}

	munmap(fb_mmap, WIDTH * HEIGHT * 2);
	close(fb_desc);
	close(gamepad_desc);

	exit(EXIT_SUCCESS);
}
