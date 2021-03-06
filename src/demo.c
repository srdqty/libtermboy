#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <termboy.h>
#include "pixmaps/all.h"

void *start_walking(void *arg);
void *keep_walking(void *arg);
void *stop_walking(void *arg);
void *start_playing(void *arg);
void *stop_playing(void *arg);
void handler(int sig);

static struct tb_sprite player, mountains, trees, waves, fill0, fill1, fill2;
static struct tb_anim walk_right, walk_left, waves_anim;

int main(void)
{
	int y, width, height, left = -1, right = 1;

	/*
	 * It's a good idea to handle fatal signals so we have a chance
	 * to clean up the screen if something goes wrong.
	 */
	signal(SIGABRT, handler);
	signal(SIGSEGV, handler);

	/*
	 * tb_screen_init must be called before attempting to draw
	 * anything.  It accepts a value (between 1 and 8) indicating
	 * how large the pixels should appear.
	 */
	if (tb_screen_init(4) != 0) {
		printf("This libtermboy demo must be run in a Linux "
			"virtual console.\n(Try pressing CTRL+ALT+F2)\n");
		exit(EXIT_FAILURE);
	}

	/* Retrieve the current screen dimensions */
	tb_screen_size(&width, &height);

	/*
	 * Colors values can be redefined to whatever we want.
	 * Sixteen colors are available.
	 */
	tb_screen_color(0,  0x000000);
	tb_screen_color(1,  0x04262E);
	tb_screen_color(2,  0x316034);
	tb_screen_color(3,  0x6A75C0);
	tb_screen_color(4,  0x452112);
	tb_screen_color(5,  0x625727);
	tb_screen_color(6,  0xE0A962);
	tb_screen_color(7,  0xFFF2BF);
	tb_screen_color(8,  0xD59470);
	tb_screen_color(9,  0x9A5A77);
	tb_screen_color(10, 0xFFFFFF);
	tb_screen_color(11, 0xF7C0B1);
	tb_screen_color(12, 0X1F7531);

	/* Initialize a new sprite with the specified dimensions */
	tb_sprite_init(&fill0, 1, 1);

	/*
	 * TB_SPRITE_COLOR can get or set a sprite's color at a
	 * particular position.  Note that, unlike tb_sprite_init, it
	 * does *not* accept a pointer argument (because it is
	 * implemented as a macro)
	 */
	TB_SPRITE_COLOR(fill0, 0, 0) = 3;

	/*
	 * Sprites can be tiled horizontally, vertically, or both;
	 * the pattern will be repeated across the entire screen.
	 */
	fill0.tile = TB_TILE_HORIZONTAL | TB_TILE_VERTICAL;

	tb_sprite_init(&mountains, 64, 7);
	/* Copy sprite data from an array of colors */
	TB_SPRITE_FILL(mountains, MOUNTAINS);
	mountains.tile = TB_TILE_HORIZONTAL;
	mountains.y = 20;

	tb_sprite_init(&fill1, 1, 25);
	for (y = 0; y < 25; ++y)
		TB_SPRITE_COLOR(fill1, 0, y) = 1;
	fill1.tile = TB_TILE_HORIZONTAL;
	/* The position of a sprite can be adjusted by setting x or y */
	fill1.y = 27;

	tb_sprite_init(&trees, 64, 26);
	TB_SPRITE_FILL(trees, TREES);
	trees.tile = TB_TILE_HORIZONTAL;
	trees.y = 52;

	tb_sprite_init(&fill2, 1, 25);
	for (y = 0; y < 25; ++y)
		TB_SPRITE_COLOR(fill2, 0, y) = 7;
	fill2.tile = TB_TILE_HORIZONTAL;
	fill2.y = 78;

	tb_sprite_init(&waves, 64, 31);
	TB_SPRITE_FILL(waves, WAVES0);
	waves.y = 103;
	waves.tile = TB_TILE_HORIZONTAL;

	tb_sprite_init(&player, 14, 16);
	TB_SPRITE_FILL(player, LINK_STAND_RIGHT);
	player.x = width/2 - 7;
	player.y = 80;
	/* Sprites on higher layers are drawn on top of lower layers */
	player.layer = 1;

	/* Make all of our sprites visible */
	tb_sprite_show(&fill0);
	tb_sprite_show(&mountains);
	tb_sprite_show(&fill1);
	tb_sprite_show(&trees);
	tb_sprite_show(&fill2);
	tb_sprite_show(&waves);
	tb_sprite_show(&player);

	/*
	 * Initialize a new animation for the specified sprite
	 * with the given number of frames
	 */
	tb_anim_init(&walk_right, &player, 2);

	/* Add a new frame to be shown for 150 milliseconds */
	tb_anim_add_frame(&walk_right, LINK_WALK_RIGHT, 150);
	tb_anim_add_frame(&walk_right, LINK_STAND_RIGHT, 150);

	tb_anim_init(&walk_left, &player, 2);
	tb_anim_add_frame(&walk_left, LINK_WALK_LEFT, 150);
	tb_anim_add_frame(&walk_left, LINK_STAND_LEFT, 150);

	tb_anim_init(&waves_anim, &waves, 6);
	tb_anim_add_frame(&waves_anim, WAVES0, 600);
	tb_anim_add_frame(&waves_anim, WAVES1, 200);
	tb_anim_add_frame(&waves_anim, WAVES2, 200);
	tb_anim_add_frame(&waves_anim, WAVES3, 600);
	tb_anim_add_frame(&waves_anim, WAVES2, 200);
	tb_anim_add_frame(&waves_anim, WAVES1, 200);
	tb_anim_start(&waves_anim);

	/*
	 * Register functions to be called (on separate threads) when
	 * certain key events occur. Keys are defined in linux/input.h.
	 */
	tb_key_handle(KEY_RIGHT, start_walking, keep_walking, stop_walking, &right);
	tb_key_handle(KEY_LEFT, start_walking, keep_walking, stop_walking, &left);
	tb_key_handle(KEY_A, start_playing, NULL, stop_playing, "A");
	tb_key_handle(KEY_B, start_playing, NULL, stop_playing, "B");
	tb_key_handle(KEY_C, start_playing, NULL, stop_playing, "C");
	tb_key_handle(KEY_D, start_playing, NULL, stop_playing, "D");
	tb_key_handle(KEY_E, start_playing, NULL, stop_playing, "E");
	tb_key_handle(KEY_F, start_playing, NULL, stop_playing, "F");
	tb_key_handle(KEY_G, start_playing, NULL, stop_playing, "G");

	/*
	 * Begin listening for key events.  This call will block until
	 * the user presses the escape key.  TB_LISTEN_NONBLOCKING can
	 * be used instead if you wish to use your own event loop.
	 */
	tb_key_listen(TB_LISTEN_BLOCKING);

	/* Make sure to restore the screen before exiting the program */
	tb_screen_restore();

	/* Free all of the memory allocated for our "objects" */
	tb_sprite_del(&player);
	tb_sprite_del(&mountains);
	tb_sprite_del(&trees);
	tb_sprite_del(&waves);
	tb_sprite_del(&fill0);
	tb_sprite_del(&fill1);
	tb_sprite_del(&fill2);
	tb_anim_del(&walk_right);
	tb_anim_del(&walk_left);
	tb_anim_del(&waves_anim);

	return EXIT_SUCCESS;
}

