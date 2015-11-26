#include "Gfx #1.3.h"
#include "z64.h"

#include <SDL.h>

SDL_Surface *sdl_Screen;

int screen_width = 640, screen_height = 480;

int rdp_init();

extern GFX_INFO gfx;

/******************************************************************
  Function: CaptureScreen
  Purpose:  This function dumps the current frame to a file
  input:    pointer to the directory to save the file to
  output:   none
*******************************************************************/ 
EXPORT void CALL CaptureScreen ( char * Directory )
{
}

/******************************************************************
  Function: ChangeWindow
  Purpose:  to change the window between fullscreen and window 
            mode. If the window was in fullscreen this should 
			change the screen to window mode and vice vesa.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL ChangeWindow (void)
{
}

/******************************************************************
  Function: CloseDLL
  Purpose:  This function is called when the emulator is closing
            down allowing the dll to de-initialise.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL CloseDLL (void)
{
}

/******************************************************************
  Function: DllAbout
  Purpose:  This function is optional function that is provided
            to give further information about the DLL.
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/ 
EXPORT void CALL DllAbout ( HWND hParent )
{
}

/******************************************************************
  Function: DllConfig
  Purpose:  This function is optional function that is provided
            to allow the user to configure the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/ 
EXPORT void CALL DllConfig ( HWND hParent )
{
}

/******************************************************************
  Function: DllTest
  Purpose:  This function is optional function that is provided
            to allow the user to test the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/ 
EXPORT void CALL DllTest ( HWND hParent )
{
}


EXPORT void CALL ReadScreen(void **dest, long *width, long *height)
{
}

/******************************************************************
  Function: DrawScreen
  Purpose:  This function is called when the emulator receives a
            WM_PAINT message. This allows the gfx to fit in when
			it is being used in the desktop.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL DrawScreen (void)
{
}

/******************************************************************
  Function: GetDllInfo
  Purpose:  This function allows the emulator to gather information
            about the dll by filling in the PluginInfo structure.
  input:    a pointer to a PLUGIN_INFO stucture that needs to be
            filled by the function. (see def above)
  output:   none
*******************************************************************/ 
EXPORT void CALL GetDllInfo ( PLUGIN_INFO * PluginInfo )
{
  PluginInfo->Version = 0x0103;
  PluginInfo->Type  = PLUGIN_TYPE_GFX;
  sprintf (PluginInfo->Name, "z64");
  
  // If DLL supports memory these memory options then set them to TRUE or FALSE
  //  if it does not support it
  PluginInfo->NormalMemory = TRUE;  // a normal BYTE array
  PluginInfo->MemoryBswaped = TRUE; // a normal BYTE array where the memory has been pre
}

/******************************************************************
  Function: InitiateGFX
  Purpose:  This function is called when the DLL is started to give
            information from the emulator that the n64 graphics
			uses. This is not called from the emulation thread.
  Input:    Gfx_Info is passed to this function which is defined
            above.
  Output:   TRUE on success
            FALSE on failure to initialise
             
  ** note on interrupts **:
  To generate an interrupt set the appropriate bit in MI_INTR_REG
  and then call the function CheckInterrupts to tell the emulator
  that there is a waiting interrupt.
*******************************************************************/ 
GFX_INFO gfx;
EXPORT BOOL CALL InitiateGFX (GFX_INFO Gfx_Info)
{
  gfx = Gfx_Info;
  return TRUE;
}

/******************************************************************
  Function: MoveScreen
  Purpose:  This function is called in response to the emulator
            receiving a WM_MOVE passing the xpos and ypos passed
			from that message.
  input:    xpos - the x-coordinate of the upper-left corner of the
            client area of the window.
			ypos - y-coordinate of the upper-left corner of the
			client area of the window. 
  output:   none
*******************************************************************/ 
EXPORT void CALL MoveScreen (int xpos, int ypos)
{
}

