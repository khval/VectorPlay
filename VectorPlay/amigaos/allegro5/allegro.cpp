
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <proto/Picasso96API.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <proto/keymap.h>
#include <proto/layers.h>

#include <string.h>
#include <png.h>
#include <stdarg.h>
#include <math.h>

#include <allegro5/allegro.h>


struct Library			*GraphicsBase = NULL;
struct GraphicsIFace	*IGraphics = NULL;

struct Library * IntuitionBase = NULL;
struct IntuitionIFace *IIntuition = NULL;

struct Library		*Picasso96Base = NULL;
struct P96IFace	*IP96 = NULL;

struct Library 		*Diskfontbase = NULL;
struct DiskfontIFace	*IDiskfont = NULL;
struct Library		*LocaleBase = NULL;
struct LocaleIFace	*ILocale = NULL;

struct Library		*Keymapbase = NULL;

struct Window *the_window_dest;
struct RastPort the_rp_dest;

struct Locale *locale;
uint32 codeset;
ULONG *CHAR_CODES;

struct XYSTW_Vertex3D { 
float x, y; 
float s, t, w; 
}; 

int dest_pen;

void draw_comp_bitmap(struct BitMap *the_bitmap,int fx,int fy, struct BitMap *the_bitmap_dest, int width,int height, int wx,int wy,int ww, int wh)
{
	#define STEP(a,xx,yy,ss,tt,ww)   P[a].x= xx; P[a].y= yy; P[a].s= ss; P[a].t= tt; P[a].w= ww;  

	int error;

	struct XYSTW_Vertex3D P[6];

	STEP(0, wx, wy ,fx ,fy ,1);
	STEP(1, wx+ww,wy,fx+width,fy,1);
	STEP(2, wx+ww,wy+wh,fx+width,fy+height,1);

	STEP(3, wx,wy, fx,fy,1);
	STEP(4, wx+ww,wy+wh,fx+width,fy+height,1);
	STEP(5, wx, wy+wh ,fx ,fy+height ,1);

	if (the_bitmap)
	{
		error = IGraphics -> CompositeTags(COMPOSITE_Src_Over_Dest, 
			the_bitmap, the_bitmap_dest,

			COMPTAG_VertexArray, P, 
			COMPTAG_VertexFormat,COMPVF_STW0_Present,
		    	COMPTAG_NumTriangles,2,

			COMPTAG_Flags,  COMPFLAG_HardwareOnly | COMPFLAG_SrcFilter ,
			TAG_DONE);
	}
}


void draw_comp_bitmap_rot(struct BitMap *the_bitmap,int fx,int fy,int width,int height, struct BitMap *the_bitmap_dest,  int wx,int wy,int ww, int wh,double r)
{
	#define STEP(a,xx,yy,ss,tt,ww)   P[a].x= xx; P[a].y= yy; P[a].s= ss; P[a].t= tt; P[a].w= ww;  

	int error;
	int v0_x,v0_y;
	int v1_x,v1_y;
	int cx, cy;
	struct XYSTW_Vertex3D P[6];
	double x[4],y[4];
	double a;
	int n;
	int tx,ty;

	x[0] = -0.5f;	y[0] = -0.5f;
	x[1] =  0.5f;	y[1] = -0.5f;
	x[2] =  0.5f;	y[2] =  0.5f;
	x[3] = -0.5f;	y[3] =  0.5f;

	for (n=0;n<4;n++)
	{
		v0_x  = cos( r ) *  ww * x[n] ;
		v0_y  =  -sin( r) * ww * x[n];

		v1_x  = cos( (double) r + (M_PI/2) )  *  -wh * y[n] ;
		v1_y  =  -sin( (double) r + (M_PI/2) ) * -wh * y[n];

		x[n] = v0_x + v1_x;
		y[n] = v0_y + v1_y;  

		x[n] +=  wx ;
		y[n] +=  wy ;
	}

	STEP(0, x[0],y[0] ,fx ,fy ,1);
	STEP(1, x[1],y[1], fx+width,fy,1);
	STEP(2, x[2],y[2],fx+width,fy+height,1);

	STEP(3, x[0],y[0], fx,fy,1);
	STEP(4, x[2],y[2], fx+width,fy+height,1);
	STEP(5, x[3],y[3] ,fx ,fy+height ,1);

	if (the_bitmap)
	{

		error = IGraphics -> CompositeTags(COMPOSITE_Src_Over_Dest, 
			the_bitmap, the_bitmap_dest,

			COMPTAG_VertexArray, P, 
			COMPTAG_VertexFormat,COMPVF_STW0_Present,
		    	COMPTAG_NumTriangles,2,

			COMPTAG_Flags,  COMPFLAG_HardwareOnly | COMPFLAG_SrcFilter ,
			TAG_DONE);
	}
}

