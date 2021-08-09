extern int ex, ey, ez, earg, eccd;
extern int epx1, epy1, epccd, epx2, epy2;
extern int erx, ery, erz;
extern int e_speed;
extern int e_lap;
extern long e_l;

#define EN_LEN 360.0

#define EN_V_POINT_1 750
#define EN_V_POINT_2 850

#define EN_A_X 6
#define EN_A_Z 8
#define EN_A_LEN 32

void en_init();
double en_calc_handle( long l1, long l2 );
void en_move();
void crash();