/******************************************************************
  Function: ProcessDList
  Purpose:  This function is called when there is a Dlist to be
            processed. (High level GFX list)
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL ProcessDList(void)
{
//   rsp_init(0);
//   rsp_execute(0x1000000);
}

/******************************************************************
  Function: ProcessRDPList
  Purpose:  This function is called when there is a Dlist to be
            processed. (Low level GFX list)
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL ProcessRDPList(void)
{
//   printf("ProcessRPDList %x %x %x\n",
//          *gfx.DPC_START_REG,
//          *gfx.DPC_END_REG,
//          *gfx.DPC_CURRENT_REG);


//   int i;
//   for (i=*gfx.DPC_CURRENT_REG; i<*gfx.DPC_END_REG; ) {
//     char s[1024];
//     i += rdp_dasm((UINT32*)gfx.RDRAM, i >>2, *gfx.DPC_END_REG - i, s);
//     printf("%s\n", s);
//   }

  void rdp_process_list(void);
  rdp_process_list();
  return;
  
  //*gfx.DPC_STATUS_REG = 0xffffffff; // &= ~0x0002;

  //*gfx.DPC_START_REG = *gfx.DPC_END_REG;
  *gfx.DPC_CURRENT_REG = *gfx.DPC_END_REG;

}

/******************************************************************
  Function: RomClosed
  Purpose:  This function is called when a rom is closed.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL RomClosed (void)
{
  //SDL_QuitSubSystem(SDL_INIT_VIDEO);
  //sleep(2);
  sdl_Screen = NULL;
}

/******************************************************************
  Function: RomOpen
  Purpose:  This function is called when a rom is open. (from the 
            emulation thread)
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL RomOpen (void)
{
  rdp_init();
  //rsp_init(0);

  const SDL_VideoInfo *videoInfo;
  Uint32 videoFlags = 0;
   
  /* Initialize SDL */
  printf("(II) Initializing SDL video subsystem...\n");
  if(SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
  {
    printf("(EE) Error initializing SDL video subsystem: %s\n", SDL_GetError());
    return;
  }
   
  /* Video Info */
  printf("(II) Getting video info...\n");
  if(!(videoInfo = SDL_GetVideoInfo()))
  {
    printf("(EE) Video query failed: %s\n", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
  }
   
  /* Setting the video mode */
  videoFlags |= /*SDL_OPENGL | SDL_GL_DOUBLEBUFFER | */SDL_HWPALETTE;
  
  if(videoInfo->hw_available)
    videoFlags |= SDL_HWSURFACE;
  else
    videoFlags |= SDL_SWSURFACE;
  
  if(videoInfo->blit_hw)
    videoFlags |= SDL_HWACCEL;
  
  //videoFlags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
//   SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
//   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
   
  printf("(II) Setting video mode %dx%d...\n", screen_width, screen_height);
  if(!(sdl_Screen = SDL_SetVideoMode(screen_width, screen_height, 0, videoFlags)))
  {
    printf("(EE) Error setting videomode %dx%d: %s\n", screen_width, screen_height, SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
  }
   
  char caption[500];
  sprintf(caption, "z64, LLE video plugin by Ziggy");
  SDL_WM_SetCaption(caption, caption);
}

/******************************************************************
  Function: ShowCFB
  Purpose:  Useally once Dlists are started being displayed, cfb is
            ignored. This function tells the dll to start displaying
			them again.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL ShowCFB (void)
{
}

/******************************************************************
  Function: UpdateScreen
  Purpose:  This function is called in response to a vsync of the
            screen were the VI bit in MI_INTR_REG has already been
			set
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL UpdateScreen (void)
{
}

/******************************************************************
  Function: ViStatusChanged
  Purpose:  This function is called to notify the dll that the
            ViStatus registers value has been changed.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL ViStatusChanged (void)
{
}

/******************************************************************
  Function: ViWidthChanged
  Purpose:  This function is called to notify the dll that the
            ViWidth registers value has been changed.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL ViWidthChanged (void)
{
}


/******************************************************************
  Function: FrameBufferWrite
  Purpose:  This function is called to notify the dll that the
            frame buffer has been modified by CPU at the given address.
  input:    addr		rdram address
			val			val
			size		1 = BYTE, 2 = WORD, 4 = DWORD
  output:   none
*******************************************************************/ 
EXPORT void CALL FBWrite(DWORD, DWORD)
{
}
/******************************************************************
  Function: FrameBufferWriteList
  Purpose:  This function is called to notify the dll that the
            frame buffer has been modified by CPU at the given address.
  input:    FrameBufferModifyEntry *plist
			size = size of the plist, max = 1024
  output:   none
*******************************************************************/ 
EXPORT void CALL FBWList(FrameBufferModifyEntry *plist, DWORD size)
{
}

/******************************************************************
  Function: FrameBufferRead
  Purpose:  This function is called to notify the dll that the
            frame buffer memory is beening read at the given address.
			DLL should copy content from its render buffer to the frame buffer
			in N64 RDRAM
			DLL is responsible to maintain its own frame buffer memory addr list
			DLL should copy 4KB block content back to RDRAM frame buffer.
			Emulator should not call this function again if other memory
			is read within the same 4KB range
  input:    addr		rdram address
			val			val
			size		1 = BYTE, 2 = WORD, 4 = DWORD
  output:   none
*******************************************************************/ 
EXPORT void CALL FBRead(DWORD addr)
{
}

/************************************************************************
Function: FBGetFrameBufferInfo
Purpose:  This function is called by the emulator core to retrieve depth
buffer information from the video plugin in order to be able
to notify the video plugin about CPU depth buffer read/write
operations

size:
= 1		byte
= 2		word (16 bit) <-- this is N64 default depth buffer format
= 4		dword (32 bit)

when depth buffer information is not available yet, set all values
in the FrameBufferInfo structure to 0

input:    FrameBufferInfo *pinfo
pinfo is pointed to a FrameBufferInfo structure which to be
filled in by this function
output:   Values are return in the FrameBufferInfo structure
************************************************************************/
EXPORT void CALL FBGetFrameBufferInfo(void *pinfo)
{
}

