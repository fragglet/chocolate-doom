//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

// DoomDef.h

#ifndef __DOOMDEF__
#define __DOOMDEF__
#include <stdio.h>
#include <string.h>
//haleyjd: removed WATCOMC
#include <limits.h>

#define HERETIC_VERSION 130
#define HERETIC_VERSION_TEXT "v1.3"

// if rangecheck is undefined, most parameter validation debugging code
// will not be compiled
//#define RANGECHECK

// all external data is defined here
#include "doomdata.h"

// all important printed strings
#include "dstrings.h"

// header generated by multigen utility
#include "info.h"

// WAD file access
#include "w_wad.h"

// fixed_t
#include "m_fixed.h"

// angle_t 
#include "tables.h"

// events
#include "d_event.h"

// gamemode/mission
#include "d_mode.h"

// ticcmd_t
#include "d_ticcmd.h"

#include "d_loop.h"

#define	SAVEGAMENAME "hticsav"

/*
===============================================================================

						GLOBAL TYPES

===============================================================================
*/

#define NUMARTIFCTS	28
#define MAXPLAYERS	4

#define	BT_ATTACK		1
#define	BT_USE			2
#define	BT_CHANGE		4       // if true, the next 3 bits hold weapon num
#define	BT_WEAPONMASK	(8+16+32)
#define	BT_WEAPONSHIFT	3

#define BT_SPECIAL		128     // game events, not really buttons
#define	BTS_SAVEMASK	(4+8+16)
#define	BTS_SAVESHIFT	2
#define	BT_SPECIALMASK	3
#define	BTS_PAUSE		1       // pause the game
#define	BTS_SAVEGAME	2       // save the game at each console
// savegame slot numbers occupy the second byte of buttons

typedef enum
{
    GS_LEVEL,
    GS_INTERMISSION,
    GS_FINALE,
    GS_DEMOSCREEN
} gamestate_t;

typedef enum
{
    ga_nothing,
    ga_loadlevel,
    ga_newgame,
    ga_loadgame,
    ga_savegame,
    ga_playdemo,
    ga_completed,
    ga_victory,
    ga_worlddone,
    ga_screenshot
} gameaction_t;

typedef enum
{
    wipe_0,
    wipe_1,
    wipe_2,
    wipe_3,
    wipe_4,
    NUMWIPES,
    wipe_random
} wipe_t;

/*
===============================================================================

							MAPOBJ DATA

===============================================================================
*/

// think_t is a function pointer to a routine to handle an actor
typedef void (*think_t) ();

typedef struct thinker_s
{
    struct thinker_s *prev, *next;
    think_t function;
} thinker_t;

typedef union
{
    int i;
    struct mobj_s *m;
} specialval_t;

struct player_s;

typedef struct mobj_s
{
    thinker_t thinker;          // thinker links

// info for drawing
    fixed_t x, y, z;
    struct mobj_s *snext, *sprev;       // links in sector (if needed)
    angle_t angle;
    spritenum_t sprite;         // used to find patch_t and flip value
    int frame;                  // might be ord with FF_FULLBRIGHT

// interaction info
    struct mobj_s *bnext, *bprev;       // links in blocks (if needed)
    struct subsector_s *subsector;
    fixed_t floorz, ceilingz;   // closest together of contacted secs
    fixed_t radius, height;     // for movement checking
    fixed_t momx, momy, momz;   // momentums

    int validcount;             // if == validcount, already checked

    mobjtype_t type;
    mobjinfo_t *info;           // &mobjinfo[mobj->type]
    int tics;                   // state tic counter
    state_t *state;
    int damage;                 // For missiles
    int flags;
    int flags2;                 // Heretic flags
    specialval_t special1;      // Special info
    specialval_t special2;      // Special info
    int health;
    int movedir;                // 0-7
    int movecount;              // when 0, select a new dir
    struct mobj_s *target;      // thing being chased/attacked (or NULL)
    // also the originator for missiles
    int reactiontime;           // if non 0, don't attack yet
    // used by player to freeze a bit after
    // teleporting
    int threshold;              // if >0, the target will be chased
    // no matter what (even if shot)
    struct player_s *player;    // only valid if type == MT_PLAYER
    int lastlook;               // player number last looked for

    mapthing_t spawnpoint;      // for nightmare respawn
} mobj_t;

// each sector has a degenmobj_t in it's center for sound origin purposes
typedef struct
{
    thinker_t thinker;          // not used for anything
    fixed_t x, y, z;
} degenmobj_t;

//
// frame flags
//
#define	FF_FULLBRIGHT	0x8000  // flag in thing->frame
#define FF_FRAMEMASK	0x7fff

