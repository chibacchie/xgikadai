#define REST_INI 6000
#define REST_ADD 4000
#define LAP_MAX 3
#define MAX_M_SPEED_INI 300

extern int mx, my, mz, marg;
extern int mx_ini, my_ini, mz_ini, marg_ini;
extern int mccd;
extern int drift;

extern int m_lap, m_lap_disp, m_speed, m_rest;
extern int m_time, m_time_sum, m_best, m_time_rec[10], m_lap_max;
extern int m_rest_ini, m_rest_add;
extern int max_m_speed;
extern long m_l;
extern int ranking;
extern int mdx, mdy, mdarg;
extern int c_x, c_y;
extern int reverse_count;

void mycar_init();
int mycar_move();
