


#ifndef __amiga_stuff_h__
#define __amiga_stuff_h__

#include "common.h"

#define ALLEGRO_FILE FILE

#define strcpy_s strcpy
#define strcat_s strcat

#define ALLEGRO_ALIGN_LEFT 0
#define ALLEGRO_ALIGN_RIGHT 1
#define ALLEGRO_ALIGN_CENTRE 2

#define ALLEGRO_EVENT_KEY_UP 1
#define ALLEGRO_EVENT_TIMER 2
#define ALLEGRO_EVENT_DISPLAY_CLOSE 3
#define ALLEGRO_EVENT_MOUSE_AXES 4
#define ALLEGRO_EVENT_MOUSE_BUTTON_DOWN 5
#define ALLEGRO_EVENT_KEY_DOWN 6
#define ALLEGRO_EVENT_KEY_CHAR 7
#define ALLEGRO_EVENT_MOUSE_BUTTON_UP 8

#define ALLEGRO_KEY_F1 RAWKEY_F1
#define ALLEGRO_KEY_F2 RAWKEY_F2
#define ALLEGRO_KEY_F3 RAWKEY_F3
#define ALLEGRO_KEY_F4 RAWKEY_F4
#define ALLEGRO_KEY_F5 RAWKEY_F5
#define ALLEGRO_KEY_F6 RAWKEY_F6
#define ALLEGRO_KEY_F7 RAWKEY_F7
#define ALLEGRO_KEY_F8 RAWKEY_F8
#define ALLEGRO_KEY_F9 RAWKEY_F9
#define ALLEGRO_KEY_F10 RAWKEY_F10

#define ALLEGRO_KEY_1 1
#define ALLEGRO_KEY_2 2
#define ALLEGRO_KEY_3 3
#define ALLEGRO_KEY_4 4
#define ALLEGRO_KEY_5 5
#define ALLEGRO_KEY_6 6
#define ALLEGRO_KEY_7 7
#define ALLEGRO_KEY_8 8
#define ALLEGRO_KEY_9 9
#define ALLEGRO_KEY_0 10

#define ALLEGRO_KEY_ESCAPE RAWKEY_ESC
#define ALLEGRO_KEY_UP RAWKEY_CRSRUP
#define ALLEGRO_KEY_DOWN RAWKEY_CRSRDOWN
#define ALLEGRO_KEY_RIGHT  RAWKEY_CRSRRIGHT
#define ALLEGRO_KEY_LEFT RAWKEY_CRSRLEFT
#define ALLEGRO_KEY_SPACE RAWKEY_SPACE

#define ALLEGRO_KEY_ENTER RAWKEY_RETURN
#define ALLEGRO_KEY_BACKSPACE RAWKEY_BACKSPACE
#define ALLEGRO_KEY_PAUSE 109

#define ALLEGRO_KEY_LSHIFT 96
#define ALLEGRO_KEY_RSHIFT 97
#define ALLEGRO_KEY_CTRL 99

#define ALLEGRO_KEY_A 32
#define ALLEGRO_KEY_B 53
#define ALLEGRO_KEY_C 51
#define ALLEGRO_KEY_D 34
#define ALLEGRO_KEY_E 18
#define ALLEGRO_KEY_F 35
#define ALLEGRO_KEY_G 36
#define ALLEGRO_KEY_H 37
#define ALLEGRO_KEY_I 23
#define ALLEGRO_KEY_J 38
#define ALLEGRO_KEY_K 39
#define ALLEGRO_KEY_L 40
#define ALLEGRO_KEY_M 55
#define ALLEGRO_KEY_N 54
#define ALLEGRO_KEY_O 24
#define ALLEGRO_KEY_P 25
#define ALLEGRO_KEY_Q 16
#define ALLEGRO_KEY_R 19
#define ALLEGRO_KEY_S 33
#define ALLEGRO_KEY_T 20
#define ALLEGRO_KEY_U 22
#define ALLEGRO_KEY_W 17
#define ALLEGRO_KEY_X 50
#define ALLEGRO_KEY_Y 52
#define ALLEGRO_KEY_Z 49

#define ALLEGRO_KEY_PAD_4 0
#define ALLEGRO_KEY_PAD_5 0
#define ALLEGRO_KEY_PAD_6 0
#define ALLEGRO_KEY_PAD_7 0
#define ALLEGRO_KEY_PAD_8 0

#define ALLEGRO_KEY_PGUP 73
#define ALLEGRO_KEY_END 113

#define ALLEGRO_KEY_PAD_MINUS 74
#define ALLEGRO_KEY_PAD_PLUS 94

extern BOOL amiga_init();
extern void amiga_uninit();

extern struct RastPort the_rp_dest;

#define ALLEGRO_BITMAP struct BitMap

#define al_destroy_bitmap(gfx) { if (gfx) IGraphics -> FreeBitMap(gfx); gfx = NULL; }

extern ALLEGRO_BITMAP *al_load_bitmap(const char *name);

extern void draw_comp_bitmap(struct BitMap *the_bitmap, struct BitMap *the_bitmap_dest, int width, int height, int wx, int wy, int ww, int wh);