void al_draw_bitmap( ALLEGRO_BITMAP *gfx, int x , int y, ULONG m)
{
	ULONG w = IGraphics ->GetBitMapAttr( gfx, BMA_ACTUALWIDTH )+1;
	ULONG h = IGraphics ->GetBitMapAttr( gfx, BMA_HEIGHT )+1;
	draw_comp_bitmap( gfx , 0, 0, the_rp_dest.BitMap ,  w, h, x, y, w, h);
}

void al_draw_bitmap_region( ALLEGRO_BITMAP *gfx, int fx, int fy, int w,int h,int  x ,int  y, ULONG m) 
{
	switch (m)
	{
		case 0:
			draw_comp_bitmap( gfx , fx, fy, the_rp_dest.BitMap,  w, h, x, y, w, h);
			break;
		case 1:
			draw_comp_bitmap( gfx , fx, fy, the_rp_dest.BitMap,  w, h, x+w, y, -w, h);
			break;
	}
}

void al_draw_scaled_rotated_bitmap( ALLEGRO_BITMAP *bm, int offx,int offy, int x, int y, double sx, double sy, double r, int flags )
{
	ULONG w = IGraphics ->GetBitMapAttr( bm, BMA_ACTUALWIDTH );
	ULONG h = IGraphics ->GetBitMapAttr( bm, BMA_HEIGHT );

	draw_comp_bitmap_rot( bm, 0,0, w,h, the_rp_dest.BitMap, x, y ,(int) ((double) w * sx),(int) ((double) h * sy), r);
}

void al_draw_tinted_scaled_rotated_bitmap_region( ALLEGRO_BITMAP *bm, double sx, double sy, double sw,  double sh, ALLEGRO_COLOR tint, double cx, double  cy, double dx,double dy, double xscale, double yscale, double angle, int flags )
{
	double v0_x,v0_y;
	double v1_x,v1_y;
	int tx,ty;

	v0_x  = cos( -angle ) *   cy * yscale;
	v0_y  =  -sin( -angle ) *  cy * yscale;

	v1_x  = cos( (double) -angle + (M_PI/2) )  *  cx * xscale;
	v1_y  =  -sin( (double) -angle + (M_PI/2) ) *  cx * xscale;

	tx =  -v0_x - v1_x + dx;
	ty =  -v0_y - v1_y + dy;

	draw_comp_bitmap_rot( bm, sx, sy,  sw ,  sh, the_rp_dest.BitMap,
		(int) tx ,(int) ty,
		(int) ((double) sw * xscale ) ,(int) ((double) sh * yscale) , -angle);
}


#define IDCMP_COMMON IDCMP_MOUSEBUTTONS | IDCMP_INACTIVEWINDOW | IDCMP_ACTIVEWINDOW  | \
	IDCMP_MOUSEMOVE | IDCMP_DELTAMOVE | IDCMP_REFRESHWINDOW | IDCMP_RAWKEY | \
	IDCMP_EXTENDEDMOUSE | IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | IDCMP_INTUITICKS


BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;
	*base = IExec->OpenLibrary( name , ver);
	if (*base)
	{
		 *interface = IExec->GetInterface( *base,  iname , iver, TAG_END );
	}

	return (*interface) ? TRUE : FALSE;
}

struct List alegro_event_list;

BOOL amiga_init()
{
	if ( ! open_lib( "intuition.library", 51L , "main", 1, &IntuitionBase, (struct Interface **) &IIntuition  ) ) return FALSE;
	if ( ! open_lib( "graphics.library", 51L , "main", 1, &GraphicsBase, (struct Interface **) &IGraphics  ) ) return FALSE;
	if ( ! open_lib( "Picasso96API.library", 51L , "main", 1, &Picasso96Base, (struct Interface **) &IP96 ) ) return FALSE;
	if ( ! open_lib( "diskfont.library", 53L , "main", 1, &Diskfontbase, (struct Interface **) &IDiskfont ) ) return FALSE;
	if ( ! open_lib( "locale.library", 53L , "main", 1, &LocaleBase, (struct Interface **) &ILocale ) ) return FALSE;
	if ( ! open_lib( "keymap.library", 53L , "main", 1, &KeymapBase, (struct Interface **) &IKeymap ) ) return FALSE;

	if ((ILocale)&&(IDiskfont))
	{
		locale = ILocale->OpenLocale(NULL);
		CHAR_CODES = (ULONG *) IDiskfont -> ObtainCharsetInfo(DFCS_NUMBER, (ULONG) locale -> loc_CodeSet , DFCS_MAPTABLE);
	}

	IExec->NewList(&alegro_event_list);

	return TRUE;
}


