//
// CNDOOM timers
//

#include "doomtype.h"
#include "doomdef.h"
#include "doomstat.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"
#include "i_system.h"
#include "hu_stuff.h" // for HU_FONTSIZE, HU_FONTSTART
#include "i_swap.h"
#include "i_video.h"
#include "r_main.h"
#include "g_game.h"
#include "wi_stuff.h"
#include "m_misc.h"

static int timer_x, timer_y;			// final calculated position
static int timer_width, timer_height;		// and size

#define TFONTWIDTH 5
#define TFONTHEIGHT 6
#define TFONTCOLON 10
#define TFONTDOT 11
#define TFONTCHARS 12
#define INITIAL_CHARS 8
#define BORDER_H 5
#define BORDER_V 3

static byte timerfont[TFONTWIDTH*TFONTHEIGHT*TFONTCHARS] =
{
	0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,
	0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x00,0x01,0x00,0x01,0x00,
	0x00,0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x00,0x01,
	0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,
	0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x02,0x02,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x02,
	0x02,0x01,0x02,0x01,0x02,0x02,0x01,0x02,0x01,0x02,0x01,0x02,
	0x00,0x01,0x02,0x02,0x02,0x02,0x01,0x02,0x02,0x02,0x02,0x00,
	0x02,0x02,0x01,0x02,0x01,0x02,0x02,0x01,0x02,0x01,0x02,0x02,
	0x01,0x02,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x02,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x01,0x01,
	0x01,0x01,0x02,0x00,0x02,0x01,0x01,0x02,0x01,0x02,0x01,0x02,
	0x00,0x01,0x01,0x01,0x01,0x02,0x01,0x01,0x01,0x01,0x02,0x00,
	0x00,0x01,0x02,0x02,0x01,0x01,0x01,0x01,0x02,0x01,0x01,0x01,
	0x01,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x02,0x00,0x01,0x02,0x00,0x00,0x01,0x02,0x02,0x01,0x02,
	0x02,0x02,0x02,0x01,0x00,0x00,0x01,0x02,0x01,0x01,0x01,0x01,
	0x02,0x00,0x02,0x02,0x01,0x02,0x01,0x02,0x02,0x01,0x02,0x00,
	0x00,0x01,0x02,0x00,0x01,0x02,0x02,0x01,0x02,0x00,0x02,0x02,
	0x01,0x02,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x01,0x01,0x01,0x02,0x01,0x01,0x01,0x01,0x02,0x01,0x01,
	0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x02,0x00,0x02,0x01,0x02,
	0x02,0x01,0x01,0x01,0x01,0x02,0x01,0x01,0x01,0x01,0x02,0x00,
	0x00,0x01,0x02,0x00,0x01,0x01,0x01,0x01,0x02,0x01,0x01,0x01,
	0x01,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x02,0x02,0x02,0x02,0x00,0x02,0x02,0x02,0x02,0x00,0x02,
	0x02,0x02,0x02,0x00,0x02,0x02,0x02,0x02,0x00,0x00,0x02,0x02,
	0x00,0x00,0x02,0x02,0x02,0x02,0x00,0x02,0x02,0x02,0x02,0x00,
	0x00,0x00,0x02,0x00,0x00,0x02,0x02,0x02,0x02,0x00,0x02,0x02,
	0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
};

extern int cn_timer_offset_x;
extern int cn_timer_offset_y;
extern int cn_timer_color_index;
extern int cn_timer_shadow_index;
extern int cn_timer_bg_colormap;
extern int cn_timer_enabled;

// from wi_stuff.c for use in new intermission timers
extern patch_t *winum[10], *wicolon;
// from hu_stuff.c for use in total time display
extern patch_t *hu_font[HU_FONTSIZE];

static void CN_DrawTimerCharacter (int x, int y, char c)
{
    int i, j;
    byte *screenp, *charap;

    if (c == ':')
	c = TFONTCOLON;
    else if (c == '.')
	c = TFONTDOT;
    else if (c >= '0' && c <= '9')
	c -= '0';
    else
	I_Error ("CN_DrawTimerCharacter: Bad character #%i.\n", c);

    screenp = I_VideoBuffer + y*SCREENWIDTH + x;
    charap = timerfont + c*TFONTWIDTH;

    for (i=0; i<TFONTHEIGHT; i++)
    {
	for (j=0; j<TFONTWIDTH; j++)
	{
	    if (*charap & 1)
		*screenp++ = cn_timer_color_index;
	    else if (*charap & 2)
		*screenp++ = cn_timer_shadow_index;
	    else
		screenp++;

	    *charap++;
	}
	screenp += SCREENWIDTH - TFONTWIDTH;
	charap += TFONTWIDTH*(TFONTCHARS-1);
    }

    return;
}

