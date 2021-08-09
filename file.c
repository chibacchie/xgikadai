#include <stdio.h>
#include <math.h>

#include "gr.h"
#include "grx.h"
#include "screen.h"
#include "course.h"
#include "mycar.h"
#include "enemy.h"

void read_course()
{
	int i;
	int ox, oy, oz, x, y, z, nx, ny, nz;
	int ow, w, nw;
	int vx, vy, wx, wy;
	extern int line_data[], init_data[];
	int ptr;

	mx_ini = init_data[0] * 4;
	my_ini = init_data[1] * 4;
	mz_ini = CAR_Z;
	marg_ini = init_data[2];

	ptr = 0;
	ox = line_data[ptr++] * 4;
	oy = line_data[ptr++] * 4;
	oz = line_data[ptr++];
	ow = line_data[ptr++] * 4;
	x = line_data[ptr++] * 4;
	y = line_data[ptr++] * 4;
	z = line_data[ptr++];
	w = line_data[ptr++] * 4;
	for( i = 0 ; line_data[ptr] != -1 ; i++ ){
		nx = line_data[ptr++] * 4;
		ny = line_data[ptr++] * 4;
		nz = line_data[ptr++];
		nw = line_data[ptr++] * 4;
		course_data[i].ox = ox;
		course_data[i].oy = oy;
		course_data[i].oz = oz;
		course_data[i].width = ow;
		course_data[i].x = x - ox;
		course_data[i].y = y - oy;
		course_data[i].z = z - oz;
		course_data[i].r2 = (long)(x - ox) * (long)(x - ox) + (long)(y - oy) * (long)(y - oy);
		get_nv( &(course_data[i].xn), &(course_data[i].yn), x - ox, y - oy );
		get_nv( &vx, &vy, nx - x, ny - y );
		wx = vx + course_data[i].xn;
		wy = vy + course_data[i].yn;
		course_data[i].bx = x + wx;
		course_data[i].by = y + wy;
		get_nv( &(course_data[i].bxn), &(course_data[i].byn), -wx, -wy );
		ox = x;
		oy = y;
		oz = z;
		ow = w;
		x = nx;
		y = ny;
		z = nz;
		w = nw;
	}
	course_data_num = i;
}

int add_point( Point *p )
{
	int i;
	for( i = 0 ; i < l_point_num ; i++ ){
		if( l_point[i].x == p->x &&
		    l_point[i].y == p->y &&
		    l_point[i].z == p->z ) return i;
	}
	l_point[i] = *p;
	l_point_num++;
	return i;
}

void read_map()
{
	int i, j, k, l, c, attr;
	Point p1, p2, p3, p4;
	extern int lobj_data[];
	int ptr;

	l_point_num = 0;
	l_polygon_num = 0;
	i = 0;
	ptr = 0;
	while( lobj_data[ptr] != -1 ){
		p1.x = lobj_data[ptr++] * 4;
		p1.y = lobj_data[ptr++] * 4;
		p1.z = lobj_data[ptr++];
		p2.x = lobj_data[ptr++] * 4;
		p2.y = lobj_data[ptr++] * 4;
		p2.z = lobj_data[ptr++];
		p3.x = lobj_data[ptr++] * 4;
		p3.y = lobj_data[ptr++] * 4;
		p3.z = lobj_data[ptr++];
		p4.x = lobj_data[ptr++] * 4;
		p4.y = lobj_data[ptr++] * 4;
		p4.z = lobj_data[ptr++];
		c = lobj_data[ptr++];
		attr = lobj_data[ptr++];
		l_polygon[i].p1 = add_point( &p1 );
		l_polygon[i].p2 = add_point( &p2 );
		l_polygon[i].p3 = add_point( &p3 );
		l_polygon[i].p4 = add_point( &p4 );
		l_polygon[i].c = c;
		l_polygon[i].attr = attr;
		outer_product( &(l_polygon[i].nv), &p4, &p2, &p1 );
		i++;
	}
	l_polygon_num = i;

	k = 0;
	for( i = 0 ; i < 32 ; i++ ){
		for( j = 0 ; j < 16 ; j++ ){
			l_obj[i][j] = NULL;
			for( l = 0 ; l < l_polygon_num ; l++ ){
				c = l_polygon[l].p1;
				if( (l_point[c].x >> 7) == i && (l_point[c].y >> 7) == j ){
					if( l_obj[i][j] == NULL )
						l_obj[i][j] = l_obj_array + k;
					l_obj_array[k] = l;
					k++;
					continue;
				}
				c = l_polygon[l].p2;
				if( (l_point[c].x >> 7) == i && (l_point[c].y >> 7) == j ){
					if( l_obj[i][j] == NULL )
						l_obj[i][j] = l_obj_array + k;
					l_obj_array[k] = l;
					k++;
					continue;
				}
				c = l_polygon[l].p3;
				if( (l_point[c].x >> 7) == i && (l_point[c].y >> 7) == j ){
					if( l_obj[i][j] == NULL )
						l_obj[i][j] = l_obj_array + k;
					l_obj_array[k] = l;
					k++;
					continue;
				}
				c = l_polygon[l].p4;
				if( (l_point[c].x >> 7) == i && (l_point[c].y >> 7) == j ){
					if( l_obj[i][j] == NULL )
						l_obj[i][j] = l_obj_array + k;
					l_obj_array[k] = l;
					k++;
					continue;
				}
				else if( (l_point[c].x >> 7) == i && l_point[c].y < 0 && j == 0 ){
					if( l_obj[i][j] == NULL )
						l_obj[i][j] = l_obj_array + k;
					l_obj_array[k] = l;
					k++;
					continue;
				}
			}
			if( l_obj[i][j] != NULL ){
				l_obj_array[k] = -1;
				k++;
			}
		}
	}
}

