#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "screen.h"
#include "window.h"
#include "course.h"

Pixmap course;

COURSE course_data[COURSE_DATA_MAX];
int course_data_num;

Polygon l_polygon[MAX_L_POLY];
int l_polygon_num;
Point l_point[MAX_L_POINT];
int l_point_num;

void course_init()
{
	course = XCreatePixmap( display, window, COURSE_WX, COURSE_WY, DefaultDepth( display, 0 ) );
}

void course_draw()
{
	int i;
	int n1, n2, n3, n4;
	Point2 p1, p2, p3, p4;
	XFillRectangle( display, course, gc_table[COURSE_BG],
		0, 0, COURSE_WX, COURSE_WY );
	for( i = 0 ; i < l_polygon_num ; i++ ){
		if( l_polygon[i].c == COURSE_ROAD_COLOR ){
			n1 = l_polygon[i].p1;
			n2 = l_polygon[i].p2;
			n3 = l_polygon[i].p3;
			n4 = l_polygon[i].p4;
			p1.x = l_point[n1].x / 16 - 64;
			p1.y = 150 - l_point[n1].y / 16;
			p2.x = l_point[n2].x / 16 - 64;
			p2.y = 150 - l_point[n2].y / 16;
			p3.x = l_point[n3].x / 16 - 64;
			p3.y = 150 - l_point[n3].y / 16;
			p4.x = l_point[n4].x / 16 - 64;
			p4.y = 150 - l_point[n4].y / 16;
			polygon4( course, &p1, &p2, &p3, &p4, COURSE_FG );
		}
	}
}
