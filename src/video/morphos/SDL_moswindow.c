/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#include "SDL_hints.h"
#include "SDL_syswm.h"
#include "SDL_timer.h"
#include "SDL_version.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../core/morphos/SDL_misc.h"
#include "../../events/SDL_keyboard_c.h"
#include "../../events/SDL_mouse_c.h"
#include "../../core/morphos/SDL_library.h"

#include "SDL_mosvideo.h"
#include "SDL_mosmodes.h"
#include "SDL_mosmouse.h"
#include "SDL_mosopengl.h"
#include "SDL_mosevents.h"

#include <sys/param.h>

#include <intuition/extensions.h>
#include <libraries/charsets.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/wb.h>

struct NewMenu SDL_NewMenu[] =
{
	{ NM_TITLE, (char *)"Project", 0, 0, 0, (APTR)MID_PROJECT },
	{ NM_ITEM , (char *)"About...", (const STRPTR)"A", 0, 0, (APTR)MID_ABOUT },
	{ NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL },
	{ NM_ITEM , (char *)"About Joystick", (const STRPTR)"J", 0, 0, (APTR)MID_JOYSTICK},
	{ NM_ITEM , (char *)"About System", (const STRPTR)"S", 0, 0, (APTR)MID_ABOUTSYS},
	{ NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL },
	{ NM_ITEM , (char *)"Hide", (const STRPTR)"H", 0, 0, (APTR)MID_HIDE },
	{ NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL },
	{ NM_ITEM , (char *)"Quit", (const STRPTR)"Q", 0, 0, (APTR)MID_QUIT},
	{ NM_TITLE, (char *)"Options", 0, 0, 0, (APTR)MID_OPTIONS },
	{ NM_ITEM , (char *)"Mute Sound", (const STRPTR)"M", (CHECKIT | MENUTOGGLE), 0, (APTR)MID_MUTE},
	{ NM_ITEM , (char *)"Low CPU Priority", (const STRPTR)"P", (CHECKIT | MENUTOGGLE), 0, (APTR)MID_PRIORITY},
	{ NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL },
	{ NM_ITEM, (char *)"HINT RENDER DRIVER", 0, 0, 0, (APTR)MID_RRENDER },
	{ NM_SUB, (char *)"Auto", 0, (CHECKED | CHECKIT | MENUTOGGLE), 0, (APTR)MID_RRAUTO },
	{ NM_SUB, (char *)"opengl", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_RRGL },
	{ NM_SUB, (char *)"software", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_RRSOFT },
	{ NM_ITEM, (char *)"HINT RENDER SCALER", 0, 0, 0, (APTR)MID_RSCALER},
	{ NM_SUB, (char *)"Auto", 0, (CHECKED | CHECKIT | MENUTOGGLE), 0, (APTR)MID_AUTO },
	{ NM_SUB, (char *)"Nearest", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_NEAREST },
	{ NM_SUB, (char *)"Linear", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_LINEAR },
	{ NM_ITEM, (char *)"HINT RENDER LOGICAL SIZE", 0, 0, 0, (APTR)MID_LLOGICAL},
	{ NM_SUB, (char *)"Auto", 0, (CHECKED | CHECKIT | MENUTOGGLE), 0, (APTR)MID_LAUTO },
	{ NM_SUB, (char *)"Letterbox / sidebars", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_LLETTER },
	{ NM_SUB, (char *)"Overscan", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_LOVERS },
	{ NM_ITEM, (char *)"HINT RENDER BATCHING", 0, 0, 0, (APTR)MID_RBATCHING },
	{ NM_SUB, (char *)"Auto", 0, (CHECKED | CHECKIT | MENUTOGGLE), 0, (APTR)MID_BAUTO },
	{ NM_SUB, (char *)"Enable", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_BENABLE },
	{ NM_SUB, (char *)"Disable", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_BDISABLE },
	{ NM_ITEM, (char *)"HINT RENDER LINE METHOD", 0, 0, 0, (APTR)MID_RMETHOD },
	{ NM_SUB, (char *)"Default", 0, (CHECKED | CHECKIT | MENUTOGGLE), 0, (APTR)MID_MDEF },
	{ NM_SUB, (char *)"Point API", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_MPOINT },
	{ NM_SUB, (char *)"Line API", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_MLINE },
	{ NM_SUB, (char *)"Geometry API", 0, (CHECKIT | MENUTOGGLE), 0, (APTR)MID_MGEO },
	{ NM_END , NULL, NULL, 0, 0, NULL }
};

