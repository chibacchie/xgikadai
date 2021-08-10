/*============== constants ==============*/

#define FXP_UNIT_LEN 64
#define FXP_UNIT_LEN_BITS 6
#define CAR_Z 3

#define MODE_TITLE 1
#define MODE_GAME 2
#define MODE_RECORD 3
#define MODE_STOP 4
#define MODE_ENTER 5
#define MODE_READY 6

#define MAX_FONT_NAME 1024

#define MAX_WAIT 256
#define MIN_WAIT 32
#define DEFAULT_WAIT 80

/*=========== type deine ==========*/
typedef struct _point{
	int x, y;
} Point2;

typedef struct _point3{
	int x, y, z;
} Point;

typedef struct _polygon{
	int p1, p2, p3, p4;
	char c;
	char attr;
	Point nv;
} Polygon;


/*=========== global variables ==========*/

extern int game_over_flag, game_clear_flag, game_break_flag, game_start_flag;
extern int game_ready_count;

extern int sin_table[];
extern char rot_table[36][50];
extern char font_name[MAX_FONT_NAME];
extern int wait_val;
extern int voice_flag;

/*=========== functions ==========*/

void print_string( int x, int y, unsigned char *str, int atr );
void print_lap();
void print_time();
void print_rest();
void print_speed();
void print_ranking();
void draw_pos_init();
void draw_pos();
void calc_ranking();
void enter_name( char *str );
void draw_course();
void game_redraw();
void game_init();
void game();

void calc_init();
double dd_sin( double n );
double dd_cos( double n );
void outer_product( Point *n, Point *p1, Point *p2, Point *o );
void get_nv( int *, int *, int, int );
void rotate_d( int *, int *, int );
void rotate( int *, int *, int, int, int );

/*=========== calculation ==========*/

#define i_sign(x)	(((x)<0)?-1:((x==0)?0:1))
#define swap2(a,b,type) {type w;w=a;a=b;b=w;}
#define l_product(ax,ay,bx,by) ((long)(ax)*(long)(bx)+(long)(ay)*(long)(by))
#define l_product3(ax,ay,az,bx,by,bz) ((long)(ax)*(long)(bx)+(long)(ay)*(long)(by)+(long)(az)*(long)(bz))
#define lsin(x) (sin_table[(x)+180])
#define lcos(x) (sin_table[(x)+270])
