#define MAX_NAME_LEN 256

extern Pixmap record_pixmap;
extern int record_pixmap_width, record_pixmap_height;
extern int best_time[10];
extern int best_lap;
extern char best_time_name[10][MAX_NAME_LEN];
extern char best_lap_name[MAX_NAME_LEN];

void record_init();
void record_redraw();
void record_button_press( int x, int y );
void enter_redraw();
void enter_init();
void enter_button_press( int x, int y );
int read_time();
int write_time();