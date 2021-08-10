#include <stdio.h>
#include <stdlib.h>

#include "gr.h"
#include "grx.h"
#include "window.h"
#include "file.h"
#include "title.h"
#include "record.h"

void press_left(), press_right(), press_mid();
void release_left(), release_mid(), release_right();
void press_left_key(), press_right_key(), press_space_key();
void release_left_key(), release_right_key(), release_space_key();
void window_redraw(), quit();

static XtActionsRec actions[] = {
{ "PressLeft", press_left },
{ "PressMid", press_mid },
{ "PressRight", press_right },
{ "ReleaseLeft", release_left },
{ "ReleaseMid", release_mid },
{ "ReleaseRight", release_right },
{ "PressLeftKey", press_left_key },
{ "PressSpaceKey", press_space_key },
{ "PressRightKey", press_right_key },
{ "ReleaseLeftKey", release_left_key },
{ "ReleaseSpaceKey", release_space_key },
{ "ReleaseRightKey", release_right_key },
{ "quit", quit },
{ "Refresh", window_redraw }, };
static char defaultTranslations[] = "#override \n\
        <Expose>:Refresh() \n\
        <Btn1Down>:PressLeft() \n\
        <Btn2Down>:PressMid() \n\
        <Btn3Down>:PressRight() \n\
        <Btn1Up>:ReleaseLeft() \n\
        <Btn2Up>:ReleaseMid() \n\
        <Btn3Up>:ReleaseRight() \n\
        <Key>Left:PressLeftKey() \n\
        <Key>KP_Left:PressLeftKey() \n\
        <Key>space:PressSpaceKey() \n\
        <Key>Right:PressRightKey() \n\
        <Key>KP_Right:PressRightKey() \n\
        <Key>q:quit() \n\
        <KeyUp>Left:ReleaseLeftKey() \n\
        <KeyUp>KP_Left:ReleaseLeftKey() \n\
        <KeyUp>space:ReleaseSpaceKey() \n\
        <KeyUp>Right:ReleaseRightKey() \n\
        <KeyUp>KP_Right:ReleaseRightKey() ";

Widget topLevel, game_widget;
XtAppContext app_context;

char font_name[MAX_FONT_NAME] = "-*-fixed-medium-r-*--20-*-*-*-*-*-*";
int wait_val = DEFAULT_WAIT;
int voice_flag = 1;

void usage()
{
	fprintf( stderr, "X Gikadai Racer ver. 1.0\n" );
	fprintf( stderr, "Copyright (c) TUT Computer Club,1996\n" );
	fprintf( stderr, "Programmed by Hideo Tomizu\n\n" );
	fprintf( stderr, "option:\n" );
	fprintf( stderr, " -f font_name\n" );
	fprintf( stderr, " -w wait(%d-%d)\n", MIN_WAIT, MAX_WAIT );
	fprintf( stderr, " -nv\n" );
	exit( 1 );
}

int main( int argc, char *argv[] )
{
	int i;
	XtTranslations new_translations;
	topLevel = XtVaAppInitialize( &app_context, "XGikadai",
		NULL, 0, &argc, argv, NULL, NULL );
	if( argc > 1 ){
		for( i = 1 ; i < argc ; i++ ){
			if( strcmp( argv[i], "-f" ) == 0 && argc > i+1 ){
				strcpy( font_name, argv[i+1] );
				i++;
			}
			else if( strcmp( argv[i], "-nv" ) == 0 ){
				voice_flag = 0;
			}
			else if( strcmp( argv[i], "-w" ) == 0 && argc > i+1 ){
				wait_val = atoi( argv[i+1] );
				if( wait_val < MIN_WAIT || wait_val > MAX_WAIT )
					usage();
				i++;
			}
			else usage();
		}
	}
        XtAppAddActions( app_context, actions, XtNumber(actions) );
        game_widget = XtVaCreateManagedWidget( "game", simpleWidgetClass,
		topLevel, XtNwidth, WINDOW_WX, XtNheight, WINDOW_WY, NULL);
	new_translations = XtParseTranslationTable( defaultTranslations );
	XtOverrideTranslations( game_widget, new_translations );
	XtRealizeWidget( topLevel );

	calc_init();
	init();
	read_course();
	read_map();
	read_sobj();
	read_time();
	game_mode = MODE_TITLE;
	title_start();
	XtAppMainLoop( app_context );

	return 0;
}
