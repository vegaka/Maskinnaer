#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "game.h"

#define WIDTH 320
#define HEIGHT 240

void init_graphics();
void cleanup_graphics();
void draw_full_fb();
void draw_rect(int x, int y, int w, int h, int r, int g, int b);
void clear_object(struct gameobject *object);
void draw_gameobject(struct gameobject *object);
void update_fb(struct gameobject *object);

#endif