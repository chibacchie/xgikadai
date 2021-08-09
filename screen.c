#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "screen.h"
#include "window.h"
#include "course.h"
#include "mycar.h"
#include "enemy.h"

#define SCREEN_OX 300
#define SCREEN_OY 150
#define SCREEN_MES_X 260
#define SCREEN_MES_Y 150
#define SCREEN_MES_COLOR 7
#define SCREEN_STAFF_X 50
#define SCREEN_STAFF_Y 170
#define SCREEN_STAFF_WY 20
#define SCREEN_STAFF_COLOR 14
#define RANGE_L_X 270L
#define RANGE_L_Y 270L
#define S_OBJ_CUT 40
#define S_OBJ_CUT_0 10
#define S_OBJ_DIST_0 200

Pixmap screen_pixmap;

#define STAFF_NUM 7
static char *staff_name[STAFF_NUM] = {
"Copyright (c) TUT Computer Club,1996",
"STAFF",
"        Program: Hideo Tomizu",
"  Object Design: Akihiro Nakano, Hideo Tomizu",
"   Title Design: Yasuhito Makino",
"          Voice: Keijiro Umehara",
" Special Thanks: Tsuyoshi Iida, Hiromitsu Shiina, sigh  (Hitoshi Suzuki)"};

int *l_obj[32][16];
int l_obj_array[1024];

OBJ_CLASS s_obj_class[32];
OBJ *s_obj[64][32];
OBJ s_obj_tbl[MAX_S_OBJ];
int s_obj_tbl_num;
Polygon s_polygon[MAX_S_POLY];
int s_polygon_num;
Point s_point[MAX_S_POINT];
int s_point_num;

static Point s_p[MAX_S_POINT];
static int s_rot_flag[MAX_S_POINT];
static ProjectedObj s_po[MAX_S_OBJ];

static Point2 l_point2[MAX_L_POINT2];
static Point l_rot[MAX_L_POINT];
static int l_projected[MAX_L_POINT];
static int l_polygon_drawn[MAX_L_POLY];
static ProjectedPolygon l_ppolygon[MAX_L_POLY];
static int l_point2_num;
static int pp_ptr;

static void z_cut( Point *p, Point *p1, Point *p2 )
{
	p->x = p2->x + (int)((long)(p1->x - p2->x)*(long)(Z_CUT - p2->z)/(long)(p1->z - p2->z));
	p->y = p2->y + (int)((long)(p1->y - p2->y)*(long)(Z_CUT - p2->z)/(long)(p1->z - p2->z));
	p->z = Z_CUT;
}
static int conv23( Point2 *p, Point *q ){
	long x, y;
	x = (long)((double)q->x * (double)RANGE_L_X  / (double)q->z);
	y = (long)((double)q->y * (double)RANGE_L_Y / (double)q->z);

	if( x < -20000 ) p->x = SCREEN_OX + 20000;
	else if( x > 20000 ) p->x = SCREEN_OX - 20000;
	else p->x = SCREEN_OX - x;

	if( y < -20000 ) p->y = SCREEN_OY + 20000;
	else if( y > 20000 ) p->y = SCREEN_OY - 20000;
	else p->y = SCREEN_OY - y;
	return 0;
}

#define pset3(pq1,pq2,pq3,qc,qdist) \
l_ppolygon[pp_ptr].p1=(pq1); \
l_ppolygon[pp_ptr].p2=(pq2); \
l_ppolygon[pp_ptr].p3=(pq3); \
l_ppolygon[pp_ptr].p4=-1; \
l_ppolygon[pp_ptr].p5=-1; \
l_ppolygon[pp_ptr].c=(qc); \
l_ppolygon[pp_ptr].dist=(qdist); \
pp_ptr++;

#define pset4(pq1,pq2,pq3,pq4,qc,qdist)  \
l_ppolygon[pp_ptr].p1=(pq1); \
l_ppolygon[pp_ptr].p2=(pq2); \
l_ppolygon[pp_ptr].p3=(pq3); \
l_ppolygon[pp_ptr].p4=(pq4); \
l_ppolygon[pp_ptr].p5=-1; \
l_ppolygon[pp_ptr].c=(qc); \
l_ppolygon[pp_ptr].dist=(qdist); \
pp_ptr++;

