#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "course.h"
#include "screen.h"
#include "window.h"
#include "title.h"

#ifdef NOVOICE
#undef VOICE
#endif

#ifdef VOICE
#ifndef VOICE_DEVICE
#define VOICE_DEVICE "/dev/audio"
#endif
#else
#undef VOICE_DEVICE
#endif

Pixmap title_pixmap;
int title_pixmap_width, title_pixmap_height;

static struct rect{
	int x, y, wx, wy;
} title_menu_pos[3] = {
{ 260, 320, 80, 20 },
{ 260, 360, 80, 20 },
{ 260, 400, 80, 20 }, };

static char *title_menu_string[3] = {
"START", "RECORD", "EXIT"};

static char *title_string = "X Gikadai Racer ver. 1.0 (c) TUT Computer Club,1996";
#define TITLE_STRING_POS_X 30
#define TITLE_STRING_POS_Y 300

#define TITLE_BG_COLOR 0
#define TITLE_FG_COLOR 6
#define TITLE_FONT_COLOR 7
#define TITLE_STR_OFF_X 8
#define TITLE_STR_OFF_Y 18

void title_init()
{
	extern char image_title[];
	title_pixmap = image_to_pixmap( image_title, &title_pixmap_width, &title_pixmap_height );
}

void title_start()
{
	FILE *fp;
	extern unsigned char voice_data[];
	int i;
	title_redraw();
	if( voice_flag == 0 ) return;
#ifdef VOICE_DEVICE
	if( fork() == 0 ){
		fp = fopen( VOICE_DEVICE, "wb" );
		if( fp == NULL ) return;
		for( i = 0 ; voice_data[i] != 0xff ; i++ ){
			fputc( voice_data[i], fp );
		}
		fclose( fp );
		exit( 0 );
	}
#endif
}

void title_redraw()
{
	int i;
	XFillRectangle( display, window, gc_table[TITLE_BG_COLOR],
		0, 0, WINDOW_WX, WINDOW_WY );
	XCopyArea( display, title_pixmap, window, gc,
		0, 0, title_pixmap_width, title_pixmap_height, 0, 0 );
	for( i = 0 ; i < 3 ; i++ ){
		XFillRectangle( display, window, gc_table[TITLE_FG_COLOR],
			title_menu_pos[i].x, title_menu_pos[i].y,
			title_menu_pos[i].wx, title_menu_pos[i].wy );
		XDrawString( display, window, gc_table[TITLE_FONT_COLOR],
			title_menu_pos[i].x + TITLE_STR_OFF_X,
			title_menu_pos[i].y + TITLE_STR_OFF_Y,
			title_menu_string[i], strlen( title_menu_string[i] ) );
	}
	XDrawString( display, window, gc_table[TITLE_FONT_COLOR],
		TITLE_STRING_POS_X, TITLE_STRING_POS_Y,
		title_string, strlen( title_string ) );
}

void title_button_press( int x, int y )
{
	if( x > title_menu_pos[0].x && y > title_menu_pos[0].y &&
	    x < title_menu_pos[0].x + title_menu_pos[0].wx &&
	    y < title_menu_pos[0].y + title_menu_pos[0].wy ){
		game_mode = MODE_READY;
		game_init();
		game();
	}
	else if( x > title_menu_pos[1].x && y > title_menu_pos[1].y &&
	    x < title_menu_pos[1].x + title_menu_pos[1].wx &&
	    y < title_menu_pos[1].y + title_menu_pos[1].wy ){
		game_mode = MODE_RECORD;
		record_redraw();
	}
	else if( x > title_menu_pos[2].x && y > title_menu_pos[2].y &&
	    x < title_menu_pos[2].x + title_menu_pos[2].wx &&
	    y < title_menu_pos[2].y + title_menu_pos[2].wy ){
		exit( 0 );
	}
}