void *start_walking(void *arg)
{
	int dir = *(int *)arg;

	if (dir == 1)
		tb_anim_start(&walk_right);
	else
		tb_anim_start(&walk_left);

	return NULL;
}

void *keep_walking(void *arg)
{
	int dir = *(int *)arg;

	tb_sprite_move(&waves, waves.x - 3*dir, waves.y);
	tb_sprite_move(&trees, trees.x - 2*dir, trees.y);
	tb_sprite_move(&mountains, mountains.x - 1*dir, mountains.y);
	tb_msleep(30);

	return NULL;
}

void *stop_walking(void *arg)
{
	int dir = *(int *)arg;

	if (dir == 1)
		tb_anim_stop(&walk_right);
	else
		tb_anim_stop(&walk_left);

	return NULL;
}

void *start_playing(void *arg)
{
	char note = *(char *)arg;
	int frequency;

	switch (note) {
	default:
	case 'A':
		frequency = 440;
		break;
	case 'B':
		frequency = 494;
		break;
	case 'C':
		frequency = 262;
		break;
	case 'D':
		frequency = 294;
		break;
	case 'E':
		frequency = 330;
		break;
	case 'F':
		frequency = 349;
		break;
	case 'G':
		frequency = 392;
		break;
	}
	tb_tone_start(frequency);

	return NULL;
}

void *stop_playing(void *arg)
{
	tb_tone_stop();

	return NULL;
}

void handler(int sig)
{
	tb_screen_restore();
	tb_key_restore();
	signal(sig, SIG_DFL);
	kill(getpid(), sig);
}