static void 
MOS_CloseWindowSafely(SDL_Window *sdlwin, struct Window *win)
{
	D("[%s]\n", __FUNCTION__);

	if (SDL_GetKeyboardFocus() == sdlwin)
		SDL_SetKeyboardFocus(NULL);

	if (SDL_GetMouseFocus() == sdlwin)
		SDL_SetMouseFocus(NULL);

	if ((sdlwin->flags & SDL_WINDOW_FOREIGN) == 0) {
		struct IntuiMessage *msg, *tmp;

		Forbid();

		ForeachNodeSafe(&win->UserPort->mp_MsgList, msg, tmp) {
			if (msg->IDCMPWindow == win) {
				REMOVE(&msg->ExecMessage.mn_Node);
				ReplyMsg(&msg->ExecMessage);
			}
		}

		SDL_WindowData *data = (SDL_WindowData *) sdlwin->driverdata;
		if (data->appmsg) {
			if (RemoveAppWindow(data->appmsg)) {
				data->appmsg = NULL;
			}
		}
		if (data->menu) {
			ClearMenuStrip(win);
			FreeMenus(data->menu);
			data->menu = NULL;
		}
		if (data->visualinfo) {
			FreeVisualInfo(data->visualinfo);
					data->visualinfo = NULL;
		}
		
		CloseWindow(win);
		
		Permit();
	}
}

void
MOS_CloseWindows(_THIS)
{
	SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
	SDL_WindowData *wd;
	D("[%s]\n", __FUNCTION__);

	ForeachNode(&data->windowlist, wd)
	{
		struct Window *win = wd->win;

		if (win)
		{
			wd->win = NULL;
			MOS_CloseWindowSafely(wd->window, win);
		}
	}
}

void MOS_OpenWindows(_THIS)
{
	SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
	SDL_WindowData *wd;
	D("[%s]\n", __FUNCTION__);

	ForeachNode(&data->windowlist, wd) {
		if (wd->win == NULL && !(wd->window->flags & SDL_WINDOW_FOREIGN) && (wd->winflags & SDL_MOS_WINDOW_SHOWN))	{
			MOS_ShowWindow_Internal(_this, wd->window);
		}
	}
}

static int
MOS_SetupWindowData(_THIS, SDL_Window *window, struct Window *win)
{
	D("[%s]\n", __FUNCTION__);
	
	SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
	SDL_WindowData *wd = SDL_malloc(sizeof(*wd));
	
	if (wd) {
		window->driverdata = wd;
		ADDHEAD(&data->windowlist, wd);
		wd->region = NULL;
		wd->fb = NULL;
		wd->window = window;
		wd->win = win;
		wd->__tglContext = NULL;
		wd->grabbed = -1;
		wd->sdlflags = 0;
		wd->window_title = NULL;
		wd->videodata = data;
		wd->first_deltamove = 0;
		wd->winflags = 0;
		wd->appmsg = NULL;
		if (win)
			win->UserData = (APTR)wd;
	} else {
		return SDL_OutOfMemory();
	}

	return 0;
}

int
MOS_CreateWindow(_THIS, SDL_Window * window)
{
	return MOS_SetupWindowData(_this, window, NULL);
}

