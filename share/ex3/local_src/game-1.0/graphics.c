#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "graphics.h"

/* The file desciptor to the framebuffer */
int fb_desc;

/* Pointer to the memory mapped framebuffer memory */
unsigned short *fb_mmap;

/*
 *  Initialises the graphics by opening the framebuffer and memory mapping it
 *
 *  Arguments: None
 *  Returns: void
 */
void init_graphics()
{
	fb_desc = open("/dev/fb0", O_RDWR);
	if (fb_desc < 0) {
		printf("Can't open framebuffer\n");
		exit(-1);
	}

	/* Memory map the framebuffer to make it easier to work with */
	fb_mmap = (unsigned short *) mmap(NULL, WIDTH * HEIGHT * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fb_desc, 0);
	if ((int) fb_mmap == -1) {
        	printf("Error: failed to map framebuffer device to memory\n");
        	printf("Error number: %d\n", errno);
        	exit(-1);
    	}

    	/* Clear the framebuffer */
    	draw_rect(0, 0, WIDTH, HEIGHT, 0, 0, 0);
    	draw_full_fb();
}

/*
 *  Cleans up by unmapping the memory and closing the file
 *
 *  Arguments: None
 *  Returns: void
 */
void cleanup_graphics()
{
	munmap(fb_mmap, WIDTH * HEIGHT * 2);
	close(fb_desc);
}

/*
 *  Draws the full framebuffer to the screen.
 *
 *  Arguments: None
 *  Returns: void
 */
void draw_full_fb()
{
	struct fb_copyarea rect;
	rect.dx = 0;
    	rect.dy = 0;
    	rect.width = WIDTH;
    	rect.height = HEIGHT;
	ioctl(fb_desc, 0x4680, &rect);
}

/*
 *  Draws the rectangle specified by the arguments with a color that is also specifies by the arguments.
 *
 *  Arguments: x: The x position of the upper left corner of the rectangle
 *             y: The y position of the upper left corner of the rectangle
 *             w: The width of the rectangle
 *             h: The height of the rectangle
 *             r: The value for the red color channel, the last 5 bits are used in the color
 *             g: The value for the green color channel, the last 6 bits are used in the color
 *             b: The value for the blue color channel, the last 5 bits are used in the color
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