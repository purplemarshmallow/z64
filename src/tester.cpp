#ifndef WIN32
#include "rsp.h"
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <zlib.h>

char dumpname[256] = "rsp.dump.gz";
int emutype = 0;

BYTE _rdram[8*1024*1024];
BYTE _rsp_dmem[0x2000];

DWORD MI_INTR_REG;

DWORD SP_MEM_ADDR_REG;
DWORD SP_DRAM_ADDR_REG;
DWORD SP_RD_LEN_REG;
DWORD SP_WR_LEN_REG;
DWORD SP_STATUS_REG;
DWORD SP_DMA_FULL_REG;
DWORD SP_DMA_BUSY_REG;
DWORD SP_PC_REG;
DWORD SP_SEMAPHORE_REG;

DWORD DPC_START_REG;
DWORD DPC_END_REG;
DWORD DPC_CURRENT_REG;
DWORD DPC_STATUS_REG;
DWORD DPC_CLOCK_REG;
DWORD DPC_BUFBUSY_REG;
DWORD DPC_PIPEBUSY_REG;
DWORD DPC_TMEM_REG;

void checkinterrupts( void );
void processdlist( void );
void processalist( void );
void processrdplist( void );
void showcfb( void );

#define rspinfo tester_rspinfo
RSP_INFO rspinfo = {
  0, // 	HINSTANCE hInst;
  0, // 	BOOL MemoryBswaped;    /* If this is set to TRUE, then the memory has been pre
  // 	                          bswap on a dword (32 bits) boundry */
  _rdram, // 	BYTE * RDRAM;
  _rsp_dmem, // 	BYTE * DMEM;
  _rsp_dmem+0x1000, // 	BYTE * IMEM;

  &MI_INTR_REG,

  &SP_MEM_ADDR_REG,
  &SP_DRAM_ADDR_REG,
  &SP_RD_LEN_REG,
  &SP_WR_LEN_REG,
  &SP_STATUS_REG,
  &SP_DMA_FULL_REG,
  &SP_DMA_BUSY_REG,
  &SP_PC_REG,
  &SP_SEMAPHORE_REG,

  &DPC_START_REG,
  &DPC_END_REG,
  &DPC_CURRENT_REG,
  &DPC_STATUS_REG,
  &DPC_CLOCK_REG,
  &DPC_BUFBUSY_REG,
  &DPC_PIPEBUSY_REG,
  &DPC_TMEM_REG,

  checkinterrupts,
  processdlist,
  processalist,
  processrdplist,
  showcfb,
};

DWORD (*dorspcycles) ( DWORD Cycles );
void (*initiatersp) ( RSP_INFO Rsp_Info, DWORD * CycleCount);

#ifdef rsp
#undef rsp
/* fixes an unrelated macro re-definition over the one in "rsp.h" */
#endif
#define rsp tester_rsp
RSP_REGS rsp __attribute__((aligned(16)));

void checkinterrupts( void ) {}
void processdlist( void ) {}
void processalist( void ) {}
void processrdplist( void ) {
  *rsp.ext.DPC_CURRENT_REG = *rsp.ext.DPC_END_REG;
  //printf("ProcessRdpList\n");
}
void showcfb( void ) {}

void loadstate(const char * name)
{
  gzFile fp = gzopen(name, "rb");
  assert(fp);
  gzread(fp, rdram, 8*1024*1024);
  gzread(fp, rsp_dmem, 0x2000);
  gzread(fp, rspinfo.MI_INTR_REG, 4);

  gzread(fp, rspinfo.SP_MEM_ADDR_REG, 4);
  gzread(fp, rspinfo.SP_DRAM_ADDR_REG, 4);
  gzread(fp, rspinfo.SP_RD_LEN_REG, 4);
  gzread(fp, rspinfo.SP_WR_LEN_REG, 4);
  gzread(fp, rspinfo.SP_STATUS_REG, 4);
  gzread(fp, rspinfo.SP_DMA_FULL_REG, 4);
  gzread(fp, rspinfo.SP_DMA_BUSY_REG, 4);
  gzread(fp, rspinfo.SP_PC_REG, 4);
  gzread(fp, rspinfo.SP_SEMAPHORE_REG, 4);

  gzread(fp, rspinfo.DPC_START_REG, 4);
  gzread(fp, rspinfo.DPC_END_REG, 4);
  gzread(fp, rspinfo.DPC_CURRENT_REG, 4);
  gzread(fp, rspinfo.DPC_STATUS_REG, 4);
  gzread(fp, rspinfo.DPC_CLOCK_REG, 4);
  gzread(fp, rspinfo.DPC_BUFBUSY_REG, 4);
  gzread(fp, rspinfo.DPC_PIPEBUSY_REG, 4);
  gzread(fp, rspinfo.DPC_TMEM_REG, 4);
  
  gzclose(fp);
}