void  amiga_uninit()
{
	if (locale) ILocale->CloseLocale(locale);

	if (IntuitionBase) IExec->CloseLibrary(IntuitionBase); IntuitionBase = 0;
	if (IIntuition) IExec->DropInterface((struct Interface*) IIntuition); IIntuition = 0;

	if (GraphicsBase) IExec->CloseLibrary(GraphicsBase); GraphicsBase = 0;
	if (IGraphics) IExec->DropInterface((struct Interface*) IGraphics); IGraphics = 0;

	if (Picasso96Base) IExec->CloseLibrary(Picasso96Base); Picasso96Base = 0;
	if (IP96) IExec->DropInterface((struct Interface*)IP96); IP96 = 0;

	if (Diskfontbase) IExec->CloseLibrary(Diskfontbase); Diskfontbase = 0;
	if (IDiskfont) IExec->DropInterface((struct Interface*) IDiskfont); IDiskfont = 0;

	if (Keymapbase) IExec->CloseLibrary(Keymapbase); Keymapbase = 0;
	if (IKeymap) IExec->DropInterface((struct Interface*) IKeymap); IKeymap = 0;
}

void al_destroy_display( ALLEGRO_DISPLAY *tmp )
{
	if (tmp)
	{
		if (tmp->window)
		{
			IIntuition -> CloseWindow( tmp -> window);
			tmp->window = NULL;
		}

		if (tmp->bitmap)
		{
			IGraphics -> FreeBitMap(  tmp -> bitmap );
			tmp->bitmap = NULL;
		}

		IExec->FreeVec(tmp);
	}
}

ALLEGRO_DISPLAY *al_create_display(int w, int h)
{
		ALLEGRO_DISPLAY *tmp;

		IGraphics -> InitRastPort(&the_rp_dest);

		tmp = (ALLEGRO_DISPLAY *) IExec -> AllocVecTags( sizeof(ALLEGRO_DISPLAY), AVT_Type, MEMF_PRIVATE, TAG_END);

		if (tmp)
		{
			tmp -> window =  IIntuition -> OpenWindowTags( NULL,
					WA_Left,			0,
					WA_Top,			0,
					WA_InnerWidth,	w,
					WA_InnerHeight,	h,
					WA_SimpleRefresh,	TRUE,
					WA_CloseGadget,	FALSE,
					WA_DepthGadget,	FALSE,
					WA_DragBar,		TRUE,
					WA_Borderless,	FALSE,

					WA_MinWidth, w / 4,
					WA_MinHeight, h / 4,

					WA_MaxWidth, ~0,
					WA_MaxHeight, ~0,

					WA_SizeGadget,      TRUE,
					WA_SizeBBottom,	TRUE,

					WA_IDCMP,           IDCMP_COMMON,
					WA_Flags,           WFLG_REPORTMOUSE | WFLG_ACTIVATE | WFLG_RMBTRAP,
					//WA_SkinInfo,				NULL,
				TAG_DONE);

			if (tmp -> window)
			{

				tmp -> bitmap = IGraphics->AllocBitMapTags(w, h  , 32,
					BMATags_Displayable, TRUE,
					BMATags_PixelFormat, PIXF_A8R8G8B8,
					BMATags_Friend, tmp -> window ->RPort -> BitMap,
					TAG_DONE);

				the_window_dest = tmp -> window;
				the_rp_dest.BitMap = tmp -> bitmap;

				// clear graphics

				IGraphics->RectFillColor( tmp -> window ->RPort, tmp -> window -> BorderLeft, tmp -> window -> BorderTop, 
							tmp -> window -> BorderLeft + w, tmp -> window -> BorderTop + h, (uint32) 0xFF000000);

				IGraphics->RectFillColor( &the_rp_dest, 0,0 , w, h, (uint32) 0xFF000000);
			}
			else
			{
				IExec->FreeVec(tmp);
				return NULL;
			}
		}

		return tmp;
}

void al_get_mouse_state( ALLEGRO_MOUSE_STATE *dest )
{
	int x, y, w, h;
	int BitmapWidth ;
	int BitmapHeight ;

	x = the_window_dest -> MouseX -  the_window_dest -> BorderLeft;
	y = the_window_dest -> MouseY -  the_window_dest -> BorderTop;

	w = the_window_dest -> Width - the_window_dest -> BorderLeft - the_window_dest -> BorderRight;
	h = the_window_dest -> Height - the_window_dest -> BorderTop - the_window_dest -> BorderBottom;

	BitmapWidth = IGraphics ->GetBitMapAttr( the_rp_dest.BitMap, BMA_ACTUALWIDTH );
	BitmapHeight = IGraphics ->GetBitMapAttr( the_rp_dest.BitMap, BMA_HEIGHT );

	dest -> x = x * BitmapWidth/w;
	dest -> y = y* BitmapHeight/h;
}


