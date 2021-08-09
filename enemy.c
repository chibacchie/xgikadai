#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gr.h"
#include "grx.h"
#include "window.h"
#include "screen.h"
#include "course.h"
#include "mycar.h"
#include "enemy.h"

static double e_handle;

int ex, ey, ez, earg, eccd;
int epx1, epy1, epccd, epx2, epy2;
int erx, ery, erz;
int e_speed;
int e_lap;
long e_l;

static double
	f_in1 = 196.0,
	f_in_b1 = 128.0, 
	f_out1 = 0.1,
	f_in2 = 128.0,
	f_in_b2 = 10.0,
	f_out2 = 0.1;
static int f_count = 100;

double en_calc_handle( long l1, long l2 )
{
	double p1, p2;

	if( labs( l1 ) > (long)f_in_b1 || (rand() & 0x1f) == 0 )
		p1 = - ((double)(l1) - f_in_b1 ) / f_in1;
	else p1 = 0;
	if( labs( l2 - l1 ) > (long)f_in_b2 )
		p2 = - ((double)(l2 - l1) - f_in_b2) / f_in2;
	else p2 = 0;
	return (p1 * f_out1 + p2 * f_out2) / 2.0;
}

void en_init()
{
	ex = s_obj_tbl[0].x;
	ey = s_obj_tbl[0].y;
	ez = s_obj_tbl[0].z;
	earg = marg;
	e_lap = 1;
	e_speed = 0;
	eccd = 0;

	e_handle = 0.0;
}

void en_move()
{
	double arg;
	double rad;
	double narg;
	double rot;
	int eccd_1, epccd_1;
	long l, l2;

	ex -= c_x;
	ey -= c_y;

	if( e_speed < 360 ) e_speed += 30;
	else e_speed = 360;

	if( e_handle == 0.0 ){
		arg = (double)earg * M_PI / 180.0;
		ey += (int)((double)e_speed * ( dd_sin( arg ) ) / 30);
		ex += (int)((double)e_speed * ( dd_cos( arg ) ) / 30);
	}
	else{
		rad = EN_LEN / e_handle;
		rot = (double)e_speed / rad;

		arg = (double)earg * M_PI / 180.0;
		narg = arg + rot;
		ey += (int)((double)rad * ( dd_cos( arg ) - dd_cos( narg ) ) / 30);
		ex -= (int)((double)rad * ( dd_sin( arg ) - dd_sin( narg ) ) / 30);
		earg = (int)(narg * 180.0 / M_PI);
		if( earg >= 360 ) earg -= 360;
		if( earg < 0 ) earg += 360;
	}

	l = l_product( course_data[eccd].xn, course_data[eccd].yn, ex-course_data[eccd].ox, ey-course_data[eccd].oy );

	eccd_1 = eccd - 1;
	if( eccd_1 < 0 ) eccd_1 = course_data_num - 1;

	if( l_product( course_data[eccd].bxn, course_data[eccd].byn, ex-course_data[eccd].bx, ey-course_data[eccd].by ) > 0 ){
		eccd++;
		if( eccd >= course_data_num ){
			eccd = 0;
			e_lap++;
		}
	}
	else if( l_product( course_data[eccd_1].bxn, course_data[eccd_1].byn, ex-course_data[eccd_1].bx, ey-course_data[eccd_1].by ) < 0 ){
		eccd--;
		if( eccd < 0 ){
			eccd = course_data_num - 1;
			e_lap--;
		}
	}
	epy1 = ey + (int)((double)EN_V_POINT_1 * ( dd_sin( arg ) ) / 30);
	epx1 = ex + (int)((double)EN_V_POINT_1 * ( dd_cos( arg ) ) / 30);
	epy2 = ey + (int)((double)EN_V_POINT_2 * ( dd_sin( arg ) ) / 30);
	epx2 = ex + (int)((double)EN_V_POINT_2 * ( dd_cos( arg ) ) / 30);

	epccd = eccd;
	epccd_1 = epccd - 1;
	if( epccd_1 < 0 ) epccd_1 = course_data_num - 1;
	if( l_product( course_data[epccd].bxn, course_data[epccd].byn, epx1-course_data[epccd].bx, epy1-course_data[epccd].by ) > 0 ){
		epccd++;
		if( epccd >= course_data_num ) epccd = 0;
	}
	else if( l_product( course_data[epccd_1].bxn, course_data[epccd_1].byn, epx1-course_data[epccd_1].bx, epy1-course_data[epccd_1].by ) < 0 ){
		epccd--;
		if( epccd < 0 ) epccd = course_data_num - 1;
	}

	e_l = l_product( course_data[eccd].x, course_data[eccd].y, ex-course_data[eccd].ox, ey-course_data[eccd].oy );
	ez = course_data[eccd].oz + (int)( ((long)course_data[eccd].z * e_l) / (long)course_data[eccd].r2 );

	l = l_product( course_data[epccd].xn, course_data[epccd].yn, epx1-course_data[epccd].ox, epy1-course_data[epccd].oy );
	l2 = l_product( course_data[epccd].xn, course_data[epccd].yn, epx2-course_data[epccd].ox, epy2-course_data[epccd].oy );

	e_handle = en_calc_handle( l, l2 );
	if( e_handle > 0.2 ) e_handle = 0.2;
	if( e_handle < -0.2 ) e_handle = -0.2;
}

void crash()
{
	int sx1, sz1;
	int sarg;
	int r, cx, cy;

	sz1 = ex - mx;
	sx1 = ey - my;
	rotate( &erz, &erx, sz1, sx1, 360 - marg );
	ery = ez - mz;
	if( abs( erx ) < EN_A_X && abs( erz ) < EN_A_Z ){
		if( erz > erx ) r = erz;
		else r = erx;
		if( r == 0 ){
			cx = 0;
			cy = 0;
		}
		else{
			cx = erx * EN_A_LEN / r;
			cy = erz * EN_A_LEN / r;
		}
		if( cy > 0 ){
			m_speed -= m_speed * cy / (EN_A_LEN*3);
			drift = 0;
		}
		else{
			e_speed += e_speed * cy / (EN_A_LEN*3);
		}
		c_x = (mx - ex);
		c_y = (my - ey);
	}	
	else{
		c_x /= 2;
		c_y /= 2;
	}
}
