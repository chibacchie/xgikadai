#define GROUND_COLOR 4
#define SKY_COLOR 5

#define SCREEN_WX 600
#define SCREEN_WY 300
#define SCREEN_SKY_Y 150
#define SCREEN_GROUND_Y 150

#define MAX_S_POINT 512
#define MAX_S_OBJ 256
#define MAX_S_POLY 128

#define Z_CUT 3

typedef struct _projectedpolygon{
	int p1, p2, p3, p4, p5;
	int c;
	int dist;
} ProjectedPolygon;

typedef struct _obj_class{
	int p_n;
	int o_n;
	Polygon *p;
	Point *o;
} OBJ_CLASS;

typedef struct _obj{
	int x, y, z;
	OBJ_CLASS *c;
	struct _obj *next;
} OBJ;

typedef struct _projected_obj{
	int x, y, z, dist;
	int sx, sy, sz;
	OBJ_CLASS *c;
} ProjectedObj;

extern Pixmap screen_pixmap;
extern GC gc_table[16];

extern int *l_obj[32][16];
extern int l_obj_array[];

extern OBJ_CLASS s_obj_class[32];
extern OBJ *s_obj[64][32];
extern OBJ s_obj_tbl[MAX_S_OBJ];
extern int s_obj_tbl_num;
extern Polygon s_polygon[MAX_S_POLY];
extern int s_polygon_num;
extern Point s_point[MAX_S_POINT];
extern int s_point_num;

void screen_init( void );
void screen_clear( void );
void screen_draw_sobj( void );
void screen_draw_lobj( void );
