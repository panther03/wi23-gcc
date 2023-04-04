/* Simulator for the wi23 processor
   Copyright (C) 2008-2023 Free Software Foundation, Inc.
   Contributed by Anthony Green

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This must come before any other includes.  */
#include "defs.h"

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>
#include "bfd.h"
#include "libiberty.h"
#include "sim/sim.h"
#include <stdio.h>
#include <math.h>

#include "sim-main.h"
#include "sim-base.h"
#include "sim-options.h"
#include "sim-io.h"
#include "sim-signal.h"
#include "target-newlib-syscall.h"

#include "wi23-sim.h"
#include "opcode/wi23.h"

/* Extract the signed 10-bit offset from a 16-bit branch
   instruction.  */
#define INST2OFFSET(o) ((((signed short)((o & ((1<<10)-1))<<6))>>6)<<1)

// because we are harvard architecture and have separate data/insn spaces
#define RAM_BIAS 0x800000 

// not sure where else to put this
#define WI23_STOP() sim_engine_halt (sd, scpu, NULL, pc, sim_stopped, SIM_SIGILL);

#define NUM_WI23_REGS 64
#define WI23_FIRST_FPR 32
#define REGNUM_RA 30

#define FP_OFFSET 32

/* wi23 register names.  */
static const char *reg_names[NUM_WI23_REGS] = { \
   /* general-purpose regs */ \
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \
    "r24", "r25", "r26", "gp", "fp", "sp", "ra", "csr", \
   /* floating-point regs */ \
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", \
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", \
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", \
    "f24", "f25", "f26", "f27", "f28", "f29", "f30", "fcsr"};

/* The machine state.

   This state is maintained in host byte order.  The fetch/store
   register functions must translate between host byte order and the
   target processor byte order.  Keeping this data in target byte
   order simplifies the register read/write functions.  Keeping this
   data in native order improves the performance of the simulator.
   Simulation speed is deemed more important.  */

/* The ordering of the moxie_regset structure is matched in the
   gdb/config/moxie/tm-moxie.h file in the REGISTER_NAMES macro.  */
// todo ^ does this matter if we don't care about gdb?
struct wi23_regset
{
  int32_t		 pc;
  int32_t	     regs[NUM_WI23_REGS]; /* primary registers */
  unsigned long long insts;                /* instruction counter */
};

union
{
  struct wi23_regset asregs;
  int32_t asints [1];		/* but accessed larger... */
} cpu;

static unsigned long
wi23_extract_unsigned_integer (const unsigned char *addr, int len)
{
  unsigned long retval;
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;
 
  if (len > (int) sizeof (unsigned long))
    printf ("That operation is not available on integers of more than %zu bytes.",
	    sizeof (unsigned long));
 
  /* Start at the most significant end of the integer, and work towards
     the least significant.  */
  retval = 0;

  for (p = endaddr; p > startaddr;)
    retval = (retval << 8) | * -- p;
  
  return retval;
}

static void
wi23_store_unsigned_integer (unsigned char *addr, int len, unsigned long val)
{
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;

  for (p = endaddr; p > startaddr;)
    {
      * -- p = val & 0xff;
      val >>= 8;
    }
}

static void
set_initial_gprs (void)
{
  int i;
  long space;
  
  /* Set up machine just out of reset.  */
  cpu.asregs.pc = 0;
  
  /* Clean out the register contents.  */
  for (i = 0; i < NUM_WI23_REGS; i++)
    cpu.asregs.regs[i] = 0;
}

/* Write a 4 byte value to data memory.  */

static INLINE void
wlat (sim_cpu *scpu, int32_t x, int32_t v)
{
  address_word cia = CPU_PC_GET (scpu);
	
  sim_core_write_aligned_4 (scpu, cia, write_map, RAM_BIAS + x, v);
}

/* Read 4 bytes from data memory.  */

static INLINE int
rlat (sim_cpu *scpu, int32_t x)
{
  address_word cia = CPU_PC_GET (scpu);
  
  return (sim_core_read_aligned_4 (scpu, cia, read_map, RAM_BIAS + x));
}

/* Read 4 bytes from insn memory.  */

static INLINE int
rlat_i (sim_cpu *scpu, int32_t x)
{
  address_word cia = CPU_PC_GET (scpu);
  
  return (sim_core_read_aligned_4 (scpu, cia, read_map, RAM_BIAS));
}

static int
wi23_reg_store (SIM_CPU *scpu, int rn, const void *memory, int length)
{
  if (rn <= NUM_WI23_REGS && rn >= 0)
    {
      if (length == 4)
	{
	  long ival;
	  
	  /* misalignment safe */
	  ival = wi23_extract_unsigned_integer (memory, 4);
	  cpu.asints[rn] = ival;
	}

      return 4;
    }
  else
    return 0;
}

static int
wi23_reg_fetch (SIM_CPU *scpu, int rn, void *memory, int length)
{
  if (rn <= NUM_WI23_REGS && rn >= 0)
    {
      if (length == 4)
	{
	  long ival = cpu.asints[rn];

	  /* misalignment-safe */
	  wi23_store_unsigned_integer (memory, 4, ival);
	}
      
      return 4;
    }
  else
    return 0;
}

static sim_cia
wi23_pc_get (sim_cpu *cpu)
{
  return WI23_SIM_CPU (cpu)->pc;
}

static void
wi23_pc_set (sim_cpu *cpu, sim_cia pc)
{
  WI23_SIM_CPU (cpu)->pc = pc;
}

// TODO: not entirely sure what this is for
/*static unsigned int
convert_target_flags (unsigned int tflags)
{
  unsigned int hflags = 0x0;

  CHECK_FLAG(0x0001, O_WRONLY);
  CHECK_FLAG(0x0002, O_RDWR);
  CHECK_FLAG(0x0008, O_APPEND);
  CHECK_FLAG(0x0200, O_CREAT);
  CHECK_FLAG(0x0400, O_TRUNC);
  CHECK_FLAG(0x0800, O_EXCL);
  CHECK_FLAG(0x2000, O_SYNC);

  if (tflags != 0x0)
    fprintf (stderr, 
	     "Simulator Error: problem converting target open flags for host.  0x%x\n", 
	     tflags);

  return hflags;
}*/

/* TODO: Split this up into finger trace levels than just insn.  */
// TODO: trace floating point regs too
#define WI23_TRACE_INSN(str) \
  TRACE_INSN (scpu, "0x%08x: %s\nInteger Registers\t\tr0:0x%x, r1:0x%x, r2:0x%x, r3:0x%x, r4:0x%x, r5:0x%x, r6:0x%x, r7:0x%x, r8:0x%x, r9:0x%x, r10:0x%x, r11:0x%x, r12:0x%x, r13:0x%x, r14:0x%x, r15:0x%x\nFloating Point Registers\tt0:0x%x, t1:0x%x, t2:0x%x, t3:0x%x, t4:0x%x, t5:0x%x, t6:0x%x, t7:0x%x, t8:0x%x, t9:0x%x, t10:0x%x, t11:0x%x, t12:0x%x, t13:0x%x, t14:0x%x, t15:0x%x\n", \
	      opc, str, cpu.asregs.regs[0], cpu.asregs.regs[1], \
	      cpu.asregs.regs[2], cpu.asregs.regs[3], cpu.asregs.regs[4], \
	      cpu.asregs.regs[5], cpu.asregs.regs[6], cpu.asregs.regs[7], \
	      cpu.asregs.regs[8], cpu.asregs.regs[9], cpu.asregs.regs[10], \
	      cpu.asregs.regs[11], cpu.asregs.regs[12], cpu.asregs.regs[13], \
	      cpu.asregs.regs[14], cpu.asregs.regs[15], cpu.asregs.regs[16], \
		  cpu.asregs.regs[17], cpu.asregs.regs[18], cpu.asregs.regs[19], \
		  cpu.asregs.regs[20], cpu.asregs.regs[21], cpu.asregs.regs[22], \
		  cpu.asregs.regs[23], cpu.asregs.regs[24], cpu.asregs.regs[25], \
		  cpu.asregs.regs[26], cpu.asregs.regs[27], cpu.asregs.regs[28], \
		  cpu.asregs.regs[29], cpu.asregs.regs[30], cpu.asregs.regs[31])


void WI23_trace_insn_simple(char * inst, int32_t pc, int modified_reg){

    printf("Instruction: %s\t\tPC: 0x%x\n", inst, pc);
    if(modified_reg < FP_OFFSET){
      printf("Modified Register: r%d=0x%x", modified_reg, cpu.asregs.regs[modified_reg]);
    }else {
      printf("Modified Register: f%d=%d", modified_reg-FP_OFFSET, cpu.asregs.regs[modified_reg]);
    }
    printf("\n\n");
    fflush(stdout);
}

void
sim_engine_run (SIM_DESC sd,
		int next_cpu_nr, /* ignore  */
		int nr_cpus, /* ignore  */
		int siggnal) /* ignore  */
{
  int32_t pc, opc;
  unsigned int inst;
  sim_cpu *scpu = STATE_CPU (sd, 0); /* FIXME <- nice fixme man bro didnt even put anything to fix */
  address_word cia = CPU_PC_GET (scpu);

  char * dmem = malloc(pow(2, 32));

  pc = cpu.asregs.pc;

  /* Run instructions here. */
  do 
    {

      const wi23_opc_info_t* opcode; 
      const wi23_fnc_info_t* fncode;

      opc = pc;

      /* Fetch the instruction at pc.  */
      inst = sim_core_read_aligned_4 (scpu, cia, read_map, pc);


      opcode = &wi23_opc_info[XT(inst, 26, 6)];

      //printf("isntruction: %x\n", inst);
      //fflush(stdout);

      // todo potential optimization if shit gets real slow: just switch directly on opcode? might be faster cause less branching
      switch (opcode->itype) {
        ///////////////
        // R-Format //
        //////////////
        case WI23_RF_F_FN_DST:{
          // TODO add support for floating point arithmetic
          unsigned dst_reg;
          float src, trg;
          
          dst_reg = OP_RD_R(inst);
          trg = cpu.asregs.regs[OP_RT_R(inst) + FP_OFFSET];
          src = cpu.asregs.regs[OP_RS(inst) + FP_OFFSET];
          
          fncode = &wi23_float_fnc[XT(inst, 0, 2)];
          if(opcode->opcode == 0x3B){
            switch (fncode->fncode) {
              case 0x00: {
                cpu.asregs.regs[dst_reg + FP_OFFSET] = src + trg;
                WI23_TRACE_INSN ("fadd -- UNIMPLEMENTED");
                break;
              }
              case 0x01: {
                cpu.asregs.regs[dst_reg + FP_OFFSET] = trg - src;
                WI23_TRACE_INSN ("fsub -- UNIMPLEMENTED");
                break;
              }
              case 0x02: {
                cpu.asregs.regs[dst_reg + FP_OFFSET] = trg * src;
                WI23_TRACE_INSN ("fmul -- UNIMPLEMENTED");
                break;
              }
              case 0x03: {
                cpu.asregs.regs[dst_reg + FP_OFFSET] = trg / src;
                WI23_TRACE_INSN ("fdiv -- UNIMPLEMENTED");
                break;
              }
            }
          }else{
            WI23_STOP(); // unsupported!
          }
          break;
        }
        case WI23_RF_I_FN_DST:{
          unsigned trg = cpu.asregs.regs[OP_RT_R(inst)];
          unsigned src = cpu.asregs.regs[OP_RS(inst)];
          unsigned dst_reg = OP_RD_R(inst);

          if (opcode->opcode == 0x1B) {
            fncode = &wi23_shift_fnc[XT(inst, 0, 2)];
            switch (fncode->fncode) {
              case 0x00: {
                cpu.asregs.regs[dst_reg] = src + trg;
                //WI23_TRACE_INSN ("add");
                WI23_trace_insn_simple("add", pc, dst_reg);

                break;
              }
              case 0x01: {
                cpu.asregs.regs[dst_reg] = src - trg;
                //WI23_TRACE_INSN ("sub");
                WI23_trace_insn_simple("sub", pc, dst_reg);
                break;
              }
              case 0x02: {
                cpu.asregs.regs[dst_reg] = src ^ trg;
                WI23_TRACE_INSN ("xor");
                break;
              }
              default: {
                cpu.asregs.regs[dst_reg] = src & ~trg;
                WI23_TRACE_INSN ("andn");
                break;
              }
            }
          } else if (opcode->opcode == 0x1A) {
            fncode = &wi23_arith_fnc[XT(inst, 0, 2)];
            switch (fncode->fncode) {
              case 0x00: {
                cpu.asregs.regs[dst_reg] = (src << trg) | (src >> (32 - trg));
                WI23_TRACE_INSN ("rol");
                break;
              }
              case 0x01: {
                cpu.asregs.regs[dst_reg] = src << trg;
                WI23_TRACE_INSN ("sll");
                break;
              }
              case 0x02: {
                cpu.asregs.regs[dst_reg] = (src >> trg) | (src << (32 - trg));
                WI23_TRACE_INSN ("ror");
                break;
              }
              default: {
                cpu.asregs.regs[dst_reg] = src >> trg;
                WI23_TRACE_INSN ("srl");
                break;
              }
            }
          } else if (opcode->opcode == 0x1F) {
            fncode = &wi23_arith_fnc[XT(inst, 0, 2)];
            switch (fncode->fncode) {
              case 0x00: {
                cpu.asregs.regs[dst_reg] = (src & trg);
                WI23_TRACE_INSN ("and");
                break;
              }
              case 0x01: {
                cpu.asregs.regs[dst_reg] = src | trg;
                WI23_TRACE_INSN ("or");
                break;
              }
            }
          }
          break;
        }
        case WI23_RF_I_DST: {
          unsigned trg = cpu.asregs.regs[OP_RT_R(inst)];
          unsigned src = cpu.asregs.regs[OP_RS(inst)];
          unsigned dst_reg = OP_RD_R(inst);

          fncode = &wi23_arith_fnc[XT(inst, 0, 2)];

          switch (opcode->opcode){
            case 0x1C: {
              cpu.asregs.regs[dst_reg] = (src == trg);
              WI23_TRACE_INSN ("seq");
              break;
            }
            case 0x1D: {
              if (fncode->fncode == 0x00){
                cpu.asregs.regs[dst_reg] = (src < trg);
                WI23_TRACE_INSN ("slt");
              } else {
                cpu.asregs.regs[dst_reg] = ((unsigned)src < (unsigned)trg);
                WI23_TRACE_INSN ("sltu");
              }

              break;
            }
            case 0x1E: {
              if (fncode->fncode == 0x00){
                cpu.asregs.regs[dst_reg] = (src <= trg);
                WI23_TRACE_INSN ("sle");
                break;
              } else {
                cpu.asregs.regs[dst_reg] = ((unsigned)src <= (unsigned)trg);
                WI23_TRACE_INSN ("sleu");
              }
            }
            case 0x3A: {
              cpu.asregs.regs[dst_reg] = ((int32_t)src >> trg);
              WI23_TRACE_INSN ("sra");
              break;
            }
            default: {
              WI23_STOP();
              break;
            }
          }

          break;

        }
        case WI23_RF_F_DST: {
          float trg = cpu.asregs.regs[OP_RT_R(inst) + FP_OFFSET];
          float src = cpu.asregs.regs[OP_RS(inst) + FP_OFFSET];
          unsigned dst_reg = OP_RD_R(inst);

          switch (opcode->opcode){
            case 0x3C: {
              WI23_TRACE_INSN ("feq -- UNIMPLEMENTED");
              break;
            }
            case 0x3E: {
              WI23_TRACE_INSN ("fle -- UNIMPLEMENTED");
              break;
            }
            case 0x3F: {
              WI23_TRACE_INSN ("flt -- UNIMPLEMENTED");
              break;
            }
            default: {
              WI23_STOP();
              break;
            }
          }

          break;
        }
        case WI23_RF_DS: {
          unsigned src = cpu.asregs.regs[OP_RS(inst)];
          unsigned dst_reg = OP_RD_R(inst);
          // only used for compare & set insns
          // WI23_RF_DS is used for floating point conversion and btr insns
          switch (opcode->opcode) {
            case 0x20: {
              float src = cpu.asregs.regs[OP_RS(inst) + FP_OFFSET];
              unsigned dst_reg = OP_RD_R(inst);
              
              WI23_TRACE_INSN ("icvtf -- UNIMPLEMENTED");
              break;
            }
            case 0x21: {
              unsigned src = cpu.asregs.regs[OP_RS(inst)];
              unsigned dst_reg = OP_RD_R(inst) + FP_OFFSET;

              //cpu.asregs.regs[dst_reg + FP_OFFSET] = (float)src;
              WI23_TRACE_INSN ("fcvti -- UNIMPLEMENTED");
              break;
            }
            case 0x22: {
              unsigned src = cpu.asregs.regs[OP_RS(inst) + FP_OFFSET];
              unsigned dst_reg = OP_RD_R(inst);
              WI23_TRACE_INSN ("imovf -- UNIMPLEMENTED");
              break;
            }
            case 0x23: {
              unsigned src = cpu.asregs.regs[OP_RS(inst)];
              unsigned dst_reg = OP_RD_R(inst) + FP_OFFSET;
              WI23_TRACE_INSN ("fmovi -- UNIMPLEMENTED");
              break;
            }
            case 0x24: {
              float src = cpu.asregs.regs[OP_RS(inst) + FP_OFFSET];
              unsigned dst_reg = OP_RD_R(inst);
              WI23_TRACE_INSN ("fclass -- UNIMPLEMENTED");
              break;
            }
            default : {
              WI23_STOP();
              break;
            }

          }
          break;
        }
        case WI23_IF_DSI_Z: {
          unsigned immz = IMM_ZX(inst);
          unsigned src = cpu.asregs.regs[OP_RS(inst)];
          unsigned dst_reg = OP_RD_I(inst);
          if (opcode->opcode == 0x0A) {
            cpu.asregs.regs[dst_reg] = src ^ immz;
            WI23_TRACE_INSN ("xori");
          } else  if (opcode->opcode == 0x0B){
            cpu.asregs.regs[dst_reg] = src & ~immz;
            WI23_TRACE_INSN ("andni");
          } else  if (opcode->opcode == 0x19){
            cpu.asregs.regs[dst_reg] = src & immz;
            WI23_TRACE_INSN ("andi");
          } else  if (opcode->opcode == 0x38){
            cpu.asregs.regs[dst_reg] = src | immz;
            WI23_TRACE_INSN ("ori");
          } 
          break;
        }
        case WI23_IF_DSI_S: {
          // TODO add the floading load/store suport here
          signed int imms = IMM_SX(inst);
          unsigned src = cpu.asregs.regs[OP_RS(inst)];
          unsigned dst_reg = OP_RD_I(inst);
          switch (opcode->opcode) {
            case 0x03: {
              cpu.asregs.regs[dst_reg] = rlat_i(scpu, src + imms);

              WI23_TRACE_INSN ("ldcr");
              break;
            }
            case 0x08: {
              cpu.asregs.regs[dst_reg] = src + imms;
              //WI23_TRACE_INSN ("addi");
              WI23_trace_insn_simple("addi", pc, dst_reg);

              break;
            }
            case 0x09: {
              cpu.asregs.regs[dst_reg] = src - imms;
              WI23_TRACE_INSN ("subi");
              break;
            }
            case 0x10: {
              unsigned dst = cpu.asregs.regs[dst_reg];
              //wlat(scpu, src + imms, dst);
              int32_t * addr = (int32_t *)(dmem + src + imms);
              *addr = dst;

              WI23_TRACE_INSN ("st");
              break;
            }
            case 0x11: {
              //cpu.asregs.regs[dst_reg] = rlat(scpu, src + imms);
              int32_t * addr = (int32_t *)(dmem + src + imms);
              cpu.asregs.regs[dst_reg] = *addr;

              WI23_TRACE_INSN ("ld");
              break;
            }
            case 0x13: {
              unsigned dst = cpu.asregs.regs[dst_reg];
              //wlat(scpu, src + imms, dst);
              int32_t * addr = (int32_t *)(dmem + src + imms);
              *addr = dst;

              cpu.asregs.regs[dst_reg] = src + imms;

              WI23_TRACE_INSN ("stu");
              break;
            }
            case 0x30: {
              // Convert the destination regsiter to a floating point register
              dst_reg += FP_OFFSET;

              WI23_TRACE_INSN ("fst -- UNIMPLEMENTED");
              break;
            }
            case 0x31: {
                // Convert the destination regsiter to a floating point register
              dst_reg += FP_OFFSET;
              
              WI23_TRACE_INSN ("fld -- UNIMPLEMENTED");
              break;
            }
            case 0x32: {
              unsigned char * addr = (unsigned char *)(dmem + src + imms);
              
              *addr = (unsigned char)cpu.asregs.regs[dst_reg];

              WI23_TRACE_INSN ("stb");
              break;
            }
            case 0x33: {
              unsigned char * addr = (unsigned char *)(dmem + src + imms);

              cpu.asregs.regs[dst_reg] = (unsigned char)(*addr);

              WI23_TRACE_INSN ("ldb");
              break;
            }
            case 0x34: {
              unsigned char * addr = (unsigned char *)(dmem + src + imms);
              unsigned char * src_addr = (unsigned char*)(cpu.asregs.regs + dst_reg);

              *addr = *src_addr;
              addr++;
              src_addr++;
              *addr = *src_addr;
              
              WI23_TRACE_INSN ("sth");
              break;
            }
            case 0x35: {
              unsigned char * addr = (unsigned char *)(dmem + src + imms);
              unsigned char * src_addr = (unsigned char*)(cpu.asregs.regs + dst_reg);

              *src_addr = *addr;
              addr++;
              src_addr++;
              *src_addr = *addr;
              
              
              WI23_TRACE_INSN ("ldh");
              break;
            }
            case 0x2A: {
              cpu.asregs.regs[dst_reg] = src ^ imms;

              WI23_TRACE_INSN ("xorsi");
              break;
            }
            
            
          }
          break;
        }
        case WI23_IF_DSI_5:{
          unsigned int imms = IMM_SX(inst);
          unsigned src = cpu.asregs.regs[OP_RS(inst)];
          unsigned dst_reg = OP_RD_I(inst);

          // Get the 5 leeast significant bits
          imms = imms & 0x1F;


          switch (opcode->opcode) {
            case 0x14: {
              cpu.asregs.regs[dst_reg] = (src << imms) | (src >> (32 - imms));
              WI23_TRACE_INSN ("roli");
              break;
            }
            case 0x15: {
              cpu.asregs.regs[dst_reg] = src << imms;
              WI23_TRACE_INSN ("slli");
              break;
            }
            case 0x16: {
              cpu.asregs.regs[dst_reg] = (src >> imms) | (src << (32 - imms));
              WI23_TRACE_INSN ("rori");
              break;
            }
            case 0x17: {
              cpu.asregs.regs[dst_reg] = src >> imms;
              WI23_TRACE_INSN ("srli");
              break;
            }
            case 0x37: {
              cpu.asregs.regs[dst_reg] = ((int32_t)src >> imms);
              WI23_TRACE_INSN ("srai");
              break;
            }
            default: {
              WI23_STOP();
              break;
            }
          }
          break;
      }
        case WI23_IF_SI_Z: {
          unsigned int immz = IMM_ZX(inst);
          unsigned src_reg = OP_RS(inst);
          // only one instruction for this - slbi
          cpu.asregs.regs[src_reg] = (cpu.asregs.regs[src_reg] << 16) | immz;
          WI23_TRACE_INSN ("slbi");
          break;
        }
        case WI23_IF_SI_S: {
          signed int imms = IMM_SX(inst);
          unsigned src_reg = OP_RS(inst);
          switch (opcode->opcode) {
            case 0x05: {
              pc = cpu.asregs.regs[src_reg] + imms;
              WI23_TRACE_INSN("jr");
              break;
            }
            case 0x07: {
              cpu.asregs.regs[REGNUM_RA] = pc + 4;
              pc = cpu.asregs.regs[src_reg] + imms;
              WI23_TRACE_INSN("jalr");
              break;
            }
            case 0x18: {
              cpu.asregs.regs[src_reg] = imms;
              WI23_TRACE_INSN ("lbi");
              break;
            }
          }
          break;
        }
        case WI23_IF_SI_PC: {
          signed int imms = IMM_SX(inst);
          unsigned src  = cpu.asregs.regs[OP_RS(inst)];
          switch (opcode->opcode) {
            case 0x0C: {
              if (src == 0)
                pc += imms;
              WI23_TRACE_INSN("beqz");
              break;
            }
            case 0x0D: {
              if (src != 0)
                pc += imms;
              WI23_TRACE_INSN("bnez");
              break;
            }
            case 0x0E: {
              if (src < 0)
                pc += imms;
              WI23_TRACE_INSN("bltz");
              break;
            }
            default:  { // 0x0F
              if (src >= 0)
                pc += imms;
              WI23_TRACE_INSN("bgez");
              break;
            }
          }
          break;
        }
        case WI23_JF_D26: {
          signed int disp = D26(inst);
          switch (opcode->opcode) {
            case 0x04: {
              pc += disp;
              WI23_TRACE_INSN("j");
              break;
            }
            default: {
              cpu.asregs.regs[REGNUM_RA] = pc + 4;
              pc += disp;
              WI23_TRACE_INSN("jal");
              break;
            }
          }
          break;
        }
        case WI23_OTHER:
          if(opcode->opcode == 0x01){
              WI23_TRACE_INSN("nop");
          }else {
            sim_engine_halt (sd, scpu, NULL, pc, sim_stopped, SIM_SIGTRAP);
          }
          break;
        default:
          WI23_STOP();
          break;   
      }
      cpu.asregs.insts++;
      pc += 4;
      cpu.asregs.pc = pc;

      if (sim_events_tick (sd))
	      sim_events_process (sd);


      // Wait for a key to be pressed to proceed to the next instruction
      printf("\nPress any key to increment to the next instruction\n");
      getchar();

    } while (1);
}

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

SIM_DESC
sim_open (SIM_OPEN_KIND kind, host_callback *cb,
	  struct bfd *abfd, char * const *argv)
{
  int i;
  SIM_DESC sd = sim_state_alloc (kind, cb);
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  /* Set default options before parsing user options.  */
  current_target_byte_order = BFD_ENDIAN_BIG;

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all_extra (sd, 0, sizeof (struct wi23_sim_cpu))
      != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* The parser will print an error message for us, so we silently return.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  sim_do_command (sd, "memory region 0x00000000,0x40000");
  sim_do_command (sd, "memory region 0x800000,0x10000");

  /* Check for/establish the a reference program image.  */
  if (sim_analyze_program (sd, STATE_PROG_FILE (sd), abfd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Configure/verify the target byte order and other runtime
     configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      /* Uninstall the modules to avoid memory leaks,
	 file descriptor leaks, etc.  */
      sim_module_uninstall (sd);
      return 0;
    }

  /* CPU specific initialization.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      SIM_CPU *cpu = STATE_CPU (sd, i);

      CPU_REG_FETCH (cpu) = wi23_reg_fetch;
      CPU_REG_STORE (cpu) = wi23_reg_store;
      CPU_PC_FETCH (cpu) = wi23_pc_get;
      CPU_PC_STORE (cpu) = wi23_pc_set;

      set_initial_gprs ();	/* Reset the GPR registers.  */
    }

  return sd;
}

SIM_RC
sim_create_inferior (SIM_DESC sd, struct bfd *prog_bfd,
		     char * const *argv, char * const *env)
{
  char * const *avp;
  int l, argc, i, tp;
  sim_cpu *scpu = STATE_CPU (sd, 0); /* FIXME */

  if (prog_bfd != NULL)
    cpu.asregs.pc = bfd_get_start_address (prog_bfd);

  return SIM_RC_OK;
}
