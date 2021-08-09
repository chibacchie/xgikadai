#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gr.h"
#include "grx.h"
#include "window.h"
#include "screen.h"
#include "course.h"
#include "record.h"
#include "mycar.h"

int b_limit = 5;

int mx, my, mz, marg;
int mx_ini, my_ini, mz_ini, marg_ini;
int mccd;
int drift;

int m_lap, m_lap_disp, m_speed, m_rest;
int m_time, m_time_sum, m_best, m_time_rec[10], m_lap_max;
int m_rest_ini, m_rest_add;
int max_m_speed;
long m_l;
int ranking;
int mdx, mdy, mdarg;
int c_x, c_y;
int reverse_count;

static double m_handle;

#define HANDLE_MAX_1 0.13
#define HANDLE_MAX_2 0.15
#define HANDLE_SPEED 0.09
#define LEN 360.0

void mycar_init()
{
	m_rest_ini = REST_INI;
	m_rest_add = REST_ADD;
	m_lap_max = LAP_MAX;

	m_lap = 1;
	m_lap_disp = 1;
	m_speed = 0;
	mccd = 0;
	m_rest = m_rest_ini;
	mx = mx_ini;
	my = my_ini;
	mz = mz_ini;
	marg = marg_ini;
	m_time = 0;
	m_handle = 0;
	max_m_speed = MAX_M_SPEED_INI;

	mdx = 0;
	mdy = 0;
	mdarg = 0;
	drift = 0;

	c_x = 0;
	c_y = 0;
}

