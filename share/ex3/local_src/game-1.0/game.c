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

#include "game.h"
#include "graphics.h"

#define GAMEPAD_NAME "/dev/TDT4258"

#define PADDLE_HEIGHT 30
#define PADDLE_WIDTH 10

#define BALL_SIZE 8
#define MAX_BALL_SPEED 7

/* The file desciptor to the gamepad driver file */
int gamepad_desc;

/* Variable to hold the state of the gamepad buttons */
short buttons;

/*
 *  Handles SIGIO signals from the gamepad driver.
 *  When a signal arrives it reads the button values from the driver.
 *
 *  Arguments: signum: The signal number which will always be SIGIO
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
 *  Arguments: paddle: The paddle to initialize
 *                  x: The x position of the to left corner of the paddle
 *                  r: The value for the red color channel, the last 5 bits are used in the color
 *                  g: The value for the green color channel, the last 6 bits are used in the color
 *                  b: The value for the blue color channel, the last 5 bits are used in the color
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
 *  Arguments: player1: The left paddle
 *             player2: The right paddle
 *                ball: The ball used to play
 *  Returns: void
 */
void init_game(struct gameobject *player1, struct gameobject *player2, struct gameobject *ball)
{
	init_paddle(player1, 20, 0xff, 0, 0);
	init_paddle(player2, WIDTH - PADDLE_WIDTH - 20, 0, 0, 0xff);
	init_ball(ball);

	/* Draw initial gamestate */
	draw_full_fb();
}

/*
 *  Performs one update for the specified player.
 *
 *  Arguments: player: The player to update
 *            buttons: The current status of the buttons for the player.
 *                     The last four bits indicate which buttons are pressed for
 *                     the specific player. A zero means the button is pressed.
 *  Returns: void
 */
void update_player(struct gameobject *player, int buttons)
{
	int up;
	int down;

	player->oldx = player->x;
	player->oldy = player->y;

	/* Get the status of the up and down buttons. */
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

	/* Check bounds */
	if (player->y < 0) {
		player->y = 0;
	} else if (player->y + player->h > HEIGHT) {
		player->y = HEIGHT - player->h;
	}
}

/* 
 *  AABB collision detection algorithm.
 *
 *  Arguments: obj1: The first object used in the collision testing
 *             obj2: The other object used in the collision testing
 *  Returns: int: 0 if the objects do not collide, 1 if they do
 */
int is_colliding(struct gameobject *obj1, struct gameobject *obj2)
{
	return obj1->x < obj2->x + obj2->w &&
		obj1->x + obj1->w > obj2->x &&
		obj1->y < obj2->y + obj2->h &&
   		obj1->h + obj1->y > obj2->y;
}

/* 
 *  Performs one update for the specified ball
 *
 *  Arguments: ball: The ball to update
 *          player1: The left paddle to check collision with
 *          player2: The right paddle to check collision with
 *  Returns: void
 */
void update_ball(struct gameobject *ball, struct gameobject *player1, struct gameobject *player2)
{
	/* Check for collisions */
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

		/* Prevent the ball from moving too fast */
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

	/* Push the ball out of the paddle */
	if (is_col_p1) {
		ball->x = player1->x + player1->w;
	} else if (is_col_p2) {
		ball->x = player2->x - ball->w;
	} else {
		ball->x += ball->dx;
	}

	/* Check bounds */
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

	/* Register SIGIO handler and tell the driver we want to 
	*  recieve async signals from it.
	*/
	signal(SIGIO, driver_signal_handler);
	fcntl(gamepad_desc, F_SETOWN, getpid());
	file_flags = fcntl(gamepad_desc, F_GETFL);
	fcntl(gamepad_desc, F_SETFL, file_flags | FASYNC);

	init_graphics();

    	struct gameobject player1;
    	struct gameobject player2;
    	struct gameobject ball;

    	init_game(&player1, &player2, &ball);

    	/* Set the initial speed and direction of the ball */
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

    		/* Sleep for 20 ms */
    		usleep(20 * 1000);
	}

	/* Cleanup */
	cleanup_graphics();
	close(gamepad_desc);

	exit(EXIT_SUCCESS);
}
