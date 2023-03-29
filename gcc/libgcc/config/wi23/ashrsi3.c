#include "tconfig.h"
#include "tsystem.h"
#include "coretypes.h"
#include "tm.h"
#include "libgcc_tm.h"

#include "libgcc2.h"

extern SItype __ashrsi3 (SItype, SItype);

SItype
__ashrsi3 (SItype a, SItype b)
{
  
  return (a >> b) | (-(a < 0) << (32 - b));
}