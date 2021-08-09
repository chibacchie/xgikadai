#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "screen.h"
#include "window.h"

Display *display;
int screen, depth;
GC gc;
XEvent Ev;
GC gc_table[16];
int current_gc;

static short color_table[16][3] = {
{ 0x0000, 0x0000, 0x0000 }, { 0x0000, 0x0000, 0xf000 },
{ 0x0000, 0x6000, 0x0000 }, { 0x0000, 0x9000, 0x0000 },
{ 0x0000, 0xc000, 0x0000 }, { 0x0000, 0xf000, 0xf000 },
{ 0x8000, 0x8000, 0xf000 }, { 0xf000, 0xf000, 0xf000 },
{ 0xb000, 0x6000, 0x4000 }, { 0x0000, 0x0000, 0xf000 },
{ 0x2000, 0x8000, 0x2000 }, { 0xe000, 0xe000, 0xe000 },
{ 0xc000, 0xc000, 0xc000 }, { 0xa000, 0xa000, 0xa000 },
{ 0x6000, 0x6000, 0x6000 }, { 0x8000, 0x8000, 0x8000 }
};

/*============= graphic ===========*/

void SetColor( n )
	int n;
{
	current_gc = n;
}
void polygon3( Window w, Point2 *p1, Point2 *p2, Point2 *p3, int c )
{
	XPoint xp[3];
	xp[0].x = p1->x;
	xp[0].y = p1->y;
	xp[1].x = p2->x;
	xp[1].y = p2->y;
	xp[2].x = p3->x;
	xp[2].y = p3->y;
	XFillPolygon( display, w, gc_table[c], xp, 3, Convex, CoordModeOrigin );
}
void polygon4( Window w, Point2 *p1, Point2 *p2, Point2 *p3, Point2 *p4, int c )
{
	XPoint xp[4];
	xp[0].x = p1->x;
	xp[0].y = p1->y;
	xp[1].x = p2->x;
	xp[1].y = p2->y;
	xp[2].x = p3->x;
	xp[2].y = p3->y;
	xp[3].x = p4->x;
	xp[3].y = p4->y;
	XFillPolygon( display, w, gc_table[c], xp, 4, Convex, CoordModeOrigin );
}
void polygon5( Window w, Point2 *p1, Point2 *p2, Point2 *p3, Point2 *p4, Point2 *p5, int c )
{
	XPoint xp[5];
	xp[0].x = p1->x;
	xp[0].y = p1->y;
	xp[1].x = p2->x;
	xp[1].y = p2->y;
	xp[2].x = p3->x;
	xp[2].y = p3->y;
	xp[3].x = p4->x;
	xp[3].y = p4->y;
	xp[4].x = p5->x;
	xp[4].y = p5->y;
	XFillPolygon( display, w, gc_table[c], xp, 5, Convex, CoordModeOrigin );
}

/*============= event ===========*/
static int button_mid = 0, button_left = 0, button_right = 0;
void ClearEventInfo(){
	button_mid = 0;
	button_left = 0;
	button_right = 0;
}
void press_left( Widget w, XButtonEvent *ev ){
	button_left = 1;
	if( game_mode == MODE_TITLE ){
		title_button_press( ev->x, ev->y );
	}
	else if( game_mode == MODE_RECORD ){
		record_button_press( ev->x, ev->y );
	}
	else if( game_mode == MODE_ENTER ){
		enter_button_press( ev->x, ev->y );
	}
	else if( game_mode == MODE_STOP ){
		game_mode = MODE_TITLE;
		title_start();
	}
}
void press_mid(){ button_mid = 1; }
void press_right(){ button_right = 1; }
void release_left(){ button_left = 0; }
void release_mid(){ button_mid = 0; }
void release_right(){ button_right = 0; }
void press_left_key(){ button_left = 1; }
void press_space_key(){ button_mid = 1; }
void press_right_key(){ button_right = 1; }
void release_left_key(){ button_left = 0; }
void release_space_key(){ button_mid = 0; }
void release_right_key(){ button_right = 0; }

int isLeft()
{
	return button_left;
}
int isMid()
{
	return button_mid;
}
int isRight()
{
	return button_right;
}
/*========== Initialize ==========*/