#define pset5(pq1,pq2,pq3,pq4,pq5,qc,qdist) \
l_ppolygon[pp_ptr].p1=(pq1); \
l_ppolygon[pp_ptr].p2=(pq2); \
l_ppolygon[pp_ptr].p3=(pq3); \
l_ppolygon[pp_ptr].p4=(pq4); \
l_ppolygon[pp_ptr].p5=(pq5); \
l_ppolygon[pp_ptr].c=(qc); \
l_ppolygon[pp_ptr].dist=(qdist); \
pp_ptr++;

static void draw_sub( int i )
{
	Point *p1, *p2, *p3, *p4, pp1, pp2;
	Point2 qq1, qq2;
	int q1, q2, q3, q4;
	Point *p;
	int n1, n2, n3, n4;
	int dist;

	n1 = l_polygon[i].p1;
	if( l_projected[n1] == 0 ){
		p = l_point + n1;
		rotate( &(l_rot[n1].z), &(l_rot[n1].x), p->x - mx, p->y - my, 360 - marg );
		l_rot[n1].y = p->z - mz;
		l_projected[n1] = 1;
	}
	n2 = l_polygon[i].p2;
	if( l_projected[n2] == 0 ){
		p = l_point + n2;
		rotate( &(l_rot[n2].z), &(l_rot[n2].x), p->x - mx, p->y - my, 360 - marg );
		l_rot[n2].y = p->z - mz;
		l_projected[n2] = 1;
	}
	n3 = l_polygon[i].p3;
	if( l_projected[n3] == 0 ){
		p = l_point + n3;
		rotate( &(l_rot[n3].z), &(l_rot[n3].x), p->x - mx, p->y - my, 360 - marg );
		l_rot[n3].y = p->z - mz;
		l_projected[n3] = 1;
	}
	n4 = l_polygon[i].p4;
	if( l_projected[n4] == 0 ){
		p = l_point + n4;
		rotate( &(l_rot[n4].z), &(l_rot[n4].x), p->x - mx, p->y - my, 360 - marg );
		l_rot[n4].y = p->z - mz;
		l_projected[n4] = 1;
	}

	dist = (l_rot[n1].z + l_rot[n3].z) / 2;

	p1 = l_rot + l_polygon[i].p1;
	p2 = l_rot + l_polygon[i].p2;
	p3 = l_rot + l_polygon[i].p3;
	p4 = l_rot + l_polygon[i].p4;

	if( p1->z < p2->z && p1->z < p3->z && p1->z < p4->z )
		{ q1 = n1; q2 = n2; q3 = n3; q4 = n4; }
	else if( p2->z < p3->z && p2->z < p4->z )
		{ q1 = n2; q2 = n3; q3 = n4; q4 = n1; }
	else if( p3->z < p4->z )
		{ q1 = n3; q2 = n4; q3 = n1; q4 = n2; }
	else 
		{ q1 = n4; q2 = n1; q3 = n2; q4 = n3; }

	if( l_rot[q1].z < Z_CUT ){
		if( l_rot[q2].z > Z_CUT ){
			if( l_rot[q4].z > Z_CUT ){
				/* 1-2, 2, 3, 4, 4-1 */
				z_cut( &pp1, l_rot + q2, l_rot + q1 );
				z_cut( &pp2, l_rot + q4, l_rot + q1 );
				if( l_projected[q2] < 2 ){
					conv23( l_point2 + q2, l_rot + q2 );
					l_projected[q2] = 2;
				}
				if( l_projected[q3] < 2 ){
					conv23( l_point2 + q3, l_rot + q3 );
					l_projected[q3] = 2;
				}
				if( l_projected[q4] < 2 ){
					conv23( l_point2 + q4, l_rot + q4 );
					l_projected[q4] = 2;
				}
				conv23( &qq1, &pp1 );
				conv23( &qq2, &pp2 );
				l_point2[l_point2_num] = qq1;
				l_point2[l_point2_num+1] = qq2;
				pset5( l_point2_num, q2, q3, q4, l_point2_num+1, l_polygon[i].c, dist );
				l_point2_num += 2;
			}
			else{
				if( l_rot[q3].z > Z_CUT ){
					/* 1-2 2 3 3-4 */
					z_cut( &pp1, l_rot + q2, l_rot + q1 );
					z_cut( &pp2, l_rot + q3, l_rot + q4 );
					if( l_projected[q2] < 2 ){
						conv23( l_point2 + q2, l_rot + q2 );
						l_projected[q2] = 2;
					}
					if( l_projected[q3] < 2 ){
						conv23( l_point2 + q3, l_rot + q3 );
						l_projected[q3] = 2;
					}
					conv23( &qq1, &pp1 );
					conv23( &qq2, &pp2 );
					l_point2[l_point2_num] = qq1;
					l_point2[l_point2_num+1] = qq2;
					pset4( l_point2_num, q2, q3, l_point2_num+1, l_polygon[i].c , dist);
					l_point2_num += 2;
				}
				else{
					/* 1-2 2 2-3 */
					z_cut( &pp1, l_rot + q2, l_rot + q1 );
					z_cut( &pp2, l_rot + q2, l_rot + q3 );
					if( l_projected[q2] < 2 ){
						conv23( l_point2 + q2, l_rot + q2 );
						l_projected[q2] = 2;
					}
					conv23( &qq1, &pp1 );
					conv23( &qq2, &pp2 );
					l_point2[l_point2_num] = qq1;
					l_point2[l_point2_num+1] = qq2;
					pset3( l_point2_num, q2, l_point2_num+1, l_polygon[i].c, dist );
					l_point2_num += 2;
				}
			}
		}
		else if( l_rot[q4].z > Z_CUT ){
			if( l_rot[q3].z > Z_CUT ){
				/* 2-3 3 4 4-1 */
				z_cut( &pp1, l_rot + q3, l_rot + q2 );
				z_cut( &pp2, l_rot + q4, l_rot + q1 );
				if( l_projected[q3] < 2 ){
					conv23( l_point2 + q3, l_rot + q3 );
					l_projected[q3] = 2;
				}
				if( l_projected[q4] < 2 ){
					conv23( l_point2 + q4, l_rot + q4 );
					l_projected[q4] = 2;
				}
				conv23( &qq1, &pp1 );
				conv23( &qq2, &pp2 );
				l_point2[l_point2_num] = qq1;
				l_point2[l_point2_num+1] = qq2;
				pset4( l_point2_num, q3, q4, l_point2_num+1, l_polygon[i].c, dist );
				l_point2_num += 2;
			}
			else{
				/* 3-4 4 4-1 */
				z_cut( &pp1, l_rot + q4, l_rot + q3 );
				z_cut( &pp2, l_rot + q4, l_rot + q1 );
				if( l_projected[q4] < 2 ){
					conv23( l_point2 + q4, l_rot + q4 );
					l_projected[q4] = 2;
				}
				conv23( &qq1, &pp1 );
				conv23( &qq2, &pp2 );
				l_point2[l_point2_num] = qq1;
				l_point2[l_point2_num+1] = qq2;
				pset3( l_point2_num, q4, l_point2_num+1, l_polygon[i].c, dist );
				l_point2_num += 2;
			}
		}
		else if( l_rot[q3].z > Z_CUT ){
			/* 2-3 3 3-4 */
			z_cut( &pp1, l_rot + q3, l_rot + q2 );
			z_cut( &pp2, l_rot + q3, l_rot + q4 );
			if( l_projected[q3] < 2 ){
				conv23( l_point2 + q3, l_rot + q3 );
				l_projected[q3] = 2;
			}
			conv23( &qq1, &pp1 );
			conv23( &qq2, &pp2 );
			l_point2[l_point2_num] = qq1;
			l_point2[l_point2_num+1] = qq2;
			pset3( l_point2_num, q3, l_point2_num+1, l_polygon[i].c, dist );
			l_point2_num += 2;
		}
		else {
			/* no */
		}
	}
	else{
		if( l_projected[q1] < 2 ){
			conv23( l_point2 + q1, l_rot + q1 );
			l_projected[q1] = 2;
		}
		if( l_projected[q2] < 2 ){
			conv23( l_point2 + q2, l_rot + q2 );
			l_projected[q2] = 2;
		}
		if( l_projected[q3] < 2 ){
			conv23( l_point2 + q3, l_rot + q3 );
			l_projected[q3] = 2;
		}
		if( l_projected[q4] < 2 ){
			conv23( l_point2 + q4, l_rot + q4 );
			l_projected[q4] = 2;
		}
		pset4( q1, q2, q3, q4, l_polygon[i].c, dist );
	}
}