int
MOS_CreateWindowFrom(_THIS, SDL_Window * window, const void *data)
{
	struct Window *win = (struct Window *)data;
	size_t flags;

	if (win->Title && win->Title != (APTR)-1)
		window->title = MOS_ConvertText(win->Title, MIBENUM_SYSTEM, MIBENUM_UTF_8);

	flags = (window->flags | SDL_WINDOW_SHOWN | SDL_WINDOW_FOREIGN) & ~SDL_WINDOW_MINIMIZED;

	if (win->Flags & WFLG_SIZEGADGET)
		flags |= SDL_WINDOW_RESIZABLE;
	else
		flags &= ~SDL_WINDOW_RESIZABLE;

	if (win->Flags & WFLG_BORDERLESS)
		flags |= SDL_WINDOW_BORDERLESS;
	else
		flags &= ~SDL_WINDOW_BORDERLESS;

	if (win->Flags & WFLG_WINDOWACTIVE) {
		flags |= SDL_WINDOW_INPUT_FOCUS;
		SDL_SetKeyboardFocus(window);
	}

	window->flags = flags;

	return MOS_SetupWindowData(_this, window, win);
}

void
MOS_SetWindowTitle(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	// D("[%s] 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		APTR old = data->window_title;
		APTR title = NULL;

		title = MOS_ConvertText(window->title, MIBENUM_UTF_8, MIBENUM_SYSTEM);

		D("[MOS_SetWindowTitle] %s to %s (0x%08lx)\n", window->title, title, data->win);
		SetWindowTitles(data->win, title, title);
			
		data->window_title = title;

		SDL_free(old);
	}
}

void
MOS_SetWindowIcon(_THIS, SDL_Window * window, SDL_Surface * icon)
{
	//#warning convert this icon to appicon
}

void
MOS_SetWindowPosition(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		SDL_VideoData *vd = data->videodata;
		struct Screen *scr = vd->WScreen;
		size_t bh = 0, top = window->y;

		if (vd->CustomScreen == NULL)
			bh = scr->BarHeight + 1;

		top = MAX(bh, top);

		ChangeWindowBox(data->win, window->x, top, data->win->Width, data->win->Height);
	}
}

void
MOS_SetWindowMinimumSize(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		SDL_VideoData *vd = data->videodata;
		struct Screen *scr = vd->WScreen;
		size_t bh = 0, min_h = window->min_h, min_w = window->min_w;

		if ((window->flags & SDL_WINDOW_BORDERLESS) == 0) {
			min_w += data->win->BorderLeft + data->win->BorderRight;
			min_h += data->win->BorderTop + data->win->BorderBottom;
		}

		if (vd->CustomScreen == NULL)
			bh = scr->BarHeight + 1;

		min_h = MIN(scr->Height - bh, min_h);

		WindowLimits(data->win, min_w, min_h, data->win->MaxWidth, MAX(data->win->MaxHeight, min_h));
	}
}

void
MOS_SetWindowMaximumSize(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		SDL_VideoData *vd = data->videodata;
		struct Screen *scr = vd->WScreen;
		size_t bh = 0, max_h = window->max_h, max_w = window->max_w;

		if ((window->flags & SDL_WINDOW_BORDERLESS) == 0) {
			max_w += data->win->BorderLeft + data->win->BorderRight;
			max_h += data->win->BorderTop + data->win->BorderBottom;
		}

		if (vd->CustomScreen == NULL)
			bh = scr->BarHeight + 1;

		max_h = MIN(scr->Height - bh, max_h);

		WindowLimits(data->win, data->win->MinWidth, MIN(data->win->MinHeight, max_h), max_w, max_h);
	}
}

void
MOS_SetWindowSize(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s]\n", __FUNCTION__);

	if (data->win) {
		SDL_VideoData *vd = data->videodata;
		struct Screen *scr = vd->WScreen;
		size_t bh = 0, h = window->h, w = window->w; 

		if ((window->flags & SDL_WINDOW_BORDERLESS) == 0) {
			w += data->win->BorderLeft + data->win->BorderRight;
			h += data->win->BorderTop + data->win->BorderBottom;
		}

		if (vd->CustomScreen == NULL)
			bh = scr->BarHeight + 1;

		h = MIN(scr->Height - bh, h);

		ChangeWindowBox(data->win, data->win->LeftEdge, data->win->TopEdge, w, h);
	}
}

static void
MOS_WindowToFront(struct Window *win)
{
	D("[%s] wnd 0x%08lx\n", __FUNCTION__, win);
	
	ActivateWindow(win);
	WindowToFront(win);
}