void init()
{
	int i;
	XColor xc;
	XFontStruct *font;
	display = XtDisplay( topLevel );
	window = XtWindow( topLevel );
	screen = XScreenNumberOfScreen( XtScreen( topLevel ) );
	depth = DefaultDepth( display, screen );
	gc = XCreateGC( display, window, 0, 0 );
	if( (font = XLoadQueryFont( display, font_name )) == NULL ){
		fprintf( stderr, "BadFontName: %s\n", font_name );
		exit( 1 );
	}
	XSetFont( display, gc, font->fid );
	for( i = 0 ; i < 16 ; i++ ){
		gc_table[i] = XCreateGC( display, window, 0, 0 );
		XCopyGC( display, gc, 0, gc_table[i] );
		XSetFont( display, gc_table[i], font->fid );
		xc.red = color_table[i][0];
		xc.green = color_table[i][1];
		xc.blue = color_table[i][2];
		XAllocColor( display, DefaultColormap( display, screen ), &xc);
		XSetForeground( display, gc_table[i], xc.pixel );
	}
	XSelectInput( display, window, ButtonPressMask | ButtonReleaseMask | ExposureMask );
	ClearEventInfo();
	window_init();
	window_map();
	title_init();
	screen_init();
	record_init();
	course_init();
	XFlush( display );
}

/*========== Read Image Data (MAG) ==========*/
#define BIT(add,bit) (((1<<(bit))&*(add))!=0)
#define HALF(add,bit) (((*(add))>>(bit*4))&0xf)
#define BYTE(add) (0xff&*(add))
#define WORD(add) (BYTE(add)|((BYTE((add)+1))<<8))
#define LONG(add) (WORD(add)|((WORD((add)+2))<<16))

unsigned long get_color( Display *d, Colormap c_map, int r, int g, int b )
{
	int i;
	XColor xc;

	xc.red = r << 8;
	xc.green = g << 8;
	xc.blue = b << 8;
	if( XAllocColor( d, c_map, &xc ) ){
		return xc.pixel;
	}
	return 0;
}


Pixmap image_to_pixmap( unsigned char *data, int *width_ret, int *height_ret )
{
	int width, height;
	int i, j;
	int c, l;
	int ptr;
	char *img, *vram;
	Pixmap p;
	XImage *img_data;
	Pixel color[16];

	ptr = 0;
/* Get Width & Height */
	width = data[0] + data[1] * 256;
	height = data[2] + data[3] * 256;
	img = malloc( width * height );
/* Get Palette */
	for( i = 0 ; i < 16 ; i++ ){
		color[i] = get_color( display,
			DefaultColormap( display, screen ),
			data[i*3+5], data[i*3+4], data[i*3+6] );
	}
/* Get Pixel */
	ptr = 4 + 16 * 3;
	i = 0;
	j = 0;
	while( i < height ){
		if( data[ptr] == 0 && data[ptr+1] == 0 ) break;
		if( data[ptr] >= 16 ){
			c = data[ptr] & 0xf;
			l = (data[ptr]>>4) & 0xf;
			ptr++;
		}
		else{
			c = data[ptr];
			if( !(data[ptr+1] & 0x80) ){
				l = data[ptr+1] & 0x7f;
				ptr += 2;
			}
			else{
				l = ((unsigned int)data[ptr+2] << 7) + (data[ptr+1] & 0x7f);
				ptr += 3;
			}
		}
		for( ; l > 0 ; l-- ){
			if( i >= height ) break;
			img[i*width+j] = c;
			j++;
			if( j >= width ){
				j = 0;
				i++;
			}
		}
	}

/* Create Pixmap */
	p = XCreatePixmap( display, window, width, height, depth );
	if( p == BadValue ) return BadValue;
	vram = malloc( width * height * (depth+7)/8 );
	img_data = XCreateImage( display,
		DefaultVisual( display, screen ), depth,
		ZPixmap, 0, vram, width, height, 8, 0 );
	for( i = 0 ; i < height ; i++ ){
		for( j = 0 ; j < width ; j++ ){
			XPutPixel( img_data, j, i, color[img[i*width+j]] );
		}
	}
	XPutImage( display, p, gc, img_data, 0, 0, 0, 0, width, height );
	// XDestroyImage( img_data );
	if( width_ret != NULL ) *width_ret = width;
	if( height_ret != NULL ) *height_ret = height;
	// free( img );
	return p;
}