#define GROUND_P -120

void read_sobj()
{
	int i, j, k, l, n;
	int x, y;
	int p1, p2, p3, p4, c, a;
	FILE *fp;
	char fn[80];
	int ptr;
	OBJ *p;
	extern int obj_pos_data[];
	extern char obj_point_data[];

	s_point_num = 0;
	s_polygon_num = 0;
	ptr = 0;
	for( i = 0 ; ; i++ ){
		k = s_point_num;
		n = obj_point_data[ptr++];
		for( j = 0 ; j < n ; j++ ){
			s_point[k+j].x = obj_point_data[ptr++];
			s_point[k+j].y = obj_point_data[ptr++];
			s_point[k+j].z = obj_point_data[ptr++];
			s_point[k+j].z -= GROUND_P;
		}
		s_obj_class[i].o_n = j;
		s_obj_class[i].o = s_point + s_point_num;
		l = 0;
		while( obj_point_data[ptr] > 0 ){
			p1 = obj_point_data[ptr++];
			p2 = obj_point_data[ptr++];
			p3 = obj_point_data[ptr++];
			p4 = obj_point_data[ptr++];
			c = obj_point_data[ptr++];
			a = obj_point_data[ptr++];
			s_polygon[s_polygon_num + l].p1 = p1 + k - 1;
			s_polygon[s_polygon_num + l].p2 = p2 + k - 1;
			s_polygon[s_polygon_num + l].p3 = p3 + k - 1;
			s_polygon[s_polygon_num + l].p4 = p4 + k - 1;
			s_polygon[s_polygon_num + l].c = c;
			s_polygon[s_polygon_num + l].attr = a;
			outer_product( &(s_polygon[s_polygon_num + l].nv), &(s_point[p1+k-1]), &(s_point[p3+k-1]), &(s_point[p2+k-1]) );
			l++;
		}
		s_obj_class[i].p_n = l;
		s_obj_class[i].p = s_polygon + s_polygon_num;

		s_point_num = k + j;
		s_polygon_num += l;

		if( obj_point_data[ptr] < 0 ) break;
		ptr++;
	}

	for( i = 0 ; i < 64 ; i++ )
		for( j = 0 ; j < 32 ; j++ )
			s_obj[j][i] = NULL;

	i = 0;
	ptr = 0;
	while( obj_pos_data[ptr] != -1 ){
		s_obj_tbl[i].x = obj_pos_data[ptr++] * 4;
		s_obj_tbl[i].y = obj_pos_data[ptr++] * 4;
		s_obj_tbl[i].z = obj_pos_data[ptr++];
		n = obj_pos_data[ptr++];
		s_obj_tbl[i].c = s_obj_class + n;

		if( i > 0 ){
			x = s_obj_tbl[i].x >> 6;
			y = s_obj_tbl[i].y >> 6;
			if( s_obj[x][y] == NULL )
				s_obj[x][y] = s_obj_tbl + i;
			else{
				p = s_obj[x][y];
				while( p->next != NULL ) p = p->next;
				p->next = s_obj_tbl + i;
			}
			s_obj_tbl[i].next = NULL;
		}
		i++;
	}
	s_obj_tbl_num = i;
}