ALLEGRO_EVENT_QUEUE *al_create_event_queue()
{
	ALLEGRO_EVENT_QUEUE *tmp;

	tmp = (ALLEGRO_EVENT_QUEUE *) IExec -> AllocVecTags( sizeof(ALLEGRO_EVENT_QUEUE), AVT_Type, MEMF_PRIVATE, TAG_END);

	tmp -> display = NULL;
	tmp -> keyboard = NULL;
	tmp -> mouse = NULL;
	tmp -> timer = NULL;

	return tmp;
}

void al_destroy_event_queue( ALLEGRO_EVENT_QUEUE *queue )
{
	if (queue)	IExec->FreeSysObject( ASOT_LIST, queue);
}

struct AL_EV_NODE
{
	struct Node node;
	ALLEGRO_EVENT event;
};


void al_wait_for_event( ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *event )
{
	ULONG sigs = 0;
	ULONG rsigs;
	ULONG timer_sig = 0;
	ULONG disp_sig = 0;
	struct IntuiMessage * msg;
	struct MsgPort *mp;
	ULONG code;
	BOOL exit_loop = false;
	struct AL_EV_NODE *node;

	if (node = (struct AL_EV_NODE *) IExec->RemHead( &alegro_event_list))
	{
		IExec -> CopyMem( &node -> event, event, sizeof(ALLEGRO_EVENT)  );
		IExec -> FreeSysObject(ASOT_NODE,node);
		return;
	}

	if (queue -> timer)
	{
		mp = queue -> timer -> mp;
		timer_sig =  1L << mp -> mp_SigBit ;
		sigs |= timer_sig;
	}

	if (queue -> display)
	{
		mp = queue -> display -> window -> UserPort;
		disp_sig =  1L << mp -> mp_SigBit ;
		sigs |= disp_sig;
	}

	if (!sigs) return;

	for(;;)
	{
		rsigs = IExec-> Wait( sigs );

		if (rsigs & timer_sig)
		{
			if (node = (struct AL_EV_NODE *) IExec->AllocSysObjectTags(ASOT_NODE, 
					ASONODE_Size, sizeof (struct AL_EV_NODE), TAG_END))
			{
				node -> event.type = ALLEGRO_EVENT_TIMER;
				IExec->AddTail( &alegro_event_list, (struct Node *) node );
			}
			IExec->SendIO( (IORequest *) queue -> timer -> tr );
		}

		if (rsigs & disp_sig)
		{
			while (msg = (struct IntuiMessage *) IExec->GetMsg( queue -> display -> window -> UserPort ))
			{
				switch (msg->Class )
				{
					case IDCMP_INTUITICKS:
						// ok so al_wait_for_event should not wait for ever, so we use this.
						exit_loop = true;
						break;

					case IDCMP_MOUSEBUTTONS:

						if (node = (struct AL_EV_NODE *) IExec->AllocSysObjectTags(ASOT_NODE, 
								ASONODE_Size, sizeof (struct AL_EV_NODE), TAG_END))
						{
							node -> event.type = msg -> Code & IECODE_UP_PREFIX ? 
								ALLEGRO_EVENT_MOUSE_BUTTON_UP : ALLEGRO_EVENT_MOUSE_BUTTON_DOWN;

							node -> event.mouse.button = (msg -> Code & ~IECODE_UP_PREFIX) - 103;
							node -> event.mouse.x = the_window_dest -> MouseX -  the_window_dest -> BorderLeft;
							node -> event.mouse.y = the_window_dest -> MouseY -  the_window_dest -> BorderTop;

							IExec->AddTail( &alegro_event_list, (struct Node *) node );
						}
						break;

					case IDCMP_DELTAMOVE:
					case IDCMP_MOUSEMOVE:
						
						if (node = (struct AL_EV_NODE *) IExec->AllocSysObjectTags(ASOT_NODE, 
								ASONODE_Size, sizeof (struct AL_EV_NODE), TAG_END))
						{
							node -> event.type = ALLEGRO_EVENT_MOUSE_AXES;
							node -> event.mouse.x = msg->MouseX;
							node -> event.mouse.y = msg->MouseY;
							IExec->AddTail( &alegro_event_list, (struct Node *) node );
						}

						break;

					case IDCMP_RAWKEY:

//						printf("%d\n", msg -> Code & ~IECODE_UP_PREFIX );

						if (msg -> Code & IECODE_UP_PREFIX)
						{
							if (node = (struct AL_EV_NODE *) IExec->AllocSysObjectTags(ASOT_NODE, 
									ASONODE_Size, sizeof (struct AL_EV_NODE), TAG_END))
							{
								node->event.type = ALLEGRO_EVENT_KEY_UP;
								node->event.keyboard.keycode = msg -> Code & ~IECODE_UP_PREFIX;
								node->event.keyboard.unichar = 0;
								IExec->AddTail( &alegro_event_list, (struct Node *) node );
								node->event.type = ALLEGRO_EVENT_KEY_UP;
							}
						}
						else
						{
							if (node = (struct AL_EV_NODE *) IExec->AllocSysObjectTags(ASOT_NODE, 
									ASONODE_Size, sizeof (struct AL_EV_NODE), TAG_END))
							{
								node->event.type = ALLEGRO_EVENT_KEY_DOWN;
								node->event.keyboard.keycode = msg -> Code & ~IECODE_UP_PREFIX;
								node->event.keyboard.unichar = 0;
								IExec->AddTail( &alegro_event_list, (struct Node *) node );
								node->event.type = ALLEGRO_EVENT_KEY_DOWN;
							}

							{
								WORD actual;
								struct InputEvent ie;
								char buffer[100];

								ie.ie_Class = IECLASS_RAWKEY;
								ie.ie_SubClass = 0;
        	        					ie.ie_Code         = msg->Code;
								ie.ie_Qualifier    = msg->Qualifier;
								ie.ie_EventAddress=(APTR *) *((ULONG *)msg->IAddress);

								actual=IKeymap->MapRawKey(&ie, buffer, sizeof(buffer), 0);

								if (actual>0)
								{
									int i;
    							    		for (i = 0; i < actual; i++)
									{
										if (node = (struct AL_EV_NODE *) IExec->AllocSysObjectTags(ASOT_NODE, 
											ASONODE_Size, sizeof (struct AL_EV_NODE), TAG_END))
										{
											node->event.type = ALLEGRO_EVENT_KEY_CHAR;
											node->event.keyboard.keycode = msg -> Code & ~IECODE_UP_PREFIX;
											node->event.keyboard.unichar = buffer[0];
											IExec->AddTail( &alegro_event_list, (struct Node *) node );
										}
									}
								}
							}
						}

						break;
				} // end switch

				IExec -> ReplyMsg( (Message *) msg );

			}  // end while
		} // end if

		if (!IsListEmpty( &alegro_event_list) ) break;
		if (exit_loop) break;
	}

	if (!IsListEmpty( &alegro_event_list) )
	{
		if (node = (struct AL_EV_NODE *) IExec->RemHead( &alegro_event_list))
		{
			IExec -> CopyMem( &node -> event, event, sizeof(ALLEGRO_EVENT)  );
			IExec -> FreeSysObject(ASOT_NODE,node);
			return;
		}
		else
		{
			bzero( event, sizeof(ALLEGRO_EVENT) );
		}
	}
}