// --- mobj.flags ---

#define	MF_SPECIAL	1         // call P_SpecialThing when touched
#define	MF_SOLID	2
#define	MF_SHOOTABLE	4
#define	MF_NOSECTOR	8         // don't use the sector links
                                  // (invisible but touchable)
#define	MF_NOBLOCKMAP	16        // don't use the blocklinks
                                  // (inert but displayable)
#define	MF_AMBUSH	32
#define	MF_JUSTHIT	64        // try to attack right back
#define	MF_JUSTATTACKED	128       // take at least one step before attacking
#define	MF_SPAWNCEILING	256       // hang from ceiling instead of floor
#define	MF_NOGRAVITY	512       // don't apply gravity every tic

// movement flags
#define	MF_DROPOFF	0x400     // allow jumps from high places
#define	MF_PICKUP	0x800     // for players to pick up items
#define	MF_NOCLIP	0x1000    // player cheat
#define	MF_SLIDE	0x2000    // keep info about sliding along walls
#define	MF_FLOAT	0x4000    // allow moves to any height, no gravity
#define	MF_TELEPORT	0x8000    // don't cross lines or look at heights
#define MF_MISSILE	0x10000   // don't hit same species, explode on block

#define	MF_DROPPED	0x20000   // dropped by a demon, not level spawned
#define	MF_SHADOW	0x40000   // use translucent draw (shadow demons / invis)
#define	MF_NOBLOOD	0x80000   // don't bleed when shot (use puff)
#define	MF_CORPSE	0x100000  // don't stop moving halfway off a step
#define	MF_INFLOAT	0x200000  // floating to a height for a move, don't
                                  // auto float to target's height

#define	MF_COUNTKILL	0x400000  // count towards intermission kill total
#define	MF_COUNTITEM	0x800000  // count towards intermission item total

#define	MF_SKULLFLY	0x1000000 // skull in flight
#define	MF_NOTDMATCH	0x2000000 // don't spawn in death match (key cards)

#define	MF_TRANSLATION	0xc000000 // if 0x4 0x8 or 0xc, use a translation
#define	MF_TRANSSHIFT	26      // table for player colormaps

// --- mobj.flags2 ---

#define MF2_LOGRAV		0x00000001  // alternate gravity setting
#define MF2_WINDTHRUST		0x00000002  // gets pushed around by the wind
                                            // specials
#define MF2_FLOORBOUNCE		0x00000004  // bounces off the floor
#define MF2_THRUGHOST		0x00000008  // missile will pass through ghosts
#define MF2_FLY			0x00000010  // fly mode is active
#define MF2_FOOTCLIP		0x00000020  // if feet are allowed to be clipped
#define MF2_SPAWNFLOAT		0x00000040  // spawn random float z
#define MF2_NOTELEPORT		0x00000080  // does not teleport
#define MF2_RIP			0x00000100  // missile rips through solid
                                            // targets
#define MF2_PUSHABLE		0x00000200  // can be pushed by other moving
                                            // mobjs
#define MF2_SLIDE		0x00000400  // slides against walls
#define MF2_ONMOBJ		0x00000800  // mobj is resting on top of another
                                            // mobj
#define MF2_PASSMOBJ		0x00001000  // Enable z block checking.  If on,
                                            // this flag will allow the mobj to
                                            // pass over/under other mobjs.
#define MF2_CANNOTPUSH		0x00002000  // cannot push other pushable mobjs
#define MF2_FEETARECLIPPED	0x00004000  // a mobj's feet are now being cut
#define MF2_BOSS		0x00008000  // mobj is a major boss
#define MF2_FIREDAMAGE		0x00010000  // does fire damage
#define MF2_NODMGTHRUST		0x00020000  // does not thrust target when
                                            // damaging
#define MF2_TELESTOMP		0x00040000  // mobj can stomp another
#define MF2_FLOATBOB		0x00080000  // use float bobbing z movement
#define MF2_DONTDRAW		0X00100000  // don't generate a vissprite

//=============================================================================
typedef enum
{
    PST_LIVE,                   // playing
    PST_DEAD,                   // dead on the ground
    PST_REBORN                  // ready to restart
} playerstate_t;

// psprites are scaled shapes directly on the view screen
// coordinates are given for a 320*200 view screen
typedef enum
{
    ps_weapon,
    ps_flash,
    NUMPSPRITES
} psprnum_t;

typedef struct
{
    state_t *state;             // a NULL state means not active
    int tics;
    fixed_t sx, sy;
} pspdef_t;

typedef enum
{
    key_yellow,
    key_green,
    key_blue,
    NUMKEYS
} keytype_t;

