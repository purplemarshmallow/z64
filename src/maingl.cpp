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

#define THREADED
#ifdef WIN32
#define THREADED
#endif

extern GFX_INFO gfx;

#ifdef THREADED
volatile static int waiting;
SDL_sem * rdpCommandSema;
SDL_sem * rdpCommandCompleteSema;
SDL_Thread * rdpThread;
int rdpThreadFunc(void * dummy)
{
  while (1) {
    SDL_SemWait(rdpCommandSema);
    waiting = 1;
    if (rglNextStatus == RGL_STATUS_CLOSED)
      rglUpdateStatus();
    else
      rdp_process_list();
    if (!rglSettings.async)
      SDL_SemPost(rdpCommandCompleteSema);
#ifndef WIN32
    if (rglStatus == RGL_STATUS_CLOSED) {
      rdpThread = NULL;
      return 0;
    }
#endif
  }
  return 0;
}

void rdpSignalFullSync()
{
  SDL_SemPost(rdpCommandCompleteSema);
}
void rdpWaitFullSync()
{
  SDL_SemWait(rdpCommandCompleteSema);
}

void rdpPostCommand()
{
  int sync = rdp_store_list();
  SDL_SemPost(rdpCommandSema);
  if (!rglSettings.async)
    SDL_SemWait(rdpCommandCompleteSema);
  else if (sync) {
    rdpWaitFullSync();
    *gfx.MI_INTR_REG |= 0x20;
    gfx.CheckInterrupts();
  }
  
  waiting = 0;
}

void rdpCreateThread()
{
  if (!rdpCommandSema) {
    rdpCommandSema = SDL_CreateSemaphore(0);
    rdpCommandCompleteSema = SDL_CreateSemaphore(0);
  }
  if (!rdpThread) {
    LOG("Creating rdp thread\n");
    rdpThread = SDL_CreateThread(rdpThreadFunc, 0);
  }
}
#endif

EXPORT void CALL CloseDLL (void)
{
}

EXPORT void CALL DllAbout ( HWND hParent )
{
}

EXPORT void CALL DllConfig ( HWND hParent )
{
}

EXPORT void CALL DllTest ( HWND hParent )
{
}


EXPORT void CALL ReadScreen(void **dest, long *width, long *height)
{
  LOG("ReadScreen\n");
}

EXPORT void CALL DrawScreen (void)
{
}

EXPORT void CALL GetDllInfo ( PLUGIN_INFO * PluginInfo )
{
  PluginInfo->Version = 0x0103;
  PluginInfo->Type  = PLUGIN_TYPE_GFX;
#ifdef RDP_DEBUG
  sprintf (PluginInfo->Name, "z64gl Debug");
#else
  sprintf(PluginInfo->Name, "z64gl");
#endif
  
  // If DLL supports memory these memory options then set them to TRUE or FALSE
  //  if it does not support it
  PluginInfo->NormalMemory = TRUE;  // a normal BYTE array
  PluginInfo->MemoryBswaped = TRUE; // a normal BYTE array where the memory has been pre
}

GFX_INFO gfx;
EXPORT BOOL CALL InitiateGFX (GFX_INFO Gfx_Info)
{
  LOG("InitiateGFX\n");
  gfx = Gfx_Info;
  memset(rdpTiles, 0, sizeof(rdpTiles));
  memset(rdpTmem, 0, 0x1000);
  memset(&rdpState, 0, sizeof(rdpState));
#ifdef THREADED
  rglSettings.threaded = 1;
  rglReadSettings();
  if (rglSettings.threaded)
    rdpCreateThread();
#endif
  return TRUE;
}

EXPORT void CALL MoveScreen (int xpos, int ypos)
{
}

EXPORT void CALL ProcessDList(void)
{
}

static void glut_rdp_process_list()
{
  rdp_process_list();
}

EXPORT void CALL ProcessRDPList(void)
{
#ifdef RGL_USE_GLUT
  rglGlutPostCommand(glut_rdp_process_list);
#else
#ifdef THREADED
  if (rglSettings.threaded) {
    rdpCreateThread();
    rdpPostCommand();
  } else
#endif
  {
    rdp_process_list();
  }
#endif
  
  return;
}

EXPORT void CALL RomClosed (void)
{
#ifdef THREADED
  if (rglSettings.threaded) {
#ifdef WIN32
    fflush(stdout); fflush(stderr);
    //while (waiting); // temporary hack
#endif
    rglNextStatus = RGL_STATUS_CLOSED;
#ifndef WIN32
    do
      rdpPostCommand();
    while (rglStatus != RGL_STATUS_CLOSED);
#else
    void rglWin32Windowed();
    rglWin32Windowed();
#endif
  } else
#endif
  {
    rglNextStatus = RGL_STATUS_CLOSED;
	rglUpdateStatus();
  }
}

EXPORT void CALL RomOpen (void)
{
	if (!b_fullscreen)
		rglNextStatus = RGL_STATUS_WINDOWED;
	else
		rglNextStatus = RGL_STATUS_FULLSCREEN;
#ifdef THREADED
  if (rglSettings.threaded) {
    rdpCreateThread();
  }
  else
#endif
	rglUpdateStatus();
}

EXPORT void CALL ShowCFB (void)
{
}

EXPORT void CALL UpdateScreen (void)
{
#ifdef THREADED
  if (rglSettings.threaded) {
    rdpPostCommand();
  } else
#endif
  {
    rglUpdateStatus();
    rglUpdate();
  }
}

EXPORT void CALL ViStatusChanged (void)
{
}

EXPORT void CALL ViWidthChanged (void)
{
}


