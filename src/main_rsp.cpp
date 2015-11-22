#include "Rsp_#1.1.h"
#include "rsp.h"
#include "z64.h"

__declspec(dllexport) void CloseDLL (void)
{
}

__declspec(dllexport) void DllAbout ( HWND hParent )
{
}

__declspec(dllexport) void DllConfig ( HWND hParent )
{
}

__declspec(dllexport) void DllTest ( HWND hParent )
{
}

#include <assert.h>
static void dump()
{
  FILE * fp = fopen("rsp.dump", "w");
  assert(fp);
  fwrite(rdram, 8*1024, 1024, fp);
  fwrite(rsp_dmem, 0x2000, 1, fp);
  fwrite(rsp.ext.MI_INTR_REG, 4, 1, fp);

  fwrite(rsp.ext.SP_MEM_ADDR_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_DRAM_ADDR_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_RD_LEN_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_WR_LEN_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_STATUS_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_DMA_FULL_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_DMA_BUSY_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_PC_REG, 4, 1, fp);
  fwrite(rsp.ext.SP_SEMAPHORE_REG, 4, 1, fp);

  fwrite(rsp.ext.DPC_START_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_END_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_CURRENT_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_STATUS_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_CLOCK_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_BUFBUSY_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_PIPEBUSY_REG, 4, 1, fp);
  fwrite(rsp.ext.DPC_TMEM_REG, 4, 1, fp);
  fclose(fp);
}

__declspec(dllexport) DWORD DoRspCycles ( DWORD Cycles )
{
	DWORD TaskType = *(DWORD*)(z64_rspinfo.DMEM + 0xFC0);

#if 0
  if (TaskType == 1) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case 'd':
              printf("Dumping !\n");
              dump();
              break;
          }
          break;
      }
    }
  }
#endif
	
// 	if (TaskType == 1) {
// 		if (z64_rspinfo.ProcessDList != NULL) {
// 			z64_rspinfo.ProcessDList();
// 		}
// 		*z64_rspinfo.SP_STATUS_REG |= (0x0203 );
// 		if ((*z64_rspinfo.SP_STATUS_REG & SP_STATUS_INTR_BREAK) != 0 ) {
// 			*z64_rspinfo.MI_INTR_REG |= R4300i_SP_Intr;
// 			z64_rspinfo.CheckInterrupts();
// 		}

// 		*z64_rspinfo.DPC_STATUS_REG &= ~0x0002;
// 		return Cycles;
//   }

  if (TaskType == 2) {
		if (z64_rspinfo.ProcessAList != NULL) {
			z64_rspinfo.ProcessAList();
		}
		*z64_rspinfo.SP_STATUS_REG |= (0x0203 );
		if ((*z64_rspinfo.SP_STATUS_REG & SP_STATUS_INTR_BREAK) != 0 ) {
			*z64_rspinfo.MI_INTR_REG |= R4300i_SP_Intr;
			z64_rspinfo.CheckInterrupts();
		}
		return Cycles;
	}  


  rsp_execute(0x10000000);
  
  return Cycles;
}

__declspec(dllexport) void GetDllInfo ( PLUGIN_INFO * PluginInfo )
{
	PluginInfo->Version = 0x0101;
	PluginInfo->Type = PLUGIN_TYPE_RSP;
	sprintf(PluginInfo->Name,"z64 RSP emulation Plugin");
	PluginInfo->NormalMemory = FALSE;
	PluginInfo->MemoryBswaped = TRUE;
}

// __declspec(dllexport) void GetRspDebugInfo ( RSPDEBUG_INFO * RSPDebugInfo )
// {
// }

__declspec(dllexport) void InitiateRSP ( RSP_INFO Rsp_Info, DWORD * CycleCount)
{
  printf("INITIATE RSP\n");
  rsp_init(Rsp_Info);
  memset(rsp_dmem, 0, 0x2000);
  *CycleCount = 0;
}

__declspec(dllexport) void InitiateRSPDebugger ( DEBUG_INFO DebugInfo)
{
}

__declspec(dllexport) void RomClosed (void)
{
  extern int rsp_gen_cache_hit;
  extern int rsp_gen_cache_miss;
  printf("cache hit %d miss %d %g%%\n", rsp_gen_cache_hit, rsp_gen_cache_miss,
  rsp_gen_cache_miss*100.0f/rsp_gen_cache_hit);
  rsp_gen_cache_hit = rsp_gen_cache_miss = 0;

  rsp_init(z64_rspinfo);
}