typedef enum
{
    wp_staff,
    wp_goldwand,
    wp_crossbow,
    wp_blaster,
    wp_skullrod,
    wp_phoenixrod,
    wp_mace,
    wp_gauntlets,
    wp_beak,
    NUMWEAPONS,
    wp_nochange
} weapontype_t;

#define AMMO_GWND_WIMPY 10
#define AMMO_GWND_HEFTY 50
#define AMMO_CBOW_WIMPY 5
#define AMMO_CBOW_HEFTY 20
#define AMMO_BLSR_WIMPY 10
#define AMMO_BLSR_HEFTY 25
#define AMMO_SKRD_WIMPY 20
#define AMMO_SKRD_HEFTY 100
#define AMMO_PHRD_WIMPY 1
#define AMMO_PHRD_HEFTY 10
#define AMMO_MACE_WIMPY 20
#define AMMO_MACE_HEFTY 100

typedef enum
{
    am_goldwand,
    am_crossbow,
    am_blaster,
    am_skullrod,
    am_phoenixrod,
    am_mace,
    NUMAMMO,
    am_noammo                   // staff, gauntlets
} ammotype_t;

typedef struct
{
    ammotype_t ammo;
    int upstate;
    int downstate;
    int readystate;
    int atkstate;
    int holdatkstate;
    int flashstate;
} weaponinfo_t;

extern weaponinfo_t wpnlev1info[NUMWEAPONS];
extern weaponinfo_t wpnlev2info[NUMWEAPONS];

typedef enum
{
    arti_none,
    arti_invulnerability,
    arti_invisibility,
    arti_health,
    arti_superhealth,
    arti_tomeofpower,
    arti_torch,
    arti_firebomb,
    arti_egg,
    arti_fly,
    arti_teleport,
    NUMARTIFACTS
} artitype_t;

typedef enum
{
    pw_None,
    pw_invulnerability,
    pw_invisibility,
    pw_allmap,
    pw_infrared,
    pw_weaponlevel2,
    pw_flight,
    pw_shield,
    pw_health2,
    NUMPOWERS
} powertype_t;

#define	INVULNTICS (30*35)
#define	INVISTICS (60*35)
#define	INFRATICS (120*35)
#define	IRONTICS (60*35)
#define WPNLEV2TICS (40*35)
#define FLIGHTTICS (60*35)

#define CHICKENTICS (40*35)

#define MESSAGETICS (4*35)
#define BLINKTHRESHOLD (4*32)

#define NUMINVENTORYSLOTS	14
typedef struct
{
    int type;
    int count;
} inventory_t;

/*
================
=
= player_t
=
================
*/

typedef struct player_s
{
    mobj_t *mo;
    playerstate_t playerstate;
    ticcmd_t cmd;

    fixed_t viewz;              // focal origin above r.z
    fixed_t viewheight;         // base height above floor for viewz
    fixed_t deltaviewheight;    // squat speed
    fixed_t bob;                // bounded/scaled total momentum

    int flyheight;
    int lookdir;
    boolean centering;
    int health;                 // only used between levels, mo->health
    // is used during levels
    int armorpoints, armortype; // armor type is 0-2

    inventory_t inventory[NUMINVENTORYSLOTS];
    artitype_t readyArtifact;
    int artifactCount;
    int inventorySlotNum;
    int powers[NUMPOWERS];
    boolean keys[NUMKEYS];
    boolean backpack;
    signed int frags[MAXPLAYERS];       // kills of other players
    weapontype_t readyweapon;
    weapontype_t pendingweapon; // wp_nochange if not changing
    boolean weaponowned[NUMWEAPONS];
    int ammo[NUMAMMO];
    int maxammo[NUMAMMO];
    int attackdown, usedown;    // true if button down last tic
    int cheats;                 // bit flags

    int refire;                 // refired shots are less accurate

    int killcount, itemcount, secretcount;      // for intermission
    char *message;              // hint messages
    int messageTics;            // counter for showing messages
    int damagecount, bonuscount;        // for screen flashing
    int flamecount;             // for flame thrower duration
    mobj_t *attacker;           // who did damage (NULL for floors)
    int extralight;             // so gun flashes light up areas
    int fixedcolormap;          // can be set to REDCOLORMAP, etc
    int colormap;               // 0-3 for which color to draw player
    pspdef_t psprites[NUMPSPRITES];     // view sprites (gun, etc)
    boolean didsecret;          // true if secret level has been done
    int chickenTics;            // player is a chicken if > 0
    int chickenPeck;            // chicken peck countdown
    mobj_t *rain1;              // active rain maker 1
    mobj_t *rain2;              // active rain maker 2
} player_t;