void measure(int mode)
{
  uint64_t total = 0;
  int i;
  for (i=0; i<10; i++) {
    loadstate(dumpname);
    if (mode)
      memcpy(((char *)rdram) +8*1024*1024-5, "ziggy", 5);
    uint64_t start, stop;
    start = RDTSC();
    dorspcycles(10000);
    stop = RDTSC();
    total += stop - start;
  }

  printf("%g\n", total/1e7);
}

uint64_t * timings;
int * counts;

int compar(const void * pa, const void * pb)
{
  int64_t diff = timings[*(int *)pa] - timings[*(int *)pb];
  if (diff < 0) return -1;
  else if (diff > 0) return +1;
  return 0;
}
int compar2(const void * pa, const void * pb)
{
  //int64_t diff = counts[*(int *)pa] - counts[*(int *)pb];
  int64_t diff = timings[*(int *)pa]/(counts[*(int *)pa]+1) - timings[*(int *)pb]/(counts[*(int *)pb]+1);
  if (diff < 0) return -1;
  else if (diff > 0) return +1;
  return 0;
}

int main(int argc, char * * argv)
{
  if (argc > 1)
    strcpy(dumpname, argv[1]);
  if (argc > 2)
    emutype = atoi(argv[2]);
  printf("testing '%s' emutype %d\n", dumpname, emutype);

  void * h;
  if (emutype < 2)
    h = dlopen("./plugins/z64-rsp.so", RTLD_NOW | RTLD_GLOBAL);
  else
    h = dlopen("./plugins/RSPcomp-pj64.so", RTLD_NOW | RTLD_GLOBAL);
  if (!h) printf(dlerror());
  assert(h);
  dorspcycles = (DWORD (*) ( DWORD Cycles )) dlsym(h, "DoRspCycles");
  initiatersp = (void (*) ( RSP_INFO Rsp_Info, DWORD * CycleCount))dlsym(h, "InitiateRSP");
  printf("%x %x\n", dorspcycles, initiatersp);

  rsp.ext = rspinfo;

  DWORD useless;
  initiatersp(rspinfo, &useless);

  loadstate(dumpname);
  
  if (emutype)
    memcpy(((char *)rdram) +8*1024*1024-5, "ziggy", 5);
  dorspcycles(10000);

  if (argc > 2)
    return 0;

  measure(0);
  measure(1);

#ifdef RSPTIMING
  timings = (uint64_t *) dlsym(h, "rsptimings");
  counts = (int *) dlsym(h, "rspcounts");
  int sorted[512];
  int sorted2[512];
  int i;
  for (i=0; i<0x140; i++) {
    sorted[i] = i;
    sorted2[i] = i;
  }
  qsort(sorted, 0x140, sizeof(int), compar);
  qsort(sorted2, 0x140, sizeof(int), compar2);
  int j;
  for (j=0; j<0x140; j++) {
    int k = sorted2[j];
    i = sorted[j];
    UINT32 op, op2;
    if (i>=0x100)
      op = (0x12<<26) | (0x10 << 21) | (i&0x3f);
    else if (i>=0xc0)
      op = (0x3a<<26) | ((i&0x1f)<<11);
    else if (i>=0xa0)
      op = (0x32<<26) | ((i&0x1f)<<11);
    else if (i>=0x80)
      op = (0x12<<26) | ((i&0x1f)<<21);
    else if (i>=0x40)
      op = (0<<26) | (i&0x3f);
    else
      op = (i&0x3f)<<26;
    if (k>=0x100)
      op2 = (0x12<<26) | (0x10 << 21) | (k&0x3f);
    else if (k>=0xc0)
      op2 = (0x3a<<26) | ((k&0x1f)<<11);
    else if (k>=0xa0)
      op2 = (0x32<<26) | ((k&0x1f)<<11);
    else if (k>=0x80)
      op2 = (0x12<<26) | ((k&0x1f)<<21);
    else if (k>=0x40)
      op2 = (0<<26) | (k&0x3f);
    else
      op2 = (k&0x3f)<<26;
    char s[128], s2[128];
    rsp_dasm_one(s, 0x800, op);
    rsp_dasm_one(s2, 0x800, op2);
    if (timings[i])
      printf("%10g %7d\t%30s\t"
             "%10g %7d\t%30s\n",
             timings[i]/1e6, counts[i], s,
             timings[k]/1.0f/counts[k], counts[k], s2
      );
  }
#endif
}
#else // WIN32
int main(int argc, char * * argv)
{
  return 0;
}
#endif