void
MOS_ShowWindow_Internal(_THIS, SDL_Window * window) 
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	SDL_VideoData *vd = data->videodata;
	D("[%s] wnd 0x%08lx, scr 0x%08lx\n", __FUNCTION__, data->win, vd->WScreen);

	if (data->win == NULL && (data->sdlflags & SDL_WINDOW_MINIMIZED) == 0) {
		struct Screen *scr;
		size_t flags = WFLG_SIMPLE_REFRESH | WFLG_REPORTMOUSE | WFLG_ACTIVATE;
		size_t w = window->w, h = window->h, max_w = window->max_w, max_h = window->max_h;
		size_t min_w = window->min_w, min_h = window->min_h;
		size_t left = window->x, top = window->y;
		int maxheight, barheight = 0;
		BYTE fullscreen = data->winflags & SDL_MOS_WINDOW_FULLSCREEN;

		data->winflags |= SDL_MOS_WINDOW_SHOWN;

		D("[%s] initial sizes %ld/%ld and %ld/%ld\n", __FUNCTION__, w, h, max_w, max_h);

		if (vd->WScreen == NULL)
			MOS_GetScreen(vd->VideoDevice, vd->FullScreen, (window->flags & SDL_WINDOW_OPENGL) != 0);

		scr = vd->WScreen;

		D("[%s] screen 0x%08lx is %ld/%ld and %ld/%ld\n", __FUNCTION__, scr, scr->Width, scr->Height);

		// Add border sizes
		APTR di = GetScreenDrawInfo(scr);

		if (vd->CustomScreen == NULL)
			barheight = GetSkinInfoAttrA(di, SI_ScreenTitlebarHeight, NULL);

		FreeScreenDrawInfo(scr, di);
		maxheight = scr->Height - barheight;

		if (fullscreen) {
			w = scr->Width;
			h = maxheight;
			top = left = 0;
		} else if (data->sdlflags & SDL_WINDOW_MAXIMIZED) { // Maximize window
			int border_w = GetSkinInfoAttrA(di, SI_BorderLeft    , NULL) + GetSkinInfoAttrA(di, SI_BorderRight, NULL);
			int border_h = GetSkinInfoAttrA(di, SI_BorderTopTitle, NULL) + GetSkinInfoAttrA(di, window->flags & SDL_WINDOW_RESIZABLE ? SI_BorderBottomSize : SI_BorderBottom, NULL);

			D("[%s] Border width %ld, border height %ld, bar height %ld\n", __FUNCTION__, border_w, border_h, barheight);

			max_w = MAX(w, max_w) - border_w;
			max_h = MAX(h, max_h) - border_h;
			max_h = MIN(maxheight - border_h, max_h);

			left = 0;
			top = barheight;

			w = max_w;
			h = max_h;

			D("[%s] maximize to %ld/%ld\n", __FUNCTION__, w, h);
		}
		
		if (!fullscreen) {
			data->visualinfo = GetVisualInfoA(vd->WScreen, NULL);
			if (data->visualinfo) {
				data->menu = CreateMenusA(SDL_NewMenu, NULL);
				if (data->menu) {
					LayoutMenusA(data->menu, data->visualinfo, NULL);
				}
			}
		}

		min_w = MIN(min_w, scr->Width);
		min_h = MIN(min_h, maxheight);
		max_w = MIN(max_w, scr->Width);
		max_h = MIN(max_h, maxheight);
		w = MAX(min_w, w);
		h = MAX(min_h, h);

		if (window->flags & SDL_WINDOW_BORDERLESS || fullscreen)
			flags |= WFLG_BORDERLESS;
		else
			flags |= WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET;

		SDL_bool win_resizable = (window->flags & SDL_WINDOW_RESIZABLE && !fullscreen);
		SDL_bool win_top = (window->flags & SDL_WINDOW_ALWAYS_ON_TOP && !fullscreen);
		if (win_resizable)
			flags |= WFLG_SIZEGADGET | WFLG_SIZEBBOTTOM;

		if (data->window_title == NULL)
			data->window_title = MOS_ConvertText(window->title, MIBENUM_UTF_8, MIBENUM_SYSTEM);

		D("[%s] min %ld/%ld, normal %ld/%ld, max %ld/%ld\n", __FUNCTION__, min_w, min_h, w, h, max_w, max_h);

		data->win = OpenWindowTags(NULL,
			WA_Left, left, WA_Top, top,
			WA_InnerWidth, w,
			WA_InnerHeight, h,
			WA_MinWidth, win_resizable ? 32 : min_w,
			WA_MinHeight, win_resizable ? 32 : min_h,
			WA_MaxWidth, win_resizable ? -1 : max_w,
			WA_MaxHeight, win_resizable ? -1 : max_h,
			WA_Flags, flags,
			vd->CustomScreen ? WA_CustomScreen : TAG_IGNORE, vd->CustomScreen,
			vd->CustomScreen ? TAG_IGNORE : WA_PubScreen, vd->WScreen,
			data->region ? WA_TransparentRegion : TAG_IGNORE, data->region,
			WA_ScreenTitle, data->window_title,
			fullscreen ? TAG_IGNORE : WA_Title, data->window_title,
			WA_UserPort, &vd->WinPort,
			WA_AutoAdjust, TRUE,
			WA_FrontWindow, win_top ? TRUE : FALSE,
			WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_DELTAMOVE | IDCMP_MOUSEBUTTONS | IDCMP_REFRESHWINDOW | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | IDCMP_CHANGEWINDOW | IDCMP_GADGETUP | IDCMP_MENUPICK,
			WA_ExtraTitlebarGadgets, ETG_ICONIFY,
			TAG_DONE);

		if (data->win) {
			if (IS_SYSTEM_CURSOR(vd->CurrentPointer)) {
				size_t pointertags[] = { WA_PointerType, vd->CurrentPointer == NULL ? POINTERTYPE_INVISIBLE : (size_t)vd->CurrentPointer->driverdata, TAG_DONE };
				SetAttrsA(data->win, (struct TagItem *)&pointertags);
			} else{
				SDL_MOSCursor *ac = (SDL_MOSCursor *)vd->CurrentPointer;
				if (ac->Pointer.mouseptr) {
					SetWindowPointer(data->win,WA_Pointer,(size_t)ac->Pointer.mouseptr,TAG_DONE);
				}
			}

			data->curr_x = data->win->LeftEdge;
			data->curr_y = data->win->TopEdge;
			data->curr_w = data->win->Width;
			data->curr_h = data->win->Height;

			data->first_deltamove = TRUE;

			data->win->UserData = (APTR)data;
			
			if (data->menu) {
				SetMenuStrip(data->win, data->menu);

				char *priority = SDL_getenv("SDL_THREAD_PRIORITY_POLICY");
				if (priority && strlen(priority)>0 && strcmp(priority, "-1")==0) {
					SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);
					MOS_GlobalMenu(data->menu, 1, 1, 0, 1);
				}
				char *renderdriver = SDL_getenv("SDL_RENDER_DRIVER");
				if (renderdriver && strlen(renderdriver)>0) {
					if (strcmp(renderdriver, "opengl")==0) {
						MOS_GlobalMenu(data->menu, 1, 3, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 3, 1, 1);
						MOS_GlobalMenu(data->menu, 1, 3, 2, 0);
						SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
					}
					if (strcmp(renderdriver, "software")==0) {
						MOS_GlobalMenu(data->menu, 1, 3, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 3, 1, 0);
						MOS_GlobalMenu(data->menu, 1, 3, 2, 1);
						SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
					}
				}
				char *scale = SDL_getenv("SDL_RENDER_SCALE_QUALITY");
				if (scale && strlen(scale)>0) {
					if (strcmp(scale, "nearest")==0) {
						MOS_GlobalMenu(data->menu, 1, 4, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 4, 1, 1);
						MOS_GlobalMenu(data->menu, 1, 4, 2, 0);
						SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
					}
					if (strcmp(scale, "linear")==0) {
						MOS_GlobalMenu(data->menu, 1, 4, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 4, 1, 0);
						MOS_GlobalMenu(data->menu, 1, 4, 2, 1);
						SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
					}
				}
				char *logical = SDL_getenv("SDL_RENDER_LOGICAL_SIZE_MODE");
				if (logical && strlen(logical)>0) {
					if (strcmp(logical, "0")==0) {
						MOS_GlobalMenu(data->menu, 1, 5, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 5, 1, 1);
						MOS_GlobalMenu(data->menu, 1, 5, 2, 0);
						SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "0");
					}
					if (strcmp(logical, "1")==0) {
						MOS_GlobalMenu(data->menu, 1, 5, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 5, 1, 0);
						MOS_GlobalMenu(data->menu, 1, 5, 2, 1);
						SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "1");
					}
				}
				char *renderbatching = SDL_getenv("SDL_RENDER_BATCHING");
				if (renderbatching && strlen(renderbatching)>0) {
					if (strcmp(renderbatching, "0")==0) {
						MOS_GlobalMenu(data->menu, 1, 6, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 6, 1, 1);
						MOS_GlobalMenu(data->menu, 1, 6, 2, 0);
						SDL_SetHint(SDL_HINT_RENDER_BATCHING, "0");
					}
					if (strcmp(renderbatching, "1")==0) {
						MOS_GlobalMenu(data->menu, 1, 6, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 6, 1, 0);
						MOS_GlobalMenu(data->menu, 1, 6, 2, 1);
						SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
					}
				}
				char *renderline = SDL_getenv("SDL_RENDER_LINE_METHOD");
				if (renderline && strlen(renderline)>0) {
					if (strcmp(renderline, "1")==0) {
						MOS_GlobalMenu(data->menu, 1, 7, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 1, 1);
						MOS_GlobalMenu(data->menu, 1, 7, 2, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 3, 0);
						SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "1");
					}
					if (strcmp(renderline, "2")==0) {
						MOS_GlobalMenu(data->menu, 1, 7, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 1, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 2, 1);
						MOS_GlobalMenu(data->menu, 1, 7, 3, 0);
						SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "2");
					}
					if (strcmp(renderline, "3")==0) {
						MOS_GlobalMenu(data->menu, 1, 7, 0, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 1, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 2, 0);
						MOS_GlobalMenu(data->menu, 1, 7, 3, 1);
						SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "3");
					}
				}
			}
			
			if (!data->appmsg) {
				data->appmsg = AddAppWindow(0, (ULONG)window, data->win, &vd->WBPort, TAG_DONE);
				if (!data->appmsg) 
					D("[%s] ERROR AddAppWindow \n", __FUNCTION__);
			}
			
			if (data->grabbed > 0)
				DoMethod((Object *)data->win, WM_ObtainEvents);
					
		}
		
	} else if (data->win)	{
		MOS_WindowToFront(data->win);
	}
}