// dims a rectangular area of the screen using COLORMAP
static void CN_DimBox (int x, int y, int w, int h, int cm)
{
    int i, j, l;
    byte *cmap, *screenp;

    l = w * h;
    cmap = colormaps + cm*256;
    screenp = I_VideoBuffer + y*SCREENWIDTH + x;

    for (i=0; i<h; i++)
    {
	for (j=0; j<w; j++)
	{
	    *screenp = cmap[*screenp++];
	}
	screenp += SCREENWIDTH - w;
    }
}

void CN_DrawIntermissionTime (int x, int y, int time)
{
    int i, l;
    static char buffer[16];
    int w = 0;

    l = M_snprintf (buffer, 16, "%02i:%05.2f", leveltime / TICRATE / 60,
                  (float)(leveltime % (60*TICRATE)) / TICRATE);

    for (i=0; i<l; i++)
    {
	if (buffer[i] == ':' || buffer[i] == '.')
	    w += SHORT(wicolon->width);
	else
	    w += SHORT(winum[0]->width);
    }

    w += 4;
    x -= w;

    // CN_DimBox (x, y-1, w, SHORT(winum[0]->height) + 2, 20); broken after high resolution

    x += w-2;

    for (i=l-1; i>=0; i--)
    {
	if (buffer[i] == ':' || buffer[i] == '.')
	{
	    x -= SHORT(wicolon->width);
            if ((gametic % TICRATE*2) < TICRATE)
		V_DrawPatch (x, y, wicolon);
	}
	else
	{
	    x -= SHORT(winum[0]->width);
	    V_DrawPatch (x, y, winum[buffer[i]-'0']);
	}
    }
}

void CN_DrawTotalTime (void)
{
    int i;
    static char buffer[16];

    i = M_snprintf (buffer, 16, "%02i:%02i:%05.2f", totaltime / TICRATE / 60 / 60, (totaltime / TICRATE / 60) % 60,
                  (float)(totaltime % (60*TICRATE)) / TICRATE);

    printf ("%s\r", buffer);
    return;
}

// needs to be called on startup and every time screen size changes
void CN_UpdateTimerLocation(int anchor)
{
    int x, y, w, h;

    // view borders
    if (anchor)
    {
	x = viewwindowx;
	y = viewwindowy;
	w = scaledviewwidth;
	h = viewheight;
    }
    // screen borders
    else
    {
	x = 0;
	y = 0;
	w = SCREENWIDTH;
	h = SCREENHEIGHT;
    }

    timer_x = x + cn_timer_offset_x;
    timer_y = y + cn_timer_offset_y;

    // negative offsets count from bottom and right edges
    if (cn_timer_offset_x < 0)
	timer_x += w;
    if (cn_timer_offset_y < 0)
	timer_y += h;

    // make sure it doesn't go outside the screen
    if (timer_x < 0)
	timer_x = 0;
    else if (timer_x > x + w - timer_width)
	timer_x = x + w - timer_width;
    if (timer_y < 0)
	timer_y = 0;
    else if (timer_y > y + h - timer_height)
	timer_y = y + h - timer_height;
}

extern boolean gamekeydown[256];

void CN_DrawTimer (void)
{
    static char buffer[16];
    int i, l, mins, x, y;
    static int lastsize = 0;

// if (gamekeydown['z']) leveltime = 6666*TICRATE; testing how it works

    mins = leveltime / TICRATE / 60;
    l = M_snprintf (buffer, 16, "%02i:%05.2f", mins,
                  (float)(leveltime % (60*TICRATE)) / TICRATE);

    if (l > INITIAL_CHARS && lastsize != l)
    {
	timer_width = TFONTWIDTH*l + BORDER_H;
	CN_UpdateTimerLocation(1);

	lastsize = l;
    }

    if (cn_timer_bg_colormap)
    {
	CN_DimBox (timer_x, timer_y, timer_width, timer_height,
	  cn_timer_bg_colormap);
    }

    x = timer_x+3;
    y = timer_y+2;

    for (i=0; i<strlen(buffer); i++)
    {
	CN_DrawTimerCharacter(x, y, buffer[i]);
	x += TFONTWIDTH;
    }
}

// call on startup and map change
void CN_ResetTimer (void)
{
    timer_width = TFONTWIDTH*INITIAL_CHARS + BORDER_H;
    timer_height = TFONTHEIGHT + BORDER_V;
    CN_UpdateTimerLocation(1);
}
