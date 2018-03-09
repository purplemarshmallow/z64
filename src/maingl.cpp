/*
* z64
*
* Copyright (C) 2007  ziggy
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
**/

#include <SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "rdp.h"
#include "rgl.h"

bool bcapture = false;
char * capture_dir;

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

        if (rglStatus == RGL_STATUS_CLOSED) {
            rdpThread = NULL;
            return 0;
        }
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
#if SDL_VERSION_ATLEAST(2,0,0)
        rdpThread = SDL_CreateThread(rdpThreadFunc, "z64rdp", 0);
#else
        rdpThread = SDL_CreateThread(rdpThreadFunc, 0);
#endif
    }
}
#endif