void
MOS_ShowWindow(_THIS, SDL_Window * window)
{
	D("[%s]\n", __FUNCTION__);

	MOS_ShowWindow_Internal(_this, window);
}

void
MOS_HideWindow_Internal(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		MOS_CloseWindowSafely(window, data->win);
		data->win = NULL;
	}
}

void
MOS_HideWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s]\n", __FUNCTION__);

	data->winflags &= ~SDL_MOS_WINDOW_SHOWN;
	MOS_HideWindow_Internal(_this, window);
}

void
MOS_RaiseWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] wnd 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win)
		MOS_WindowToFront(data->win);
}

void
MOS_MaximizeWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	SDL_VideoData *videodata = (SDL_VideoData *) data->videodata;
	struct Screen *scr = data->win->WScreen;
	ULONG bh = 0;

	D("[%s] wnd 0x%08lx\n", __FUNCTION__, data->win);

	data->sdlflags |=  SDL_WINDOW_MAXIMIZED;
	data->sdlflags &= ~SDL_WINDOW_MINIMIZED;

	if (data->win) {
		if (videodata->CustomScreen == NULL)
			bh = scr->BarHeight + 1;

		ChangeWindowBox(data->win, 0, bh, data->win->MaxWidth, data->win->MaxHeight);
	}
}

