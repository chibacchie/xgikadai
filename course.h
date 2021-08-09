#define COURSE_WX 150
#define COURSE_WY 150

#define COURSE_BG 3
#define COURSE_FG 0xc
#define COURSE_ROAD_COLOR 0xf
#define COURSE_DATA_MAX 100

#define MAX_L_POINT 800
#define MAX_L_POINT2 1200
#define MAX_L_POLY 200

extern Pixmap course;

void course_init();
void course_draw();

typedef struct _course{
	int ox, oy, oz, width;
	int x, y, z;
	long r2;
	int xn, yn;
	int bx, by, bxn, byn;
} COURSE;

extern COURSE course_data[COURSE_DATA_MAX];
extern int course_data_num;

extern Polygon l_polygon[MAX_L_POLY];
extern int l_polygon_num;
extern Point l_point[MAX_L_POINT];
extern int l_point_num;