static int s_comp( const void *p1, const void *p2 )
{
	return ((ProjectedObj *)p2)->dist - ((ProjectedObj *)p1)->dist;
}
static int l_comp( const void *p1, const void *p2 )
{
	return ((ProjectedPolygon *)p2)->dist - ((ProjectedPolygon *)p1)->dist;
}

void screen_draw_sobj()
{
	int i, j, n, x, y;
	int sx, sy, sz, sarg, esarg;
	int ox, oy;
	Polygon *p;
	Point q1, q2, q3, q4;
	Point2 r1, r2, r3, r4;
	Point nv;
	OBJ *op;
	int pon;

	for( i = 0 ; i < MAX_S_POINT ; i++ ) s_rot_flag[i] = 0;

	sarg = 360 - marg;

	ox = mx >> 6;
	oy = my >> 6;
	n = (marg + 5) / 10;
	if( n >= 36 ) n -= 36;
	if( n < 0 ) n += 36;
	j = 0;
	for( i = 0 ; i < 50 ; i += 2 ){
		x = rot_table[n][i] + ox;
		y = rot_table[n][i+1] + oy;
		if( !((x & 0xffc0) || (y & 0xffe0)) ){
			if( (op = s_obj[x][y] ) != NULL ){
				for( ; op != NULL ; op = op->next ){
					s_po[j].x = op->x;
					s_po[j].y = op->y;
					s_po[j].z = op->z;
					s_po[j].c = op->c;
					sz = s_po[j].x - mx;
					sx = s_po[j].y - my;
					rotate( &(s_po[j].sz), &(s_po[j].sx), sz, sx, sarg );
					if( s_po[j].sz < S_OBJ_CUT ) continue;
					s_po[j].sy = s_po[j].z - mz;
					s_po[j].dist = s_po[j].sz;
					j++;
				}
			}
		}
	}
	s_po[j].x = ex;
	s_po[j].y = ey;
	s_po[j].z = ez;
	s_po[j].c = s_obj_class;
	s_po[j].sx = erx;
	s_po[j].sy = ery;
	s_po[j].sz = erz;
	if( s_po[j].sz > S_OBJ_CUT_0 && s_po[j].sz < S_OBJ_DIST_0 ){
		s_po[j].dist = s_po[j].sz;
		j++;
	}
	pon = j;
	qsort( s_po, pon, sizeof(ProjectedObj), s_comp );

	for( i = 0 ; i < pon ; i++ ){
		sz = s_po[i].sz;
		sx = s_po[i].sx;
		sy = s_po[i].sy;
		if( s_po[i].c == s_obj_class ){
			esarg = sarg + earg;
			if( esarg >= 360 ) esarg -= 360;
		}
		else esarg = sarg;
		for( j = 0 ; j < s_po[i].c->p_n ; j++ ){
			p = s_po[i].c->p + j;
			if( p->attr > 0 ){
				nv.x = s_po[i].x - mx;
				nv.y = s_po[i].y - my;
				nv.z = s_po[i].z - mz;
				if( s_po[i].c == s_obj_class ){
					rotate_d( &(nv.x), &(nv.y), 360 - earg );
				}
				if( l_product3( nv.x, nv.y, nv.z, p->nv.x, p->nv.y, p->nv.z ) < 0){
					continue;
				}
			}
			if( s_rot_flag[p->p1] == 0 ){
				rotate( &(q1.z), &(q1.x),
					s_point[p->p1].x, s_point[p->p1].y, esarg );
				q1.y = s_point[p->p1].z;
				s_p[p->p1] = q1;
				s_rot_flag[p->p1] = 1;
			} else q1 = s_p[p->p1];
			q1.x += sx;
			q1.y += sy;
			q1.z += sz;
			if( s_rot_flag[p->p2] == 0 ){
				rotate( &(q2.z), &(q2.x),
					s_point[p->p2].x, s_point[p->p2].y, esarg );
				q2.y = s_point[p->p2].z;
				s_p[p->p2] = q2;
				s_rot_flag[p->p2] = 1;
			} else q2 = s_p[p->p2];
			q2.x += sx;
			q2.y += sy;
			q2.z += sz;
			if( s_rot_flag[p->p3] == 0 ){
				rotate( &(q3.z), &(q3.x),
					s_point[p->p3].x, s_point[p->p3].y, esarg );
				q3.y = s_point[p->p3].z;
				s_p[p->p3] = q3;
				s_rot_flag[p->p3] = 1;
			} else q3 = s_p[p->p3];
			q3.x += sx;
			q3.y += sy;
			q3.z += sz;
			if( p->p1 != p->p4 ){
				if( s_rot_flag[p->p4] == 0 ){
					rotate( &(q4.z), &(q4.x),
						s_point[p->p4].x, s_point[p->p4].y, esarg );
					q4.y = s_point[p->p4].z;
					s_p[p->p4] = q4;
					s_rot_flag[p->p4] = 1;
				} else q4 = s_p[p->p4];
				q4.x += sx;
				q4.y += sy;
				q4.z += sz;
			}
			conv23( &r1, &q1 );
			conv23( &r2, &q2 );
			conv23( &r3, &q3 );
			if( p->p1 != p->p4 ) conv23( &r4, &q4 );

			if( p->p1 == p->p4 ){
				polygon3( screen_pixmap, &r1, &r2, &r3, p->c );
			}
			else{
				polygon4( screen_pixmap, &r1, &r2, &r3, &r4, p->c );
			}
		}
	}
}