int mmove()
{
	int i, k6, k7, k8, k9, j_p, sig;
	int vx, vy;
	int mccd_1;
	int mx1, my1, marg1;
	double handle_max;
	long l;
	double rad, rot, arg, narg;

	mx1 = mx;
	my1 = my;
	marg1 = marg;

	mx += c_x;
	my += c_y;

	if( isMid() ||  game_clear_flag ){
		if( fabs( m_handle ) > HANDLE_MAX_1 && !game_clear_flag && m_speed > 220 ){
			drift = 14;
		}
		handle_max = HANDLE_MAX_1;
		if( m_speed < 150) m_speed += 9;
		else if( m_speed < max_m_speed ) m_speed += (max_m_speed - m_speed) / 30;
		else m_speed = max_m_speed;
	}
	else{
		handle_max = HANDLE_MAX_2;
		m_speed -= 4;
		if( m_speed < 0 ) m_speed = 0;
	}
	if( game_over_flag ) m_speed -= 30;

	if( isLeft() ){
		m_handle += HANDLE_SPEED;
		if( m_handle > handle_max ) m_handle = handle_max;
		if( drift && mdarg < 0 ) mdarg /= 2;
	}
	else if( isRight() ){
		m_handle -= HANDLE_SPEED;
		if( m_handle < -handle_max ) m_handle = -handle_max;
		if( drift && mdarg > 0 ) mdarg /= 2;
	}
	else{
		if( m_handle > HANDLE_SPEED ) m_handle -= HANDLE_SPEED;
		else if( m_handle < -HANDLE_SPEED ) m_handle += HANDLE_SPEED;
		else m_handle = 0.0;
	}
	if( m_handle == 0.0 ){
		arg = (double)marg * M_PI / 180.0;
		vy = (int)((double)m_speed * ( dd_sin( arg ) ) / 35);
		vx = (int)((double)m_speed * ( dd_cos( arg ) ) / 35);
		my += vy;
		mx += vx;
	}
	else{
		rad = LEN / m_handle;
		rot = (double)m_speed / rad;

		arg = (double)marg * M_PI / 180.0;
		narg = arg + rot;
		vy = (int)((double)rad * ( dd_cos( arg ) - dd_cos( narg ) ) / 35);
		vx = - (int)((double)rad * ( dd_sin( arg ) - dd_sin( narg ) ) / 35);
		my += vy;
		mx += vx;
		marg = (int)(narg * 180.0 / M_PI);
		if( marg >= 360 ) marg -= 360;
		if( marg < 0 ) marg += 360;
	}

	l = l_product( course_data[mccd].xn, course_data[mccd].yn, mx-course_data[mccd].ox, my-course_data[mccd].oy );

	if( l > course_data[mccd].width * FXP_UNIT_LEN ){
		m_speed -= m_speed / 2;
		drift = 0;
		if( l > b_limit * FXP_UNIT_LEN ){
			i = (int)(l >> FXP_UNIT_LEN_BITS) - b_limit;
			mx -= ( course_data[mccd].xn * i ) >> FXP_UNIT_LEN_BITS;
			my -= ( course_data[mccd].yn * i ) >> FXP_UNIT_LEN_BITS;
		}
	}
	else if( l < - course_data[mccd].width * FXP_UNIT_LEN ){
		m_speed -= m_speed / 2;
		drift = 0;
		if( l < - b_limit * FXP_UNIT_LEN ){
			i = (int)((-l) >> FXP_UNIT_LEN_BITS ) - b_limit;
			mx += ( course_data[mccd].xn * i ) >> FXP_UNIT_LEN_BITS;
			my += ( course_data[mccd].yn * i ) >> FXP_UNIT_LEN_BITS;
		}
	}


	mccd_1 = mccd - 1;
	if( mccd_1 < 0 ) mccd_1 = course_data_num - 1;

	if( l_product( course_data[mccd].bxn, course_data[mccd].byn, mx-course_data[mccd].bx, my-course_data[mccd].by ) > 0 ){
		mccd++;
		if( mccd >= course_data_num ){
			mccd = 0;
			m_lap++;
		}
		reverse_count = -1;
	}
	else if( l_product( course_data[mccd_1].bxn, course_data[mccd_1].byn, mx-course_data[mccd_1].bx, my-course_data[mccd_1].by ) < 0 ){
		mccd--;
		if( mccd < 0 ){
			mccd = course_data_num - 1;
			m_lap--;
		}
		if( reverse_count >= 0 ){
			reverse_count++;
			if( reverse_count >= 2 ) reverse_count = -20;
		}
	}
	if( m_lap_disp < m_lap && mccd > 2 ){
		m_lap_disp = m_lap;
		m_rest += m_rest_add;
		m_time_rec[m_lap_disp - 2] = m_time;
		m_time = 0;
		game_over_flag = 0;
		if( m_lap == m_lap_max + 1 ){
			game_clear_flag = 1;
		}
	}
	sig = 1;
	if( l_product( course_data[mccd].x, course_data[mccd].y, vx, vy ) < 0 ) sig = -1;

	m_l = l_product( course_data[mccd].x, course_data[mccd].y, mx-course_data[mccd].ox, my-course_data[mccd].oy );
	mz = CAR_Z + course_data[mccd].oz + (int)( ((long)course_data[mccd].z * m_l) / (long)course_data[mccd].r2 );
	max_m_speed = MAX_M_SPEED_INI - course_data[mccd].z * 3 * sig;

	if( reverse_count < 0 ){
		reverse_count++;
	}
	if( drift > 0 ){
		if( drift >= 14 ){
			mx += mdx / 2;
			my += mdy / 2;
			mdx = mx - mx1;
			mdy = my - my1;
			mdarg = marg - marg1;
			if( mdarg >= 180 ) mdarg -= 360;
			if( mdarg <= -180 ) mdarg += 360;
			drift--;
		}
		else{
			int mx2, my2;
			mx2 = mx - mx1;
			my2 = my - my1;
			mx = mx1 + mdx;
			my = my1 + mdy;
			mdx = (mdx + mx2) / 2;
			mdy = (mdy + my2) / 2;
			marg = marg1 + mdarg;
			if( marg >= 360 ) marg -= 360;
			else if( marg < 0 ) marg += 360;
			if( drift > 1 ) drift--;
			else mdarg /= 2;
		}
		if( mdarg == 0 ) drift = 0;
	}
	else{
		mx += mdx / 2;
		my += mdy / 2;
		mdx = mx - mx1;
		mdy = my - my1;
	}
	return 0;
}
