//==========================================================================
//
//      monitor.h
//
//      Main definitions for the CygMON ROM monitor
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    
// Contributors: gthomas
// Date:         1999-10-20
// Purpose:      Main definitions for the CygMON ROM monitor
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#ifndef MONITOR_H
#define MONITOR_H

#if !defined(__ASSEMBLER__)
#include <stdarg.h>
#endif

#include <board.h>
#ifdef HAVE_BSP
#include "cpu_info.h"
#endif
#include <monitor_cmd.h>

#ifndef ASM

#ifdef HAVE_BSP
#define xprintf    bsp_dprintf
#define xsprintf   bsp_sprintf
#define xvprintf   bsp_vprintf
#define xputchar   bsp_debug_putc
#define xgetchar   bsp_debug_getc
#define xungetchar bsp_debug_ungetc
#define __getTty() bsp_set_debug_comm(-1)
#define set_pc(x)  bsp_set_pc((x), mon_saved_regs)
#define __single_step() bsp_singlestep_setup(mon_saved_regs)
#define __reset    bsp_reset
#define breakpoint() bsp_breakpoint()
#else
extern void xprintf(const char *fmt, ...);
extern void xsprintf(char *str, const char *fmt, ...);
extern void xvprintf(const char *fmt, va_list ap);

#ifdef HAS_USER_IO
#define xputchar   putUserChar
#define xgetchar   getUserChar
#else
extern void putDebugChar(int ch);
extern int  getDebugChar(void);
#define xputchar   putDebugChar
#define xgetchar   getDebugChar
#endif
#define xungetchar ungetDebugChar
#endif


struct bp {
  mem_addr_t address;
  bp_inst_t  old_inst;
  char       in_memory;
  struct bp  *next;
};

struct regstruct
{
  char *registername;
  int  registernumber;
#ifdef HAVE_BSP
  int  registertype;
#endif
};



#ifdef HAVE_BSP
#define REGTYPE_INT    1
#define REGTYPE_FLOAT  2
#define REGTYPE_DOUBLE 3

union target_reg
{
    unsigned long  i;	/* integer register (32/64 bit) */
#if HAVE_FLOAT_REGS
    float          f;	/* float register (32bit)       */
#endif
#if HAVE_DOUBLE_REGS
    double         d;	/* double register (64bit)      */
#endif
};

typedef union target_reg target_regval_t;


/* This is a template for what should be defined in  the board specific
   header file composed, board.h */
#if ! defined(MEM_ADDR_DEFINED)
#define MEM_ADDR_DEFINED 1
typedef struct mem_addr {
  unsigned long addr;
} mem_addr_t;
#endif

#if !defined(BP_INST_T_DEFINED)
#define BP_INST_T_DEFINED 1
typedef unsigned char bp_inst_t ;
#endif

#if ! defined(MAKE_STD_ADDR)
#define MAKE_STD_ADDR(SRC, DST) ((DST)->addr = (SRC))
#endif


#if ! defined(ADD_OFFSET)
#define ADD_OFFSET(SRC,DST,OFFSET) ((DST)->addr = (SRC)->addr + (OFFSET))
#endif

#if ! defined(ADD_ALIGN)
#define ADD_ALIGN(SRC,DST,ALIGN) \
              ((DST)->addr = (SRC)->addr - ((SRC)->addr % (ALIGN)))
#endif

#if ! defined(MEM_ADDR_EQ_P)
#define MEM_ADDR_EQ_P(A, B) ((A).addr == (B).addr)     
#endif

#if ! defined(MEM_ADDR_DIFF)
#define MEM_ADDR_DIFF(A, B) ((A).addr - (B).addr)
#endif

#if ! defined(MEM_ADDR_ASI)
#define MEM_ADDR_ASI(A) -1
#endif

#endif  /* HAVE_BSP */

#if defined(NO_MALLOC) && ! defined(MAX_NUM_BP)
#define MAX_NUM_BP 64
#endif

extern struct regstruct regtab[];

extern char **argvect;

#ifdef HAVE_BSP
#define VERSION "release 2.0"
#else
#define VERSION "release 1.2"
#endif

#define MAXLINELEN 80
#define PROMPT "cygmon> "
#if ! defined MAX_HIST_ENTS
#define MAX_HIST_ENTS 10
#endif

/*
 * From monitor.c
 */
extern mem_addr_t last_pc;
extern int        stub_is_active;
#ifdef HAVE_BSP
extern void *mon_saved_regs;
#else
extern int (*user_signal_handler)(int);
#endif

extern void clear_user_state (void);
extern int  transfer_to_stub (void);
extern void version (void);
#ifdef MONITOR_CONTROL_INTERRUPTS
/* Enable interrupts within the monitor. */
extern void monitor_enable_interrupts (void);

/* Disable interrupts within the monitor. */
extern void monitor_disable_interrupts (void);

/* Returns 1 if interrupts have been enabled within the monitor, 0
   otherwise. */
extern int monitor_interrupt_state (void);
#endif /* MONITOR_CONTROL_INTERRUPTS */


/*
 * From utils.c
 */
extern int switch_to_stub_flag;
extern int input_char (void);
extern target_register_t str2int (char *str, int base);
#if HAVE_DOUBLE_REGS
extern double str2double (char *str, int base);
#endif
extern target_register_t str2intlen (char *str, int base, int len);
extern int hex2bytes(char *string, char *dest, int maxsize);
extern char *int2str (target_register_t number, int base, int numdigs);
#ifndef NO_MALLOC
extern char *strdup(const char *str);
#endif
extern target_register_t get_pc(void);
extern char *get_register_str (regnames_t which, int detail);
extern void store_register (regnames_t which, char *string);

/*
 * From breakpoints.c
 */
extern int add_mon_breakpoint (mem_addr_t location);
extern void install_breakpoints (void);
extern void clear_breakpoints (void);
extern int  show_breakpoints (void);
extern int clear_mon_breakpoint (mem_addr_t location);


/*
 * From do-dis.c
 */
extern mem_addr_t do_dis (mem_addr_t *addr);
extern void flush_dis (void);


/*
 * From architecture-mon.c
 */
#ifdef HAVE_BSP
extern void initialize_mon(void);
extern target_regval_t get_register(int regnum);
extern void put_register(int regnum, target_regval_t val);
#endif


/* Lame. */
#ifndef ITIMER_REAL
#define ITIMER_REAL 0
#endif

#endif /* ASM */
#endif