void al_register_event_source( ALLEGRO_EVENT_QUEUE *queue, ULONG type, void *ptr )
{
	switch (type)
	{
		case 0: queue -> display = (ALLEGRO_DISPLAY *) ptr; break;
		case 1: queue -> timer = (ALLEGRO_TIMER *) ptr; break;
		case 2: queue -> keyboard = ptr; break;
		case 3: queue -> mouse = ptr; break;
	}
}


typedef struct CompositeHookData_s {
	struct BitMap *srcBitMap; // The source bitmap
	int32 srcWidth, srcHeight; // The source dimensions
	int32 offsetX, offsetY; // The offsets to the destination area relative to the window's origin
	int32 scaleX, scaleY; // The scale factors
	uint32 retCode; // The return code from CompositeTags()
} CompositeHookData;

static ULONG compositeHookFunc(struct Hook *hook, struct RastPort *rastPort, struct BackFillMessage *msg) 
{
	CompositeHookData *hookData = (CompositeHookData*)hook->h_Data;

	hookData->retCode = IGraphics -> CompositeTags(
		COMPOSITE_Src_Over_Dest, hookData->srcBitMap, rastPort->BitMap,
		COMPTAG_SrcAlpha, 0x10000,
		COMPTAG_SrcWidth,   hookData->srcWidth,
		COMPTAG_SrcHeight,  hookData->srcHeight,
		COMPTAG_ScaleX, 	hookData->scaleX,
		COMPTAG_ScaleY, 	hookData->scaleY,
		COMPTAG_OffsetX,    msg->Bounds.MinX - (msg->OffsetX - hookData->offsetX),
		COMPTAG_OffsetY,    msg->Bounds.MinY - (msg->OffsetY - hookData->offsetY),
		COMPTAG_DestX,      msg->Bounds.MinX,
		COMPTAG_DestY,      msg->Bounds.MinY,
		COMPTAG_DestWidth,  msg->Bounds.MaxX - msg->Bounds.MinX + 1,
		COMPTAG_DestHeight, msg->Bounds.MaxY - msg->Bounds.MinY + 1,
		COMPTAG_Flags,      COMPFLAG_SrcFilter | COMPFLAG_SrcAlphaOverride | COMPFLAG_IgnoreDestAlpha | COMPFLAG_HardwareOnly,
		TAG_END);

	return 0;
}