#define CF_NOCLIP		1
#define	CF_GODMODE		2
#define	CF_NOMOMENTUM	4       // not really a cheat, just a debug aid

#define	SBARHEIGHT	42      // status bar height at bottom of screen


/*
===============================================================================

					GLOBAL VARIABLES

===============================================================================
*/

#define TELEFOGHEIGHT (32*FRACUNIT)

extern gameaction_t gameaction;

extern boolean paused;

extern GameMode_t gamemode;

extern boolean ExtendedWAD;     // true if main WAD is the extended version

extern boolean nomonsters;      // checkparm of -nomonsters

extern boolean respawnparm;     // checkparm of -respawn

extern boolean debugmode;       // checkparm of -debug

extern boolean usergame;        // ok to save / end game

extern boolean ravpic;          // checkparm of -ravpic

extern boolean altpal;          // checkparm to use an alternate palette routine

extern boolean cdrom;           // true if cd-rom mode active ("-cdrom")

extern boolean deathmatch;      // only if started as net death

extern boolean netgame;         // only true if >1 player

extern boolean playeringame[MAXPLAYERS];

extern int consoleplayer;       // player taking events and displaying

extern int displayplayer;

extern int viewangleoffset;     // ANG90 = left side, ANG270 = right

extern player_t players[MAXPLAYERS];

extern boolean DebugSound;      // debug flag for displaying sound info

extern int GetWeaponAmmo[NUMWEAPONS];

extern boolean demorecording;
extern boolean demoplayback;
extern int skytexture;

extern gamestate_t gamestate;
extern skill_t gameskill;
extern boolean respawnmonsters;
extern int gameepisode;
extern int gamemap;
extern int prevmap;
extern int totalkills, totalitems, totalsecret; // for intermission
extern int levelstarttic;       // gametic at level start
extern int leveltime;           // tics in game play for par

extern ticcmd_t *netcmds;

#define SAVEGAMESIZE 0x30000
#define SAVESTRINGSIZE 24

extern mapthing_t *deathmatch_p;
extern mapthing_t deathmatchstarts[10];
extern mapthing_t playerstarts[MAXPLAYERS];

extern int mouseSensitivity;

extern boolean precache;        // if true, load all graphics at level load

extern boolean singledemo;      // quit after playing a demo from cmdline

extern int bodyqueslot;
extern skill_t startskill;
extern int startepisode;
extern int startmap;
extern boolean autostart;

extern boolean testcontrols;
extern int testcontrols_mousespeed;

/*
===============================================================================

					GLOBAL FUNCTIONS

===============================================================================
*/

#include "z_zone.h"

//----------
//BASE LEVEL
//----------
void D_DoomMain(void);
void IncThermo(void);
void InitThermo(int max);
void tprintf(char *string, int initflag);
// not a globally visible function, just included for source reference
// calls all startup code
// parses command line options
// if not overrided, calls N_AdvanceDemo

void D_DoomLoop(void);
// not a globally visible function, just included for source reference
// called by D_DoomMain, never exits
// manages timing and IO
// calls all ?_Responder, ?_Ticker, and ?_Drawer functions
// calls I_GetTime, I_StartFrame, and I_StartTic

//---------
//SYSTEM IO
//---------
byte *I_AllocLow(int length);
// allocates from low memory under dos, just mallocs under unix

// haleyjd: was WATCOMC, preserved for historical interest.
// This is similar to the -control structure in DOOM v1.4 and Strife.
#if 0
extern boolean useexterndriver;

#define EBT_FIRE			1
#define EBT_OPENDOOR 		2
#define EBT_SPEED			4
#define EBT_STRAFE			8
#define EBT_MAP				0x10
#define EBT_INVENTORYLEFT 	0x20
#define EBT_INVENTORYRIGHT 	0x40
#define EBT_USEARTIFACT		0x80
#define EBT_FLYDROP			0x100
#define EBT_CENTERVIEW		0x200
#define EBT_PAUSE			0x400
#define EBT_WEAPONCYCLE 	0x800

typedef struct
{
    short vector;               // Interrupt vector

    signed char moveForward;    // forward/backward (maxes at 50)
    signed char moveSideways;   // strafe (maxes at 24)
    short angleTurn;            // turning speed (640 [slow] 1280 [fast])
    short angleHead;            // head angle (+2080 [left] : 0 [center] : -2048 [right])
    signed char pitch;          // look up/down (-110 : +90)
    signed char flyDirection;   // flyheight (+1/-1)
    unsigned short buttons;     // EBT_* flags
} externdata_t;
#endif

//----
//GAME
//----

