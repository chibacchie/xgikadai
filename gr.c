#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gr.h"
#include "grx.h"
#include "window.h"
#include "screen.h"
#include "course.h"
#include "record.h"
#include "enemy.h"
#include "mycar.h"

#define C_OFFSET_X 225
#define C_OFFSET_Y 300

#define E_COLOR 6
#define M_COLOR 7
#define BG_COLOR 0
#define FontColor 7
#define FontReverseColor 6

int game_over_flag, game_clear_flag, game_break_flag, game_start_flag;
int game_ready_count;
int game_mode;

void game_init()
{
	int i;

	ranking = 2;
	game_over_flag = 0;
	game_clear_flag = 0;
	game_start_flag = 0;
	reverse_count = 0;

	mycar_init();
	en_init();
	crash();

	for( i = 0 ; i < m_lap_max ; i++ ) m_time_rec[i] = 0;
	window_redraw();
}

void game_clear()
{
	int i, high_flag;
	high_flag = 0;
	m_best = m_time_rec[0];
	for( i = 0, m_time_sum = 0 ; i < LAP_MAX ; i++ ){
		m_time_sum += m_time_rec[i];
		if( m_time_rec[i] < m_best ) m_best = m_time_rec[i];
	}
	for( i = 0 ; i < LAP_MAX ; i++ ){
		if( m_time_rec[i] < best_lap ) high_flag = 1;
	}
	if( m_time_sum < best_time[9] ) high_flag = 1;
	if( high_flag ){
		game_mode = MODE_ENTER;
		enter_init();
		enter_redraw();
	}
	else{
		game_mode = MODE_RECORD;
		record_redraw();
	}
}

#define HANDLE_MAX_1 0.13
#define HANDLE_MAX_2 0.15
#define HANDLE_SPEED 0.09
#define LEN 360.0


void calc_ranking()
{
	if( game_clear_flag > 0 ) return;
	if( m_lap < e_lap ) ranking = 2;
	else if( m_lap > e_lap ) ranking = 1;
	else if( mccd < eccd ) ranking = 2;
	else if( mccd > eccd ) ranking = 1;
	else if( m_l < e_l ) ranking = 2;
	else ranking = 1;
	print_ranking();
}

int game()
{
	course_draw();
	calc_ranking();
	screen_clear();
	screen_draw_lobj();
	screen_draw_sobj();
	game_break_flag = 0;
	game_ready_count = 4;
	XtAppAddTimeOut( app_context, 10, game_ready, NULL );
}
void game_ready( XtPointer xp, XtIntervalId *id )
{
	char buf[2];
	game_ready_count--;
	buf[0] = game_ready_count + '0';
	buf[1] = 0;
	screen_clear();
	screen_draw_lobj();
	screen_draw_sobj();
	screen_draw_message( buf );
	if( game_ready_count <= 0 ){
		game_mode = MODE_GAME;
		XtAppAddTimeOut( app_context, wait_val, mainloop, NULL );
	}
	else{
		XtAppAddTimeOut( app_context, 1000, game_ready, NULL );
	}
	game_redraw();
}


void mainloop( XtPointer xp, XtIntervalId *id )
{
	if( game_break_flag ) return;
	if( mmove() ) return;
	en_move();
	calc_ranking();
	crash();
	screen_clear();
	screen_draw_lobj();
	screen_draw_sobj();
	if( m_lap <= m_lap_max ) print_lap();
	if( mccd > 2 ) game_start_flag = 1;
	if( m_time < 30000 && game_start_flag ) m_time += 7;
	if( game_over_flag == 0 ){
		if( m_rest > 0 ) m_rest -= 7;
		else game_over_flag = 1;
	}
	else{
		if( m_speed <= 0 ){
			screen_draw_message( "Game Over" );
			game_mode = MODE_STOP;
			game_redraw();
			return;
		}
	}
	if( game_clear_flag > 0 ){
		game_clear_flag++;
		if( ranking == 1 ){
			screen_draw_message( "You Win" );
		}
		else screen_draw_message( "You Lose" );
		if( game_clear_flag > 32 ){
			if( ranking == 1 ) screen_draw_staff();
			game_mode = MODE_STOP;
			game_clear();
			return;
		}
	}
	if( drift > 0 ){
		screen_draw_message( "Drift" );
	}
	else if( reverse_count < 0 ){
		screen_draw_message( "Reverse" );
	}
	window_redraw();

	XtAppAddTimeOut( app_context, wait_val, mainloop, NULL );
	return;
}
void game_redraw()
{
	XCopyArea( display, screen_pixmap, window, gc, 0, 0, SCREEN_WX, SCREEN_WY, 0, 0
 );
	XCopyArea( display, course, window, gc, 0, 0, COURSE_WX, COURSE_WY, C_OFFSET_X, C_OFFSET_Y );
	XFillRectangle( display, window, gc_table[BG_COLOR], 0, C_OFFSET_Y, C_OFFSET_X, COURSE_WY );
	XFillRectangle( display, window, gc_table[BG_COLOR], C_OFFSET_X + COURSE_WX, C_OFFSET_Y, C_OFFSET_X, COURSE_WY );
	print_lap();
	print_time();
	print_rest();
	print_speed();
	print_ranking();
	XFillRectangle( display, window, gc_table[M_COLOR],
		C_OFFSET_X + mx / 16 - 66, C_OFFSET_Y + 148 - my / 16, 4, 4 );
	XFillRectangle( display, window, gc_table[E_COLOR],
		C_OFFSET_X + ex / 16 - 66, C_OFFSET_Y + 148 - ey / 16, 4, 4 );
	XFlush( display );
}