extern void al_draw_bitmap_region( ALLEGRO_BITMAP *gfx, int fx, int fy, int w,int h,int  x ,int  y ,ULONG m) ;
extern void al_draw_bitmap( ALLEGRO_BITMAP *gfx, int x , int y, ULONG m);
extern void al_draw_scaled_rotated_bitmap(ALLEGRO_BITMAP *bm, int offx, int offy, int x, int y, double sx, double sy, double r, int flags);
extern void al_draw_tinted_scaled_rotated_bitmap_region(ALLEGRO_BITMAP *bm, double fx, double fy, double sw, double sh, ALLEGRO_COLOR tint, double cx, double  cy, double dx, double dy, double xscale, double yscale, double angle, int flags);

#define al_get_bitmap_width( gfx )  IGraphics ->GetBitMapAttr( gfx, BMA_ACTUALWIDTH )
#define al_get_bitmap_height( gfx ) IGraphics ->GetBitMapAttr( gfx, BMA_HEIGHT )

#define _strdup strdup

#define al_set_new_display_option


#define al_get_display_event_source( disp ) 0,disp
#define al_get_timer_event_source( timer ) 1,timer
#define al_get_keyboard_event_source( ) 2,NULL
#define al_get_mouse_event_source( ) 3,NULL

extern void al_register_event_source( ALLEGRO_EVENT_QUEUE *queue, ULONG type, void *ptr );

extern ALLEGRO_DISPLAY *al_create_display(int w, int h);
extern void al_destroy_display( ALLEGRO_DISPLAY *disp );

extern ALLEGRO_EVENT_QUEUE *al_create_event_queue();
extern void al_destroy_event_queue( ALLEGRO_EVENT_QUEUE *queue );

extern void al_wait_for_event( ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *event );

extern ALLEGRO_TIMER * al_create_timer( double speed_secs ) ;
extern void  al_destroy_timer( ALLEGRO_TIMER *timer );
extern void al_start_timer( ALLEGRO_TIMER *timer );

extern void al_flip_display( void );


#define al_is_event_queue_empty( queue ) IsListEmpty( &queue -> display -> window -> UserPort -> mp_MsgList ) 

#define al_map_rgb(  r,  g,  b ) 0xFF000000 | (( r <<16) | ( g << 8) |  b )
#define al_map_rgba( r, g, b, a ) ( ( a << 24) |  ( r <<16) | ( g << 8) | b )

void al_unmap_rgba( ALLEGRO_COLOR color, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a );

//define al_unmap_rgba ( color, r, g, b, a )  *(r) = (color >> 16) & 0xFF; *(g) = (color >>8) & 0xFF; *(b) = color & 0xFF; *(a) = (color >> 24) & 0xFF; 

extern void al_draw_textf( ALLEGRO_FONT *font, ALLEGRO_COLOR color, double x1, double y1, ULONG flag, char *fmt, ... );

extern ALLEGRO_FONT *al_load_font( char const *filename, int size, int flags );

extern uint32 al_get_font_line_height(ALLEGRO_FONT *font);

extern void al_destroy_font( ALLEGRO_FONT *font );

extern ALLEGRO_SAMPLE *al_load_sample( char *name);
extern void al_destroy_sample( ALLEGRO_SAMPLE *sample );
extern void al_reserve_samples(int samples);
extern void al_play_sample( ALLEGRO_SAMPLE *sound, double x , double y , double z,  int mode, void *ptr );
extern void al_install_audio();

extern ALLEGRO_SAMPLE_INSTANCE *al_create_sample_instance( ALLEGRO_SAMPLE *sample ); 
extern void al_destroy_sample_instance( ALLEGRO_SAMPLE_INSTANCE *instance );
extern BOOL al_play_sample_instance( ALLEGRO_SAMPLE_INSTANCE *instance );
extern ALLEGRO_MIXER *al_get_default_mixer();
extern BOOL al_attach_sample_instance_to_mixer( ALLEGRO_SAMPLE_INSTANCE *instance, ALLEGRO_MIXER *mixer);
extern BOOL al_set_sample_instance_playmode( ALLEGRO_SAMPLE_INSTANCE *instance, ALLEGRO_PLAYMODE val);

extern void al_get_mouse_state( ALLEGRO_MOUSE_STATE *dest );

// mid pri

uint32 al_get_text_width( ALLEGRO_FONT *font,char *txt );

extern ALLEGRO_BITMAP *al_create_bitmap( int w, int h );

#define al_set_target_bitmap( bm ) \
{ the_rp_dest.BitMap = bm; }

#define al_get_target_bitmap() the_rp_dest.BitMap

#define al_fread(fd,ptr,size) fread( ptr, size, 1, fd )
#define al_fwrite( fd, ptr, size)  fwrite( ptr, size, 1, fd ) 
#define al_fopen fopen
#define al_fclose fclose

// low pri

#define al_stop_sample_instance( ... )


#define al_hide_mouse_cursor( ... )
#define al_grab_mouse( ... )
#define al_set_new_bitmap_flags( flag )

// not needed.

#define al_init( ... ) TRUE
#define al_install_keyboard()
#define al_install_mouse()
#define al_init_primitives_addon()
#define al_init_font_addon()
#define al_init_ttf_addon()
#define al_init_image_addon()
#define al_init_acodec_addon()


#endif

#define ALLEGRO_PI M_PI
