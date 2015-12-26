#include "z642.h"
#include "Gfx #1.3.h"

extern const int screen_width = 1024, screen_height = 768;

HRESULT res;
RECT dst, src;
INT32 pitchindwords;

FILE* zeldainfo = 0;
int ProcessDListShown = 0;
extern int SaveLoaded;
extern UINT32 command_counter;

extern GFX_INFO gfx;

int rdp_init();
int rdp_close();
int rdp_update();
void rdp_process_list(void);
extern INLINE void popmessage(const char* err, ...);
extern INLINE void fatalerror(const char* err, ...);
