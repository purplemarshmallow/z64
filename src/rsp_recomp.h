#include "rsp_opinfo.h"

struct rsp_bc_t {
  UINT32        op;   // original opcode
  short         op2;  // simplified opcode
  short         flags;
  rsp_regmask_t need;
};
