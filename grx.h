#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Simple.h>

extern Widget topLevel, game_widget;
extern XtAppContext app_context;
extern int game_mode;
extern Display *display;
extern int screen, depth;
extern GC gc;
extern XEvent Ev;
extern GC gc_table[16];
extern int current_gc;

#define TIMEOUT 120

void press_left(), press_right(), press_mid();
void release_left(), release_mid(), release_right();
void game_ready( XtPointer xp, XtIntervalId *id );
void mainloop( XtPointer xp, XtIntervalId *id );
void init( void );
void SetColor( int );
void polygon3( Window w, Point2 *p1, Point2 *p2, Point2 *p3, int c );
void polygon4( Window w, Point2 *p1, Point2 *p2, Point2 *p3, Point2 *p4, int c );
void polygon5( Window w, Point2 *p1, Point2 *p2, Point2 *p3, Point2 *p4, Point2 *p5, int c
 );
void ClearEventInfo();
void GetEvent();
int isLeft(), isMid(), isRight();
Pixmap mag_to_pixmap( Display *d, Window w, int scr, int depth, GC gc, char *data, int *width_ret, int *height_ret );