void al_flip_display( void )
{
	int BitmapWidth ;
	int BitmapHeight ;
	CompositeHookData hookData;
	struct Rectangle rect;
	struct Hook hook;

	ILayers -> LockLayer(0, the_window_dest->RPort->Layer);

	BitmapWidth = IGraphics ->GetBitMapAttr( the_rp_dest.BitMap, BMA_ACTUALWIDTH );
	BitmapHeight = IGraphics ->GetBitMapAttr( the_rp_dest.BitMap, BMA_HEIGHT );

 	rect.MinX = the_window_dest->BorderLeft;
 	rect.MinY = the_window_dest->BorderTop;
 	rect.MaxX = the_window_dest->Width - the_window_dest->BorderRight - 1;
 	rect.MaxY = the_window_dest->Height - the_window_dest->BorderBottom - 1;
 	double destWidth = rect.MaxX - rect.MinX + 1;
 	double destHeight = rect.MaxY - rect.MinY + 1;
 	double scaleX = (destWidth + 0.5f) / BitmapWidth;
 	double scaleY = (destHeight + 0.5f) / BitmapHeight;

	hookData.srcBitMap = the_rp_dest.BitMap;
	hookData.srcWidth = BitmapWidth;
	hookData.srcHeight = BitmapHeight;
	hookData.offsetX = the_window_dest->BorderLeft;
	hookData.offsetY = the_window_dest->BorderTop;
	hookData.scaleX = COMP_FLOAT_TO_FIX(scaleX);
	hookData.scaleY = COMP_FLOAT_TO_FIX(scaleY);
	hookData.retCode = COMPERR_Success;

	hook.h_Entry = (HOOKFUNC)compositeHookFunc;
	hook.h_Data = &hookData;

	ILayers -> DoHookClipRects(&hook, the_window_dest -> RPort, &rect);
	ILayers -> UnlockLayer(the_window_dest->RPort->Layer);
	IGraphics->WaitBOVP(&(the_window_dest->WScreen->ViewPort));
}


void al_clear_to_color( ALLEGRO_COLOR color )
{
	ULONG w = IGraphics ->GetBitMapAttr( the_rp_dest.BitMap, BMA_ACTUALWIDTH );
	ULONG h = IGraphics ->GetBitMapAttr( the_rp_dest.BitMap, BMA_HEIGHT );
	IGraphics->RectFillColor(&the_rp_dest, 0, 0, w, h, (uint32) color);
}

ALLEGRO_BITMAP *al_create_bitmap( int w, int h )
{
	struct BitMap *bm = NULL;
	struct RastPort rp;

	if (bm = IGraphics->AllocBitMapTags( w, h, 32, 
		BMATags_Friend, the_window_dest -> RPort -> BitMap, 
		BMATags_Displayable, TRUE, BMATags_PixelFormat, PIXF_A8R8G8B8, TAG_DONE))
	{

		IGraphics -> InitRastPort(&rp);
		rp.BitMap =bm;
		IGraphics->RectFillColor( &rp, 0, 0, w, h, (uint32) 0xFF000000);
	}
	else
	{
		printf("Bitmap not created %dx%d\n",w,h);
	}

	return (ALLEGRO_BITMAP *) bm;
}

static void (*copy_row) (unsigned char *from_row, unsigned char *to_row, unsigned int row_bytes);

static void copy_RGBA_to_ARGB(unsigned char *from_row, unsigned char *to_row, unsigned int row_bytes)
{
	int x;
	// copy & convert from RGBA to ARGB
	for (x = 0 ; x < row_bytes; x++)
	{
		to_row[(x&~3)+((x&3)+1&3)]= from_row[x];
	}
}

static void copy_RGB_to_ARGB(unsigned char *from_row, unsigned char *to_row, unsigned int row_bytes)
{
	int x;
	int tb;
	int fb;
	// copy & convert from RGBA to ARGB

	for (x = 0 ; x < row_bytes / 3; x++)
	{
		tb = x * 4;
		fb = x * 3;

		to_row[ tb + 0]= 0xFF;
		to_row[ tb + 1]= from_row[ fb + 0];
		to_row[ tb + 2]= from_row[ fb + 1];
		to_row[ tb + 3]= from_row[ fb + 2];
	}
}

