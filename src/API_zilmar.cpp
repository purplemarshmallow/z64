#include "Gfx #1.3.h"

#include "rdp.h"
#include "rgl.h"

GFX_INFO gfx;

#ifdef __cplusplus
extern "C" {
#endif

    EXPORT void CALL CaptureScreen(char* Directory)
    {
        capture_dir = Directory;
        bcapture = true;
    }

    EXPORT void CALL CloseDLL(void)
    {
    }

    EXPORT void CALL DllAbout(HWND hParent)
    {
    }

    EXPORT void CALL DllConfig(HWND hParent)
    {
    }

    EXPORT void CALL DllTest(HWND hParent)
    {
    }


    EXPORT void CALL ReadScreen(void **dest, long *width, long *height)
    {
        LOG("ReadScreen\n");
    }

    EXPORT void CALL DrawScreen(void)
    {
    }

    EXPORT void CALL GetDllInfo(PLUGIN_INFO * PluginInfo)
    {
        PluginInfo->Version = 0x0103;
        PluginInfo->Type = PLUGIN_TYPE_GFX;
#ifdef RDP_DEBUG
        sprintf(PluginInfo->Name, "z64gl Debug");
#else
        sprintf(PluginInfo->Name, "z64gl");
#endif

        // If DLL supports memory these memory options then set them to TRUE or FALSE
        //  if it does not support it
        PluginInfo->NormalMemory = TRUE;  // a normal BYTE array
        PluginInfo->MemoryBswaped = TRUE; // a normal BYTE array where the memory has been pre
    }

    static void glut_rdp_process_list()
    {
        rdp_process_list();
    }

    EXPORT void CALL RomClosed(void)
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
#endif
        }
        else
#endif
        {
            rglNextStatus = RGL_STATUS_CLOSED;
            rglUpdateStatus();
        }
    }

    EXPORT void CALL RomOpen(void)
    {
        no_dlists = true;
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

#ifdef __cplusplus
}
#endif
