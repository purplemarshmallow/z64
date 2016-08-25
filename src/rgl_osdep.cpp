/*
 * z64
 *
 * This program is free software; you can redistribute it and/
 * or modify it under the terms of the GNU General Public Li-
 * cence as published by the Free Software Foundation; either
 * version 2 of the Licence, or any later version.
 *
 * This program is distributed in the hope that it will be use-
 * ful, but WITHOUT ANY WARRANTY; without even the implied war-
 * ranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public Licence for more details.
 *
 * You should have received a copy of the GNU General Public
 * Licence along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 *
**/

#include "Gfx #1.3.h"
#include "rdp.h"
#include "rgl.h"
#include "rgl_glut.h"

#include <SDL.h>

SDL_Surface *sdl_Screen;
int viewportOffset;

#ifdef WIN32
//int screen_width = 640, screen_height = 480;
int screen_width = 1024, screen_height = 768;
//int screen_width = 320, screen_height = 240;
#else
//int screen_width = 640, screen_height = 480;
int screen_width = 1024, screen_height = 768;
//int screen_width = 320, screen_height = 240;
#endif
int viewport_offset;
#ifdef WIN32
int pc_width, pc_height;
static HDC hDC = NULL;
static HGLRC hGLRC = NULL;
static HWND hToolBar = NULL;
static HWND hwnd_win = NULL;
static unsigned long windowedExStyle, windowedStyle;
static RECT windowedRect;
static HMENU windowedMenu;

# include <fcntl.h>
# ifndef ATTACH_PARENT_PROCESS
#  define ATTACH_PARENT_PROCESS ((DWORD)-1)
# endif

typedef BOOL (WINAPI * AttachConsoleType)(DWORD);
BOOL (WINAPI * AttachConsolePTR)(DWORD);
#endif // _WIN32

#ifdef RGL_USE_GLUT

static void glut_rdp_init()
{
  rdp_init();
}

#else

void rglSwapBuffers()
{
#ifdef _WIN32
	SwapBuffers(wglGetCurrentDC());
#else // _WIN32
  SDL_GL_SwapBuffers();
#endif // _WIN32
}

#endif


#ifdef WIN32
BOOL CALLBACK FindToolBarProc(HWND hWnd, LPARAM lParam)
{
	if (GetWindowLong(hWnd, GWL_STYLE) & RBS_VARHEIGHT)
	{
		hToolBar = hWnd;
		return FALSE;
	}
	return TRUE;
}
#endif // _WIN32


