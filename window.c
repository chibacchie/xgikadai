#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "course.h"
#include "screen.h"
#include "window.h"
#include "title.h"
#include "mycar.h"
#include "enemy.h"
#include "record.h"

Window window;

#define FontColor 7
#define FontReverseColor 6

void print_lap()
{
	char buf[80];
	if( m_lap_disp > 3 ) return;
	sprintf( buf, "Lap %d", m_lap_disp );
	XDrawString( display, window, gc_table[FontColor],
		20, 320, buf, strlen( buf ) );
}
void print_time()
{
	int i, c;
	char buf[80];
	XDrawString( display, window, gc_table[FontColor],
		420, 340, "TIME", 4  );
	for( i = 1 ; i <= m_lap_disp ; i++ ){
		if( i == m_lap_disp ){
			if( i > 3 ) continue;
			c = FontColor;
			sprintf( buf, "%02d'%02d\"", m_time / 100, m_time % 100 );
		}
		else{
			c = FontReverseColor;
			sprintf( buf, "%02d'%02d\"", m_time_rec[i - 1] / 100, m_time_rec[i - 1] % 100 );
		}
		XDrawString( display, window, gc_table[c],
			470, 320 + i * 20, buf, strlen( buf ) );
	}
}
void print_rest()
{
	char buf[80];

	sprintf( buf, "Rest %02d", m_rest / 100 );
	XDrawString( display, window, gc_table[FontColor],
		20, 350, buf, strlen( buf ) );
}
void print_speed()
{
	char buf[80];
	sprintf( buf, "Speed %02d km/h", m_speed / 3 );
	XDrawString( display, window, gc_table[FontColor],
		20, 380, buf, strlen( buf ) );
}
void print_ranking()
{
	if( ranking == 1 ){
		XDrawString( display, window, gc_table[FontColor],
			180, 320, "1st", 3 );
	}
	else{
		XDrawString( display, window, gc_table[FontColor],
			180, 320, "2nd", 3 );
	}
}
void window_init()
{
	window = XtWindow( game_widget );
}
void window_map()
{
	XMapWindow( display, window );
}
void window_redraw()
{
	switch( game_mode ){
	case MODE_TITLE:
		title_redraw();
		break;
	case MODE_RECORD:
		record_redraw();
		break;
	case MODE_GAME:
		game_redraw();
		break;
	case MODE_STOP:
		game_redraw();
		break;
	case MODE_ENTER:
		enter_redraw();
		break;
	case MODE_READY:
		game_redraw();
		break;
	}
}
void quit()
{
	switch( game_mode ){
	case MODE_TITLE:
		exit( 0 );
	case MODE_GAME:
		game_break_flag = 1;
		game_mode = MODE_TITLE;
		title_start();
		break;
	}
}
