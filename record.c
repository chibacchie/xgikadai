#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "window.h"
#include "record.h"
#include "mycar.h"

Pixmap record_pixmap;
int record_pixmap_width, record_pixmap_height;

int best_time[10];
int best_lap;
char best_time_name[10][MAX_NAME_LEN];
char best_lap_name[MAX_NAME_LEN];

#ifndef RECORD_DIR
#define RECORD_DIR "/usr/local/games/xgikadai"
#endif
#define SAVE_FILE "xgikadai-record"

#define RECORD_BG_COLOR 0
#define RECORD_FONT_COLOR 7

void record_init()
{
	extern char image_record[];
	record_pixmap = image_to_pixmap( image_record, &record_pixmap_width, &record_pixmap_height );
}


void record_redraw()
{
	int i;
	char buf[256];
	XFillRectangle( display, window, gc_table[RECORD_BG_COLOR],
		0, 0, WINDOW_WX, WINDOW_WY );
	XCopyArea( display, record_pixmap, window, gc,
		0, 0, record_pixmap_width, record_pixmap_height, 0, 0 );
	for( i = 0 ; i < 10 ; i++ ){
		sprintf( buf, "%d'%02d\"%02d %3s",
			best_time[i] / 6000,
			(best_time[i] % 6000) / 100, 
			best_time[i] % 100,
			best_time_name[i] );
		XDrawString( display, window, gc_table[RECORD_FONT_COLOR],
			150 + (i / 5) * 150, 80 + (i % 5) * 20,
			buf, strlen( buf ) );
	}
	sprintf( buf, "BEST LAP %d'%02d\"%02d %3s",
		best_lap / 6000,
		(best_lap % 6000) / 100, 
		best_lap % 100,
		best_lap_name );
	XDrawString( display, window, gc_table[RECORD_FONT_COLOR],
		210, 180, buf, strlen( buf ) );
}

void record_button_press( int x, int y )
{
	game_mode = MODE_TITLE;
	title_start();
}

/*
** enter name
*/
static char name[MAX_NAME_LEN] = "";
static char *name_letter="ABCDEFGHIJKLMNOPQRSTUVWXYZ,.<!";
static int name_len = 0;

#define NAME_ORIGIN_X 30
#define NAME_ORIGIN_Y 90
#define NAME_WX 30
#define NAME_WY 20
#define NAME_SKIP_X 35
#define NAME_SKIP_Y 25
#define DEFAULT_NAME "AAA"

void enter_init()
{
	name_len = 0;
	name[0] = 0;
}

void enter_button_press( int x, int y )
{
	int i;
	if( name_len >= 3 ){
		read_time();
		if( best_time[9] > m_time_sum ){
			for( i = 9 ; i >= 1 && best_time[i-1] > m_time_sum ; i-- ){
				best_time[i] = best_time[i-1];
				strcpy( best_time_name[i], best_time_name[i-1] );
			}
			best_time[i] = m_time_sum;
			strcpy( best_time_name[i], name );
		}
		if( m_best < best_lap ){
			best_lap = m_best;
			strcpy( best_lap_name, name );
		}
		write_time();

		game_mode = MODE_RECORD;
		record_redraw();
		return;
	}
	for( i = 0 ; i < strlen( name_letter ) ; i++ ){
		if( x > NAME_ORIGIN_X + NAME_SKIP_X * (i % 16) &&
		    y > NAME_ORIGIN_Y + NAME_SKIP_Y * (i / 16) &&
		    x < NAME_ORIGIN_X + NAME_SKIP_X * (i % 16) + NAME_WX &&
		    y < NAME_ORIGIN_Y + NAME_SKIP_Y * (i / 16) + NAME_WY ){
			if( name_letter[i] == '<' ){
				if( name_len > 0 ){
					name_len--;
					name[name_len] = 0;
				}
			}
			else if( name_letter[i] == '!' ){
				if( name_len == 0 )
					strcpy( name, DEFAULT_NAME );
				name_len = 3;
			}
			else{
				name[name_len] = name_letter[i];
				name_len++;
				name[name_len] = 0;
			}
		}
	}
	enter_redraw();
}

void enter_redraw()
{
	static char name_string[256], letter[5];
	int i, c;

	game_redraw();
	strcpy( name_string, "Enter your name: " );
	strcat( name_string, name );
	XDrawString( display, window, gc_table[RECORD_FONT_COLOR],
		200, 50, "It's a new record!", 18 );
	XDrawString( display, window, gc_table[RECORD_FONT_COLOR],
		200, 70, name_string, strlen( name_string ) );
	if( name_len >= 3 ) return;
	for( i = 0 ; i < strlen( name_letter ) ; i++ ){
		XFillRectangle( display, window, gc_table[RECORD_BG_COLOR],
			NAME_ORIGIN_X + NAME_SKIP_X * (i % 16),
			NAME_ORIGIN_Y + NAME_SKIP_Y * (i / 16),
			NAME_WX, NAME_WY );
		if( name_letter[i] == '<' ) strcpy( letter, "BS" );
		else if( name_letter[i] == '!' ) strcpy( letter, "END" );
		else{
			letter[0] = ' ';
			letter[1] = name_letter[i];
			letter[2] = 0;
		}
		XDrawString( display, window, gc_table[RECORD_FONT_COLOR],
			NAME_ORIGIN_X + NAME_SKIP_X * (i % 16),
			NAME_ORIGIN_Y + NAME_SKIP_Y * (i / 16) + 15,
			letter, strlen( letter ) );
	}
}

int read_time()
{
	FILE *fp;
	int i;
	char buf[MAX_NAME_LEN+16], name[1024];
	strcpy( name, RECORD_DIR );
	if( name[strlen(name)-1] != '/' ) strcat( name, "/" );
	strcat( name, SAVE_FILE );
	fp = fopen( name, "r" );
	if( fp == NULL ){
		fp = fopen( SAVE_FILE, "r" );
		if( fp == NULL ){
			for( i = 0 ; i < 10 ; i++ ){
				best_time[i] = 18000;
				strcpy( best_time_name[i], "AAA" );
			}
			best_lap = 6000;
			strcpy( best_lap_name, "AAA" );
			return 1;
		}
	}
	for( i = 0 ; i < 10 ; i++ ){
		fgets( buf, MAX_NAME_LEN + 16, fp );
		sscanf( buf, "%d%s", &(best_time[i]), best_time_name[i] );
	}
	fgets( buf, MAX_NAME_LEN + 16, fp );
	sscanf( buf, "%d%s", &(best_lap), best_lap_name );
	fclose( fp );
	return 0;
}

int write_time()
{
	FILE *fp;
	int i;
	char name[1024];
	strcpy( name, RECORD_DIR );
	if( name[strlen(name)-1] != '/' ) strcat( name, "/" );
	strcat( name, SAVE_FILE );
	fp = fopen( name, "w" );
	if( fp == NULL ){
		fp = fopen( SAVE_FILE, "w" );
		if( fp == NULL ) return 1;
	}
	for( i = 0 ; i < 10 ; i++ ){
		fprintf( fp, "%d %s\n", best_time[i], best_time_name[i]);
	}
	fprintf( fp, "%d %s\n", best_lap, best_lap_name);
	fclose( fp );
	return 0;
}