void
MOS_MinimizeWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] wnd 0x%08lx\n", __FUNCTION__, data->win);

	data->sdlflags |=  SDL_WINDOW_MINIMIZED;
	data->sdlflags &= ~SDL_WINDOW_MAXIMIZED;

	MOS_HideWindow(_this, window);
}

void
MOS_RestoreWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] wnd 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		data->sdlflags &= ~(SDL_WINDOW_MINIMIZED | SDL_WINDOW_MAXIMIZED);

		ChangeWindowBox(data->win, window->x, window->y, window->w, window->h);
		MOS_WindowToFront(data->win);
	}
}

void
MOS_SetWindowFullscreen(_THIS, SDL_Window * window, SDL_VideoDisplay * _display, SDL_bool fullscreen)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	SDL_VideoData *vd = data->videodata;

	D("[%s]\n", __FUNCTION__);

	if (fullscreen)
		data->winflags |= SDL_MOS_WINDOW_FULLSCREEN;
	else
		data->winflags &= ~SDL_MOS_WINDOW_FULLSCREEN;

	vd->FullScreen = fullscreen;

	MOS_OpenWindows(_this);
	if (data->__tglContext) MOS_GL_ResizeContext(_this, window);
}


int
MOS_SetWindowGammaRamp(_THIS, SDL_Window * window, const Uint16 * ramp)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	SDL_VideoData *vd = data->videodata;
	int i;

	for (i = 0; i < 256; i++) {
		vd->gammatable[0][i] = ramp[0*256 + i] >> 8;
		vd->gammatable[1][i] = ramp[1*256 + i] >> 8;
		vd->gammatable[2][i] = ramp[2*256 + i] >> 8;
	}

	if (vd->CustomScreen) {
		SetAttrs(vd->CustomScreen,
			SA_GammaRed,   (IPTR)vd->gammatable[0],
			SA_GammaGreen, (IPTR)vd->gammatable[1],
			SA_GammaBlue,  (IPTR)vd->gammatable[2],
			TAG_DONE);
	}

	return 0;
}