int rglOpenScreen()
{
#if defined(WIN32) && defined(RGL_ASSERT)
  static int win32console;
  if (!win32console) {
    FILE * newstdout = freopen("z64log.txt", "w", stdout);
    FILE * newstderr = freopen("z64err.txt", "w", stderr);
    //_dup2(_fileno(newstdout), _fileno(stderr));
    win32console = 1;
  }
#endif
  
  rglRestoreSettings();
  rglReadSettings();
  
	if (rglStatus == RGL_STATUS_WINDOWED) {
    screen_width = rglSettings.resX;
    screen_height = rglSettings.resY;
  } else {
    screen_width = rglSettings.fsResX;
    screen_height = rglSettings.fsResY;
  }
  
  viewportOffset = 0;

#ifdef RGL_USE_GLUT

  rglGlutCreateThread(1);

  rglGlutPostCommand(glut_rdp_init);
  
#else
#ifdef WIN32
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                       // version number
		PFD_DRAW_TO_WINDOW |     // support window
		PFD_SUPPORT_OPENGL |     // support OpenGL
		PFD_GENERIC_ACCELERATED | //PFD_SWAP_COPY | PFD_SWAP_EXCHANGE |
		PFD_DOUBLEBUFFER,        // double buffered
		PFD_TYPE_RGBA,           // RGBA type
		32,
		0, 0, 0, 0, 0, 0,        // color bits ignored
		0,                       // no alpha buffer
		0,                       // shift bit ignored
		0,                       // no accumulation buffer
		0, 0, 0, 0,              // accum bits ignored
		24,        // z-buffer      
		0,                       // no stencil buffer
		0,                       // no auxiliary buffer
		PFD_MAIN_PLANE,          // main layer
		0,                       // reserved
		0, 0, 0};                // layer masks ignored
	int pfm;
	RECT windowRect, toolRect, statusRect;
	int pc_width, pc_height;

  HWND hWnd = gfx.hWnd;
	if ((HWND)hWnd == NULL) hWnd = GetActiveWindow();
	hwnd_win = (HWND)hWnd;

	if (rglStatus == RGL_STATUS_WINDOWED)
	{
		ChangeDisplaySettings(NULL, 0);
		GetClientRect(hwnd_win, &windowRect);
		EnumChildWindows(hwnd_win, FindToolBarProc, 0);

		if (hToolBar)
			GetWindowRect(hToolBar, &toolRect);
		else
			toolRect.bottom = toolRect.top = 0;

		windowRect.right = windowRect.left + screen_width - 1;
		windowRect.bottom = windowRect.top + screen_height - 1 + 40;
		AdjustWindowRect(&windowRect, GetWindowLong(hwnd_win, GWL_STYLE), GetMenu(hwnd_win) != NULL);

		SetWindowPos(hwnd_win, NULL, 0, 0, windowRect.right - windowRect.left + 1,
					windowRect.bottom - windowRect.top + 1 /*+ toolRect.bottom - toolRect.top + 1*/, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

		if (gfx.hStatusBar)
			GetWindowRect(gfx.hStatusBar, &statusRect);
		else
			statusRect.bottom = statusRect.top = 0;

		viewportOffset = statusRect.bottom - statusRect.top;
		LOG("viewportOffset %d\n", viewportOffset);
	}
	else
	{
		DEVMODE fullscreenMode;
		DEVMODE currentMode;
		
		if (gfx.hStatusBar)
		  ShowWindow( gfx.hStatusBar, SW_HIDE );

		pc_width = screen_width;
		pc_height = screen_height;
		memset(&fullscreenMode, 0, sizeof(DEVMODE));
		fullscreenMode.dmSize = sizeof(DEVMODE);
		fullscreenMode.dmPelsWidth= pc_width;
		fullscreenMode.dmPelsHeight= pc_height;
		fullscreenMode.dmBitsPerPel= 32;
		fullscreenMode.dmDisplayFrequency= 60;
		fullscreenMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &currentMode);
		fullscreenMode.dmDisplayFrequency = currentMode.dmDisplayFrequency;

		if (ChangeDisplaySettings( &fullscreenMode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL)
		{
			LOGERROR("can't change to fullscreen mode");
		}
		ShowCursor(FALSE);

		windowedExStyle = GetWindowLong(hwnd_win, GWL_EXSTYLE);
		windowedStyle = GetWindowLong(hwnd_win, GWL_STYLE);

		SetWindowLong(hwnd_win, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLong(hwnd_win, GWL_STYLE, WS_POPUP);

		GetWindowRect(hwnd_win, &windowedRect);
		windowedMenu = GetMenu(hwnd_win);
		if (windowedMenu) SetMenu(hwnd_win, NULL);

		SetWindowPos(hwnd_win, NULL, 0, 0, pc_width, pc_height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	
	if ((hDC = GetDC(hwnd_win)) == NULL)
	{
		LOGERROR("GetDC on main window failed");
		return -1;
	}
	SetViewportExtEx(hDC, screen_width, screen_height, NULL);
	SetWindowExtEx(hDC, screen_width, screen_height, NULL);

	if ((pfm = ChoosePixelFormat(hDC, &pfd)) == 0) {
    printf("disabling auxiliary buffers\n");
    pfd.cAuxBuffers = 0;
    pfm = ChoosePixelFormat(hDC, &pfd);
  }
	if (pfm == 0)
	{
		LOGERROR("ChoosePixelFormat failed");
		return -1;
	}
	if (SetPixelFormat(hDC, pfm, &pfd) == FALSE)
	{
		LOGERROR("SetPixelFormat failed");
		return -1;
	}

	DescribePixelFormat(hDC, pfm, sizeof(pfd), &pfd);
	
	if ((hGLRC = wglCreateContext(hDC)) == 0)
	{
		LOGERROR("wglCreateContext failed!");
    rglCloseScreen();
		return -1;
	}

	if (!wglMakeCurrent(hDC, hGLRC))
	{
		LOGERROR("wglMakeCurrent failed!");
    rglCloseScreen();
		return -1;
	}
  
#else // WIN32
  const SDL_VideoInfo *videoInfo;
  Uint32 videoFlags = 0;

  /* Initialize SDL */
  printf("(II) Initializing SDL video subsystem...\n");
  if(SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
  {
    printf("(EE) Error initializing SDL video subsystem: %s\n", SDL_GetError());
    return -1;
  }
   
  /* Video Info */
  printf("(II) Getting video info...\n");
  if(!(videoInfo = SDL_GetVideoInfo()))
  {
    printf("(EE) Video query failed: %s\n", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return -1;
  }
   
  /* Setting the video mode */
  videoFlags |= SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;
  
  if(videoInfo->hw_available)
    videoFlags |= SDL_HWSURFACE;
  else
    videoFlags |= SDL_SWSURFACE;
  
  if(videoInfo->blit_hw)
    videoFlags |= SDL_HWACCEL;

  if (rglStatus == RGL_STATUS_FULLSCREEN)
    videoFlags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
   
  printf("(II) Setting video mode %dx%d...\n", screen_width, screen_height);
  if(!(sdl_Screen = SDL_SetVideoMode(screen_width, screen_height, 0, videoFlags)))
  {
    printf("(EE) Error setting videomode %dx%d: %s\n", screen_width, screen_height, SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return -1;
  }
   
  char caption[500];
  sprintf(caption, "z64, LLE video plugin by Ziggy");
  SDL_WM_SetCaption(caption, caption);
#endif // else WIN32

    printf("OpenGL device support errors:  %i\n", init_GL_extensions());

  rdp_init();
  //rsp_init(0);
#endif

  return 0;
}

void rglCloseScreen()
{
#ifdef RGL_USE_GLUT
  rglGlutCreateThread(0);

  rglGlutPostCommand(rglGlutMinimizeWindow);
#else
# ifdef WIN32
	if (hGLRC)
	{
    wglMakeCurrent(hDC, hGLRC);
    rglClose();
    
    wglMakeCurrent(NULL,NULL);
		wglDeleteContext(hGLRC);
		hGLRC = NULL;
	}
  /*
  if (hDC != NULL) 
  {
	  ReleaseDC(hwnd_win,hDC);
    hDC = NULL;
  }
	//*/
  ShowCursor(TRUE);
  if (gfx.hStatusBar)
    ShowWindow( gfx.hStatusBar, SW_SHOW );
	if (rglStatus == RGL_STATUS_FULLSCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
		SetWindowLong(hwnd_win, GWL_STYLE, windowedStyle);
		SetWindowLong(hwnd_win, GWL_EXSTYLE, windowedExStyle);
		SetWindowPos(hwnd_win, NULL, windowedRect.left, windowedRect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		if (windowedMenu) SetMenu(hwnd_win, windowedMenu);
	}
# else // WIN32
  if (!sdl_Screen)
    return;
  rglClose();
  //SDL_QuitSubSystem(SDL_INIT_VIDEO);
  //sleep(2);
  sdl_Screen = NULL;
# endif // else WIN32
#endif
}

#ifdef WIN32
void rglWin32Windowed()
{
  if (rglStatus != RGL_STATUS_FULLSCREEN) return;
//   ChangeDisplaySettings(NULL, 0);
//   SetWindowLong(hwnd_win, GWL_STYLE, windowedStyle);
//   SetWindowLong(hwnd_win, GWL_EXSTYLE, windowedExStyle);
//   SetWindowPos(hwnd_win, NULL, windowedRect.left, windowedRect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
//   if (windowedMenu) SetMenu(hwnd_win, windowedMenu);
}
#endif

EXPORT void CALL ChangeWindow (void)
{
	b_fullscreen = !b_fullscreen;
//  if (rglNextStatus == RGL_STATUS_CLOSED || rglStatus == RGL_STATUS_CLOSED)
//    return;

	if (b_fullscreen)
      rglNextStatus = RGL_STATUS_FULLSCREEN;
	else
      rglNextStatus = RGL_STATUS_WINDOWED;
}