ALLEGRO_BITMAP *al_load_bitmap(const char *name)
{
	
png_structp png_ptr;
	png_info *info_ptr;
	png_bytepp row_pointers;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	int x,y;
	int dx;
	struct BitMap *bm = NULL;
	unsigned char *to_row;
	struct RenderInfo	rinf;
	int plock;

	int outWidth;
	int outHeight;
	BOOL outHasAlpha;
	struct RastPort rp;
	int BytesPerPixel;

	FILE *fp;

	if ((fp = fopen(name, "rb")) == NULL)	return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	info_ptr=png_create_info_struct(png_ptr);

	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return NULL;
	}
 
	outWidth = png_get_image_width(png_ptr,info_ptr);
	outHeight =  png_get_image_height(png_ptr,info_ptr);

	color_type = png_get_color_type(png_ptr,info_ptr);

	switch ( color_type )
	{
		case PNG_COLOR_TYPE_RGBA:
			outHasAlpha = TRUE;
			copy_row = copy_RGBA_to_ARGB;
			break;

		case PNG_COLOR_TYPE_RGB:
			outHasAlpha = FALSE;
			copy_row = copy_RGB_to_ARGB;
			break;

	        default:
			printf("color type %d, not supported\n", color_type);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
			return NULL;
	}

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);

	row_pointers = png_get_rows(png_ptr, info_ptr);

	bm = IGraphics->AllocBitMapTags(outWidth, outHeight  , 32,
					BMATags_Displayable, TRUE,
					BMATags_PixelFormat, PIXF_A8R8G8B8,
					BMATags_Friend, the_window_dest -> RPort -> BitMap,
					TAG_DONE);

	if (bm)
	{
		IGraphics -> InitRastPort(&rp);
		rp.BitMap = bm;

		rinf.BytesPerRow = outWidth * 4;
		if (rinf.Memory = IExec -> AllocVecTags( rinf.BytesPerRow * outHeight, AVT_Type, MEMF_PRIVATE, TAG_END))
		{
			memset( rinf.Memory, 0, rinf.BytesPerRow * outHeight );

			for ( y = 0; y < outHeight; y++)
			{
				to_row	= ( unsigned char *) ((char *) rinf.Memory + ( y * rinf.BytesPerRow ) );
				copy_row(  row_pointers[y], to_row, row_bytes);
			}

			IGraphics -> WritePixelArray( (uint8 *) rinf.Memory,0, 0, rinf.BytesPerRow, PIXF_A8R8G8B8, &rp, 0, 0, outWidth, outHeight);
			IExec -> FreeVec( rinf.Memory );
		}
	}


    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(fp);

    return bm;
}

BOOL timer_device_open = FALSE;

ALLEGRO_TIMER * al_create_timer( double speed_secs ) 
{
	ALLEGRO_TIMER *timer = (ALLEGRO_TIMER *) IExec -> AllocVecTags( sizeof(ALLEGRO_TIMER), AVT_Type, MEMF_PRIVATE, TAG_END);

	if (timer)
	{
		timer -> open = FALSE;

		if (timer -> mp = (struct MsgPort *) IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE))
		{
			timer -> tr = (struct TimeRequest *) IExec->AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_ReplyPort, timer -> mp, ASOIOR_Size, sizeof(struct TimeRequest), TAG_DONE);
		}

		if (timer -> tr)
		{
			timer -> tr->Time.Seconds = (ULONG) speed_secs;
			timer -> tr ->Time.Microseconds = ( (ULONG) (speed_secs * 1000000) % 1000000);

			if (IExec->OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest *) timer -> tr,0L)==0)
			{
				timer -> open=TRUE;
			}
		}

		if (timer -> open == FALSE)
		{
			al_destroy_timer( timer );
			timer = NULL;
		}
	}

	return timer;
}

void  al_destroy_timer( ALLEGRO_TIMER *timer )
{
	if (timer -> open)
	{
		IExec->CloseDevice((struct IORequest *) timer -> tr );
		timer -> open = FALSE;
	}

	if ( timer -> tr )
	{
		IExec->FreeSysObject(ASOT_IOREQUEST, (struct IORequest *) timer -> tr);
		timer -> tr = NULL;
	}

	if ( timer -> mp ) 
	{
		IExec->FreeSysObject(ASOT_PORT, timer -> mp);
		timer -> mp = NULL;
	}
}

extern void al_start_timer( ALLEGRO_TIMER *timer )
{
	timer -> tr ->Request.io_Command =  UNIT_WAITUNTIL;
	timer -> tr ->Request.io_Flags = IOF_QUICK;
	IExec->SendIO((struct IORequest *) timer ->tr);
}

void al_draw_filled_rounded_rectangle( double x1, double y1, double x2, double y2, double rx, double ry, ALLEGRO_COLOR color )
{
	IGraphics -> RectFillColor( &the_rp_dest , (ULONG) x1, (ULONG) y1, (ULONG) x2, (ULONG) y2, color );
 }