void
MOS_SetWindowGrab(_THIS, SDL_Window * window, SDL_bool grabbed)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;

	if (data->win && data->grabbed != (grabbed ? 1 : 0)) {
		D("[%s] %s\n", __FUNCTION__, grabbed ? "grabbed" : "not grabbed");

		data->grabbed = grabbed ? 1 : 0;

		if (grabbed && (data->win->Flags & WFLG_WINDOWACTIVE) == 0)
			MOS_WindowToFront(data->win);

		DoMethod((Object *)data->win, grabbed ? WM_ObtainEvents : WM_ReleaseEvents);
	}
}

void
MOS_DestroyWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s]\n", __FUNCTION__);
	
	window->driverdata = NULL;

	if (data) {
		SDL_VideoData *videodata = (SDL_VideoData *) data->videodata;

		REMOVE(&data->node);

		if (data->win) {
			MOS_CloseWindowSafely(window, data->win);
			data->__tglContext = NULL;
			data->win = NULL;
		}
		if (data->region)
			DisposeRegion(data->region);

		if (IsListEmpty((struct List *)&videodata->windowlist)) {
			D("[%s] Was last window... get rid of screen.\n", __FUNCTION__);
			MOS_CloseDisplay(_this);
		}

		SDL_free(data->window_title);
		SDL_free(data);
	}
}

SDL_bool
MOS_GetWindowWMInfo(_THIS, SDL_Window * window, struct SDL_SysWMinfo * info)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;

	if (info->version.major >= SDL_MAJOR_VERSION) {
		info->subsystem = SDL_SYSWM_MORPHOS;
		info->info.intui.window = data->win;
		return SDL_TRUE;
	} else {
		SDL_SetError("Application not compiled with SDL %d\n", SDL_MAJOR_VERSION);
		return SDL_FALSE;
	}
}

static void MOS_CloseWindow(SDL_Window *window) 
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] 0x%08lx\n", __FUNCTION__, data->win);

	if (data->win) {
		if (data->appmsg) {
			if (RemoveAppWindow(data->appmsg)) {
				data->appmsg = NULL;
			}
		}
	
		struct Window *win = data->win;
		CloseWindow(win);
		data->win = NULL;	
	}
	
}

