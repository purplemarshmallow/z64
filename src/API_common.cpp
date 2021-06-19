#include "rdp.h"
#include "rgl.h"

#ifdef MUPEN64PLUS
#include "m64p_common.h"
#include "m64p_config.h"
#include "m64p_plugin.h"
#include "m64p_types.h"
#include "m64p_vidext.h"
#include "osal_dynamiclib.h"
#else
#include "Gfx #1.3.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    EXPORT int CALL InitiateGFX(GFX_INFO Gfx_Info)
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

    EXPORT void CALL ProcessRDPList(void)
    {
#ifdef RGL_USE_GLUT
        rglGlutPostCommand(glut_rdp_process_list);
#else
        if (rglSettings.softgfx) {
            rdp_process_list();
        }
#ifdef THREADED
        else if (rglSettings.threaded) {
            rdpCreateThread();
            rdpPostCommand();
        }
        else
#endif
        {
            rgl_process_list();
        }
#endif
        return;
    }

    EXPORT void CALL ShowCFB(void)
    {
        no_dlists = true;
    }

    EXPORT void CALL UpdateScreen(void)
    {
#ifdef THREADED
        if (rglSettings.threaded) {
            rdpPostCommand();
        }
        else
#endif
        {
            rglUpdateStatus();
            rglUpdate();
        }
    }

    EXPORT void CALL ViStatusChanged(void)
    {
    }

    EXPORT void CALL ViWidthChanged(void)
    {
    }

    EXPORT void CALL MoveScreen(int xpos, int ypos)
    {
    }

    EXPORT void CALL ProcessDList(void)
    {
    }

#ifdef __cplusplus
}
#endif