void al_draw_filled_rectangle( double x1, double y1, double x2, double y2, ALLEGRO_COLOR color )
{
	IGraphics -> RectFillColor( &the_rp_dest , (ULONG) x1, (ULONG) y1, (ULONG) x2-1, (ULONG) y2-1, color );
 }

void al_draw_pixel( double x, double y,ALLEGRO_COLOR color )
{
	IGraphics -> WritePixelColor( &the_rp_dest , (ULONG) x, (ULONG) y, (ULONG) color );
}

 void al_draw_line( double x1, double y1, double x2, double y2, ALLEGRO_COLOR color,double thickness )
{
	IGraphics -> SetRPAttrs( &the_rp_dest,  RPTAG_APenColor, (ULONG) color, TAG_END );
	IGraphics -> Move( &the_rp_dest,  (ULONG) x1, (ULONG) y1 );
	IGraphics -> Draw( &the_rp_dest,  (ULONG) x2, (ULONG) y2 );
}

void al_draw_rectangle( double x1, double y1, double x2, double y2, ALLEGRO_COLOR color, double thickness)
{
	thickness--;
	if (thickness<0) thickness = 0;

	x1 --;
	y1--;

	x2 --;
	y2 --;

	IGraphics -> RectFillColor( &the_rp_dest , (ULONG) x1, (ULONG) y1, (ULONG) x2, (ULONG) (y1 + thickness), color );
	IGraphics -> RectFillColor( &the_rp_dest , (ULONG) x1, (ULONG) y1, (ULONG) (x1+thickness), (ULONG) y2, color );
	IGraphics -> RectFillColor( &the_rp_dest , (ULONG) (x2 - thickness), (ULONG) y1, (ULONG) x2, (ULONG) y2, color );
	IGraphics -> RectFillColor( &the_rp_dest , (ULONG) x1, (ULONG) (y2 - thickness), (ULONG) x2, (ULONG) y2, color );
}

void al_draw_textf( ALLEGRO_FONT *font, ALLEGRO_COLOR color, double x1, double y1, ULONG flag, char *fmt, ... )
{
	struct TextExtent ext;
	char *txt;
	char buffer[10000];
	int offx;

        va_list args;
        va_start (args, fmt);
        vsprintf( buffer, fmt , args);
        va_end (args);

	txt = buffer;

	if (font) IGraphics -> SetFont(&the_rp_dest, font);

	IGraphics -> SetRPAttrs(&the_rp_dest, RPTAG_APenColor, 0xFF000000 | color, RPTAG_DrMd, JAM1, TAG_END );
	IGraphics -> TextExtent(&the_rp_dest, txt, strlen(txt), &ext);

	switch(flag)
	{
		case ALLEGRO_ALIGN_RIGHT:
			offx = -ext.te_Width;
			break;

		case ALLEGRO_ALIGN_CENTRE:
			offx = -ext.te_Width/2;
			break;

		default:
			offx=0;
			break;
	}

	IGraphics -> Move( &the_rp_dest ,(ULONG) x1 + offx, (ULONG) y1 - ext.te_Extent.MinY);
	IGraphics -> Text( &the_rp_dest , txt, strlen(txt) );
 }

uint32 al_get_text_width( ALLEGRO_FONT *font,char *txt ) 
{
	struct TextExtent ext;
	if (font) IGraphics -> SetFont(&the_rp_dest, font);
	IGraphics ->TextExtent(&the_rp_dest, txt, strlen(txt), &ext);
	return ext.te_Width;
}

uint32 al_get_font_line_height(ALLEGRO_FONT *font)
{
	struct TextExtent ext;
	char *txt = (char *) "yXz!|-+";

	if (font) IGraphics -> SetFont(&the_rp_dest, font);

	IGraphics ->TextExtent(&the_rp_dest, txt, strlen(txt), &ext);
	return ext.te_Height;
}

ALLEGRO_FONT *al_load_font( char const *filename, int size, int flags )
{
	struct TextAttr ta;
	 ALLEGRO_FONT *font;

	if (!filename) return NULL;

	ta.ta_Name = filename;  
	ta.ta_YSize = size; 
	ta.ta_Style = 0; 
	ta.ta_Flags =  FPF_DISKFONT; 

	font = IDiskfont->OpenDiskFont(&ta);
	if (!font) printf("%s: not loaded\n", filename );

	return font;
}

void al_destroy_font( ALLEGRO_FONT *font )
{
	if (font) IGraphics->CloseFont(font);
}

void al_unmap_rgba( ALLEGRO_COLOR color, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a )
{
	*r = (color >> 16) & 0xFF; 
	*g = (color >>8) & 0xFF; 
	*b = color & 0xFF; 
	*a = (color >> 24) & 0xFF; 
}