static 
void MOS_RecreateWindow(_THIS, SDL_Window * window)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	D("[%s] 0x%08lx\n", __FUNCTION__, data->win);
	
    if (window->flags & SDL_WINDOW_FOREIGN) {
        D("[%s] Cannot modify native window '%s'\n", __FUNCTION__, window->title);
        return;
    }

    if (data->win) {
        D("[%s] Closing system window '%s' before re-creation\n", __FUNCTION__, window->title);
        MOS_CloseWindow(window);
    }

    MOS_ShowWindow_Internal(_this, window);

    if (data->win) {
        // Make sure the new window is active
        MOS_ShowWindow(_this, window);
    } else {
        D("[%s] Failed to re-create window '%s'\n", __FUNCTION__, window->title);
    }
}

void
MOS_SetWindowAlwaysOnTop(_THIS, SDL_Window * window, SDL_bool on_top)
{
	D("[%s]\n", __FUNCTION__);
	MOS_RecreateWindow(_this, window);
}

int
MOS_SetWindowHitTest(SDL_Window *window, SDL_bool enabled)
{
    return 0;  /* just succeed, the real work is done elsewhere. */
}

void
MOS_SetWindowResizable(_THIS, SDL_Window * window, SDL_bool resizable)
{
	D("[%s]\n", __FUNCTION__);
	MOS_RecreateWindow(_this, window);
}

void
MOS_SetWindowBordered(_THIS, SDL_Window * window, SDL_bool bordered)
{
	D("[%s]\n", __FUNCTION__);
	MOS_RecreateWindow(_this, window);
}

static SDL_bool
MOS_SetWindowOpacityPrivate(_THIS, struct Window * window, ULONG value)
{
	
	LONG ret = SetAttrs(window, WA_Opacity, value, TAG_DONE);
	if (ret) {
		D("[%s] Failed to set window opaqueness to %d\n", __FUNCTION__, value);
		return SDL_FALSE;
	}
	
	return SDL_TRUE;
}

int
MOS_SetWindowOpacity(_THIS, SDL_Window * window, float opacity)
{
	D("[%s] \n", __FUNCTION__);
	
    SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	if (!data->win) return 0;
	
    ULONG value = ((opacity) * (ULONG_MAX));

    return MOS_SetWindowOpacityPrivate(_this, data->win, value) ? 0 : -1;
}

static void
MOS_FlashOperationPrivate(_THIS, struct Window * window)
{
	ULONG value_old = 0;
	
	GetAttr(WA_Opacity, window, &value_old);

	WindowToFront(window);
	const Uint32 start = SDL_GetTicks();
	ULONG elapsed = 0;
	ULONG value = 0;
	
	while (TRUE) {
		elapsed = SDL_GetTicks() - start;
		if (elapsed > 200) break;
		value = 128 + 127 * sinf(elapsed * 3.14159f / 50.0f) * (ULONG_MAX);
		MOS_SetWindowOpacityPrivate(_this, window, value);	
		SDL_Delay(1);
	}
	
	ActivateWindow(window);
	MOS_SetWindowOpacityPrivate(_this, window, value_old);
}

int
MOS_FlashWindow(_THIS, SDL_Window * window, SDL_FlashOperation operation)
{
	D("[%s] \n", __FUNCTION__);
	
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	if (!data->win) return 0;
	
	switch(operation)
	{
		case SDL_FLASH_BRIEFLY:
		case SDL_FLASH_UNTIL_FOCUSED:
			MOS_FlashOperationPrivate(_this, data->win);
			break;
		case SDL_FLASH_CANCEL:
			break;
	}
	
	return 0;
}

int
MOS_GetWindowBordersSize(_THIS, SDL_Window * window, int *top, int *left, int *bottom, int *right)
{
	SDL_WindowData *data = (SDL_WindowData *) window->driverdata;
	
	if (top)
		*top = data->win->BorderTop;
	
	if (left)
		*left = data->win->BorderLeft;
	
	if (bottom)
		*bottom = data->win->BorderBottom;
	
	if (right)
		*right = data->win->BorderRight;	
	
	return 0;
}

/* vi: set ts=4 sw=4 expandtab: */