void G_DeathMatchSpawnPlayer(int playernum);

void G_InitNew(skill_t skill, int episode, int map);

void G_DeferedInitNew(skill_t skill, int episode, int map);
// can be called by the startup code or M_Responder
// a normal game starts at map 1, but a warp test can start elsewhere

void G_DeferedPlayDemo(char *demo);

void G_LoadGame(char *name);
// can be called by the startup code or M_Responder
// calls P_SetupLevel or W_EnterWorld
void G_DoLoadGame(void);

void G_SaveGame(int slot, char *description);
// called by M_Responder

#define SAVE_GAME_TERMINATOR 0x1d
// Support routines for saving games
char *SV_Filename(int slot);
void SV_Open(char *fileName);
void SV_OpenRead(char *fileName);
void SV_Close(char *fileName);
void SV_Write(void *buffer, int size);
void SV_WriteByte(byte val);
void SV_WriteWord(unsigned short val);
void SV_WriteLong(unsigned int val);
void SV_Read(void *buffer, int size);
byte SV_ReadByte(void);
uint16_t SV_ReadWord(void);
uint32_t SV_ReadLong(void);

extern char *savegamedir;

void G_RecordDemo(skill_t skill, int numplayers, int episode, int map,
                  char *name);
// only called by startup code

void G_PlayDemo(char *name);
void G_TimeDemo(char *name);

void G_ExitLevel(void);
void G_SecretExitLevel(void);

void G_WorldDone(void);

void G_Ticker(void);
boolean G_Responder(event_t * ev);

void G_ScreenShot(void);

//-----
//PLAY
//-----

void P_Ticker(void);
// called by C_Ticker
// can call G_PlayerExited
// carries out all thinking of monsters and players

void P_SetupLevel(int episode, int map, int playermask, skill_t skill);
// called by W_Ticker

void P_Init(void);
// called by startup code

void P_ArchivePlayers(void);
void P_UnArchivePlayers(void);
void P_ArchiveWorld(void);
void P_UnArchiveWorld(void);
void P_ArchiveThinkers(void);
void P_UnArchiveThinkers(void);
void P_ArchiveSpecials(void);
void P_UnArchiveSpecials(void);
// load / save game routines


//-------
//REFRESH
//-------

extern boolean setsizeneeded;

extern boolean BorderNeedRefresh;
extern boolean BorderTopRefresh;

extern int UpdateState;
// define the different areas for the dirty map
#define I_NOUPDATE	0
#define I_FULLVIEW	1
#define I_STATBAR	2
#define I_MESSAGES	4
#define I_FULLSCRN	8

void R_RenderPlayerView(player_t * player);
// called by G_Drawer

void R_Init(void);
// called by startup code

void R_DrawViewBorder(void);
void R_DrawTopBorder(void);
// if the view size is not full screen, draws a border around it

void R_SetViewSize(int blocks, int detail);
// called by M_Responder

int R_FlatNumForName(char *name);

int R_TextureNumForName(char *name);
int R_CheckTextureNumForName(char *name);
// called by P_Ticker for switches and animations
// returns the texture number for the texture name


//----
//MISC
//----
// returns the position of the given parameter in the arg list (0 if not found)

int M_DrawText(int x, int y, boolean direct, char *string);

//----------------------
// Interlude (IN_lude.c)
//----------------------

extern boolean intermission;

void IN_Start(void);
void IN_Ticker(void);
void IN_Drawer(void);

//----------------------
// Chat mode (CT_chat.c)
//----------------------

void CT_Init(void);
void CT_Drawer(void);
boolean CT_Responder(event_t * ev);
void CT_Ticker(void);
char CT_dequeueChatChar(void);

extern boolean chatmodeon;
extern boolean ultimatemsg;

//--------------------
// Finale (F_finale.c)
//--------------------

void F_Drawer(void);
void F_Ticker(void);
void F_StartFinale(void);

//----------------------
// STATUS BAR (SB_bar.c)
//----------------------

void SB_Init(void);
boolean SB_Responder(event_t * event);
void SB_Ticker(void);
void SB_Drawer(void);

//-----------------
// MENU (MN_menu.c)
//-----------------

extern boolean MenuActive;

void MN_Init(void);
void MN_ActivateMenu(void);
void MN_DeactivateMenu(void);
boolean MN_Responder(event_t * event);
void MN_Ticker(void);
void MN_Drawer(void);
void MN_DrTextA(char *text, int x, int y);
int MN_TextAWidth(char *text);
void MN_DrTextB(char *text, int x, int y);
int MN_TextBWidth(char *text);

#include "sounds.h"

#endif // __DOOMDEF__
