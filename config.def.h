/* See LICENSE file for copyright and license details. */

// All patches:
// movestack, uselessgap (improved version), focusonclick,
// urgentborder, selfrestart (customized), focusurgent (custom)

// TODO:
// mod4+n/mod4+shift+n - minimize/restore windows
// mod4+n - minimize window
// mod4+m - toggle maximize - equivalent of XK_m+XK_t
// mod4+f - toggle fullscreen
// mod4+wheelup - compton-trans -c +10
// mod4+wheeldown - compton-trans -c -10
// autorun, feh, set bg - http://dwm.suckless.org/patches/autostart
// have new windows become slaves instead of masters:
//   http://dwm.suckless.org/patches/attachaside
// keyboard moving/resizing - http://dwm.suckless.org/patches/moveresize
//   another: http://dwm.suckless.org/patches/maximize
//   better: http://dwm.suckless.org/patches/exresize
// attach aside - http://dwm.suckless.org/patches/attachaside
// attach above current client - http://dwm.suckless.org/patches/attachabove
// full taskbar - http://dwm.suckless.org/patches/fancybar
//   see also: http://dwm.suckless.org/patches/fancycoloredbarclickable
// Built-in launcher:
// https://bbs.archlinux.org/viewtopic.php?pid=874763#p874763
// Systray
// https://bbs.archlinux.org/viewtopic.php?pid=874768#p874768
// http://dwm.suckless.org/patches/systray
// Status Colors
// http://dwm.suckless.org/patches/statuscolors
// Statusbar written in C
// http://dwm.suckless.org/dwmstatus/

// http://dwm.suckless.org/patches/movestack
// http://dwm.suckless.org/patches/movestack-5.8.2.diff
// alternative: http://dwm.suckless.org/patches/push
#include "movestack.c"

// http://dwm.suckless.org/patches/selfrestart
// http://dwm.suckless.org/patches/dwm-r1615-selfrestart.diff
#define DWM_PATH "/usr/bin/dwm"
#include "selfrestart.c"

// Focus urgent function
#include "focusurgent.c"

// Center window function
#include "center.c"

// http://dwm.suckless.org/patches/uselessgap
// http://dwm.suckless.org/patches/dwm-5.9-uselessgap.diff
// For other useless gaps, see:
// https://www.google.com/search?q=dwm+vertical+useless+gap
// https://bbs.archlinux.org/viewtopic.php?pid=1055914
// https://bbs.archlinux.org/viewtopic.php?id=92895&p=26
// https://bbs.archlinux.org/viewtopic.php?id=92895&p=1
// https://github.com/simongmzlj/dwm/blob/master/gaps.diff
// https://github.com/vodik/dwm/commit/74cebfa997815ac0dce34934a5626414d3c8553a
// http://ompldr.org/vY2s2Mg/gaps.diff
// https://bbs.archlinux.org/viewtopic.php?pid=1050873#p1050873
// #ifndef USELESS_GAP
// #define USELESS_GAP 1
// #endif

// http://dwm.suckless.org/patches/focusonclick
// http://dwm.suckless.org/patches/dwm-6.0-focusonclick.diff
// #ifndef FOCUS_ON_CLICK
// #define FOCUS_ON_CLICK 1
// #endif

// http://dwm.suckless.org/patches/urgentborder
// http://dwm.suckless.org/patches/dwm-5.7.2-urgentborder.diff
// #ifndef URGENT_BORDER
// #define URGENT_BORDER 1
// #endif

/* appearance */
static const char font[]            = "-*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*";

static const char normbordercolor[] = "#000000";
static const char normbgcolor[]     = "#222222";
static const char normfgcolor[]     = "#aaaaaa";
static const char selbordercolor[]  = "#535d6c";
static const char selbgcolor[]      = "#222222";
static const char selfgcolor[]      = "#ffffff";
#if URGENT_BORDER
static const char urgbordercolor[]  = "#ff0000";
#endif

static const unsigned int borderpx  = 1;        /* border pixel of windows */
#if USELESS_GAP
// NOTE: Default was originally 6.
static const unsigned int gappx     = 10;        /* gap pixel between windows */
#endif
static const unsigned int snap      = 32;       /* snap pixel */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */

#if FOCUS_ON_CLICK
/* False means using the scroll wheel on a window will not change focus */
static const Bool focusonwheelscroll = False;
#endif

/* tagging */
static const char *tags[] = { "root", "www", "media", "opt", "etc" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       "QEMU",     0,            True,        -1 },
	{ "MPlayer",  NULL,       NULL,       0,            True,        -1 },
	{ "Skype",    NULL,       NULL,       0,            False,       -1 },
	{ "XClock",   NULL,       NULL,       0,            True,        -1 },
	{ "feh",      NULL,       NULL,       0,            True,        -1 },
};

/* layout(s) */
static const float mfact      = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = False; /* True means respect size hints in tiled resizals */

#if USELESS_GAP
static const Layout layouts[] = {
	/* symbol     gap       arrange function */
	{ "[]=",      True,     tile },    /* first entry is default */
	{ "><>",      False,    NULL },    /* no layout function means floating behavior */
	{ "[M]",      False,    monocle },
};
#else
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};
#endif

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *dmenucmd[] = { "dmenu_run", "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "xterm", NULL };

// Custom:
static const char *voldowncmd[]   = { "amixer", "set", "Master", "5%-", NULL };
static const char *volupcmd[]     = { "amixer", "set", "Master", "5%+", NULL };
static const char *voltogglecmd[] = { "amixer", "set", "Master", "toggle", NULL };
static const char *captogglecmd[] = { "amixer", "set", "Capture", "toggle", NULL };
static const char *pastecmd[]     = { "xdotool", "click", "2", NULL };
#ifdef USE_IMAGEMAGICK
static const char *scrotcmd[]     = { "sh", "-c", "import -window root ~/screenshot.png", NULL };
#else
static const char *scrotcmd[]     = { "scrot", "-e", "mv $f ~/screenshots", NULL };
#endif
static const char *amenucmd[]     = { "amenu", "d", "-b", "-p", ">", "-fn", font,
                                      "-nb", normbgcolor, "-nf", normfgcolor,
                                      "-sb", selbgcolor, "-sf", selfgcolor,
                                      NULL };
static const char *clipcmd[]  = { "sh", "-c",
                                  " echo '' | xclip -i -selection primary;"
                                  " echo '' | xclip -i -selection secondary;"
                                  " echo '' | xclip -i -selection clipboard;"
                                  " killall xclip || killall -9 xclip",
                                  NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	//{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	//{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	//{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },

	// Custom:
	// NOTE: See /usr/include/X11/keysymdef.h
	{ MODKEY,                       XK_comma,  spawn,          {.v = voldowncmd } },
	{ MODKEY,                       XK_period, spawn,          {.v = volupcmd } },
	{ MODKEY,                       XK_v,      spawn,          {.v = voltogglecmd } },
	{ MODKEY,                       XK_b,      spawn,          {.v = captogglecmd } },
	{ 0,                            XK_Menu,   spawn,          {.v = pastecmd } },
	{ 0,                            XK_Print,  spawn,          {.v = scrotcmd } },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_r,      spawn,          {.v = amenucmd } },
	{ MODKEY,                       XK_w,      spawn,          {.v = amenucmd } },
	{ MODKEY|ControlMask,           XK_r,      self_restart,   {0} },
	{ MODKEY,                       XK_u,      focusurgent,    {0} },
	{ MODKEY,                       XK_c,      center,         {0} },
	{ MODKEY,                       XK_g,      spawn,          {.v = clipcmd } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