void screen_draw_lobj()
{
	int i, j, n, *ip, k;
	int ox, oy, x, y;
	int sx, sy, sz;
	Point *nv;
	ProjectedPolygon *pp;

	pp_ptr = 0;
	l_point2_num = l_point_num;

	for( i = 0 ; i < MAX_L_POINT ; i++ ) l_projected[i] = 0;
	for( i = 0 ; i < MAX_L_POLY ; i++ ) l_polygon_drawn[i] = 0;

	ox = mx >> 7;
	oy = my >> 7;
	n = (marg + 5) / 10;
	if( n >= 36 ) n -= 36;
	if( n < 0 ) n += 36;

	for( j = 0 ; j < 50 ; j += 2 ){
		x = rot_table[n][j] + ox;
		y = rot_table[n][j+1] + oy;
		if( !((x & 0xffe0) || (y & 0xfff0)) ){
			if( (ip = l_obj[x][y] ) != NULL ){
				for( ; (*ip) >= 0 ; ip++ ){
					if( l_polygon_drawn[*ip] == 0 ){
						if( l_polygon[*ip].attr == 1 )
							draw_sub( *ip );
						else{
							k = l_polygon[*ip].p1;
							sx = mx - l_point[k].x;
							sy = my - l_point[k].y;
							sz = mz - l_point[k].z;
							nv = &(l_polygon[*ip].nv);
							if( l_product3( sx, sy, sz, nv->x, nv->y, nv->z ) >= 0)
								draw_sub( *ip );
							l_polygon_drawn[*ip] = 1;
						}
					}
				}
			}
		}
	}
	qsort( l_ppolygon, pp_ptr, sizeof(ProjectedPolygon), l_comp );
	for( i = 0 ; i < pp_ptr ; i++ ){
		pp = l_ppolygon + i;
		if( pp->p5 == -1 ){
			if( pp->p4 == -1 ){
				polygon3( screen_pixmap, l_point2 + pp->p1, l_point2 + pp->p2, l_point2 + pp->p3, pp->c );
			}
			else{
				polygon4( screen_pixmap, l_point2 + pp->p1, l_point2 + pp->p2, l_point2 + pp->p3, l_point2 + pp->p4, pp->c );
			}
		}
		else{
			polygon5( screen_pixmap, l_point2 + pp->p1, l_point2 + pp->p2, l_point2 + pp->p3, l_point2 + pp->p4, l_point2 + pp->p5, pp->c );
		}
	}
}

void screen_init()
{
	screen_pixmap = XCreatePixmap( display, window,
		SCREEN_WX, SCREEN_WY, depth );
}
void screen_clear()
{
	XFillRectangle( display, screen_pixmap, gc_table[SKY_COLOR],
		0, 0, SCREEN_WX, SCREEN_SKY_Y );
	XFillRectangle( display, screen_pixmap, gc_table[GROUND_COLOR],
		0, SCREEN_SKY_Y, SCREEN_WX, SCREEN_GROUND_Y );
}

void screen_draw_message( str )
	char *str;
{
	XDrawString( display, screen_pixmap, gc_table[SCREEN_MES_COLOR],
		SCREEN_MES_X, SCREEN_MES_Y, str, strlen( str ) );

}

void screen_draw_staff()
{
	int i;
	for( i = 0 ; i < STAFF_NUM ; i++ ){
		XDrawString( display, screen_pixmap,
			gc_table[SCREEN_STAFF_COLOR],
			SCREEN_STAFF_X, SCREEN_STAFF_Y + SCREEN_STAFF_WY * i,
			staff_name[i], strlen( staff_name[i] ) );
	}
}
