#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gr.h"

#define UNIT_V_LEN 64

static double *sin_tbl;

void calc_init()
{
	double si;
	int i;
	sin_tbl = (double *)malloc( sizeof(double) * 451 );
	for( i = 0 ; i <= 90 ; i++ ){
		si = sin( (double)i * M_PI / 180.0 );
		sin_tbl[i] = si;
		sin_tbl[180 - i] = si;
		sin_tbl[i + 180] = - si;
		sin_tbl[360 - i] = - si;
		sin_tbl[i + 360] = si;
	}
}

double dd_sin( double n )
{
	int i;
	double sg;
	if( n < 0 ){ n = -n; sg = -1.0; }
	else sg = 1.0;
	n = n * 180.0 / M_PI;
	i = (int)n;
	n = n - (double)i;
	i %= 360;
	return sg * (sin_tbl[i] + (sin_tbl[i+1] - sin_tbl[i])* n);
}
double dd_cos( double n )
{
	int i;
	if( n < 0 ) n = -n;
	n = n * 180.0 / M_PI;
	i = (int)n;
	n = n - (double)i;
	i %= 360;
	return sin_tbl[i+90] + (sin_tbl[i+91] - sin_tbl[i+90])* n;
}

void outer_product( Point *n, Point *p1, Point *p2, Point *o )
{
	long x1, y1, z1, x2, y2, z2, x, y, z;
	double r;
	x1 = (p1->x - o->x);
	y1 = (p1->y - o->y);
	z1 = (p1->z - o->z);
	x2 = (p2->x - o->x);
	y2 = (p2->y - o->y);
	z2 = (p2->z - o->z);
	x = ( y1 * z2 - z1 * y2 );
	y = ( z1 * x2 - x1 * z2 );
	z = ( x1 * y2 - y1 * x2 );
	r = sqrt( x * x + y * y + z * z );
	r /= UNIT_V_LEN;
	n->x = (int)((double)x / r);
	n->y = (int)((double)y / r);
	n->z = (int)((double)z / r);
}

void get_nv( int *nx, int *ny, int x, int y )
{
	double r;
	*nx = -y;
	*ny = x;
	r = (long)x * (long)x + (long)y * (long)y;
	r = sqrt( r );
	if( r != 0 ){
		*nx = (int)((double)(*nx) * FXP_UNIT_LEN / r);
		*ny = (int)((double)(*ny) * FXP_UNIT_LEN / r);
	}
}

void rotate_d( int *x, int *y, int arg )
{
	double d_arg, dx, dy;
	d_arg = arg * M_PI / 180.0;
	dx = (double)*x;
	dy = (double)*y;
	*x = (long)(dx * dd_cos( d_arg ) - dy * dd_sin( d_arg ));
	*y = (long)(dx * dd_sin( d_arg ) + dy * dd_cos( d_arg ));
}

void rotate( int *dx, int *dy, int x, int y, int arg )
{
	double d_arg;
	d_arg = arg * M_PI / 180.0;
	*dx = (long)((double)x * dd_cos( d_arg ) - (double)y * dd_sin( d_arg ));
	*dy = (long)((double)x * dd_sin( d_arg ) + (double)y * dd_cos( d_arg ));
}
