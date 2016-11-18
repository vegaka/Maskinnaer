#ifndef GAME_H
#define GAME_H

/* Generic object used in the game */
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

#endif