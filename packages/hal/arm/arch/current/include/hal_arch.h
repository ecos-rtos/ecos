#ifndef CYGONCE_HAL_ARCH_H
#define CYGONCE_HAL_ARCH_H

//==========================================================================
//
//      hal_arch.h
//
//      Architecture specific abstractions
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg, gthomas
// Contributors: nickg, gthomas
// Date:         1999-02-20
// Purpose:      Define architecture abstractions
// Usage:        #include <cyg/hal/hal_arch.h>

//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>         // To decide on stack usage
#include <cyg/infra/cyg_type.h>

//
// CPSR Register defines
//

#define CPSR_IRQ_DISABLE	0x80	// IRQ disabled when =1
#define CPSR_FIQ_DISABLE	0x40	// FIQ disabled when =1
#define CPSR_FIQ_MODE		0x11
#define CPSR_IRQ_MODE		0x12
#define CPSR_SUPERVISOR_MODE	0x13
#define CPSR_UNDEF_MODE		0x1B

#define CPSR_MODE_BITS          0x1F

#define CPSR_INITIAL (CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_SUPERVISOR_MODE)
#define CPSR_THREAD_INITIAL (CPSR_SUPERVISOR_MODE)

//--------------------------------------------------------------------------
// Processor saved states:
// The layout of this structure is also defined in "arm.inc", for assembly
// code, which will be generated automatically if this file changes.

#define HAL_THREAD_CONTEXT_FIRST        0
#define HAL_THREAD_CONTEXT_R0           (0-HAL_THREAD_CONTEXT_FIRST)
#define HAL_THREAD_CONTEXT_R4           (4-HAL_THREAD_CONTEXT_FIRST)
#define HAL_THREAD_CONTEXT_R10          (10-HAL_THREAD_CONTEXT_FIRST)
#define HAL_THREAD_CONTEXT_LAST         10
#define HAL_NUM_THREAD_CONTEXT_REGS     (HAL_THREAD_CONTEXT_LAST - \
                                          HAL_THREAD_CONTEXT_FIRST+1)

// It seems that r0-r3,r12 are considered scratch by function calls

typedef struct 
{
    // These are common to all saved states
    cyg_uint32  d[HAL_NUM_THREAD_CONTEXT_REGS] ;  // Data regs (r0..r10)
    cyg_uint32  fp;                               // (r11) Frame pointer
    cyg_uint32  sp;                               // (r13) Stack pointer
    cyg_uint32  lr;                               // (r14) Link Reg
    cyg_uint32  pc;                               // (r15) PC place holder
                                                  //       (never used)
    cyg_uint32  cpsr;                             // Condition Reg
    cyg_uint32  ip;                               // (r12) Previous stack
                                                  //       pointer
    // These are only saved for exceptions and interrupts
    cyg_uint32  vector;                           // Vector number
    cyg_uint32  msr;                              // Machine State Reg

} HAL_SavedRegisters;

//-------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-------------------------------------------------------------------------
// Bit manipulation macros

externC int hal_lsbindex(int);
externC int hal_msbindex(int);

#define HAL_LSBIT_INDEX(index, mask) index = hal_lsbindex(mask)
#define HAL_MSBIT_INDEX(index, mask) index = hal_msbindex(mask)

//-------------------------------------------------------------------------
// Context Initialization
// Initialize the context of a thread.
// Arguments:
// _sparg_ name of variable containing current sp, will be changed to new sp
// _thread_ thread object address, passed as argument to entry point
// _entry_ entry point address.
// _id_ bit pattern used in initializing registers, for debugging.

#define HAL_THREAD_INIT_CONTEXT( _sparg_, _thread_, _entry_, _id_ )         \
    CYG_MACRO_START                                                         \
    register CYG_WORD _sp_ = ((CYG_WORD)_sparg_) &~15;                      \
    register HAL_SavedRegisters *_regs_;                                    \
    int _i_;                                                                \
    _regs_ = (HAL_SavedRegisters *)((_sp_) - sizeof(HAL_SavedRegisters));   \
    for( _i_ = HAL_THREAD_CONTEXT_FIRST; _i_ <= HAL_THREAD_CONTEXT_LAST;    \
           _i_++ )                                                          \
        (_regs_)->d[_i_] = (_id_)|_i_;                                      \
    (_regs_)->d[00] = (CYG_WORD)(_thread_); /* R0 = arg1 = thread ptr */    \
    (_regs_)->sp = (CYG_WORD)(_sp_);        /* SP = top of stack      */    \
    (_regs_)->lr = (CYG_WORD)(_entry_);     /* LR = entry point       */    \
    (_regs_)->pc = (CYG_WORD)(_entry_);     /* PC = [initial] entry point */\
    (_regs_)->cpsr = (CPSR_THREAD_INITIAL); /* PSR = Interrupt enabled */   \
    _sparg_ = (CYG_ADDRESS)_regs_;                                          \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// Context switch macros.
// The arguments are pointers to locations where the stack pointer
// of the current thread is to be stored, and from where the sp of the
// next thread is to be fetched.

externC void hal_thread_switch_context( CYG_ADDRESS to, CYG_ADDRESS from );
externC void hal_thread_load_context( CYG_ADDRESS to )
    __attribute__ ((noreturn));

#define HAL_THREAD_SWITCH_CONTEXT(_fspptr_,_tspptr_)                    \
        hal_thread_switch_context((CYG_ADDRESS)_tspptr_,                \
                                  (CYG_ADDRESS)_fspptr_);

#define HAL_THREAD_LOAD_CONTEXT(_tspptr_)                               \
        hal_thread_load_context( (CYG_ADDRESS)_tspptr_ );

//--------------------------------------------------------------------------
// Execution reorder barrier.
// When optimizing the compiler can reorder code. In multithreaded systems
// where the order of actions is vital, this can sometimes cause problems.
// This macro may be inserted into places where reordering should not happen.

#define HAL_REORDER_BARRIER() asm volatile ( "" : : : "memory" )

//--------------------------------------------------------------------------
// Breakpoint support
// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen
// if executed.
// HAL_BREAKINST is the value of the breakpoint instruction and 
// HAL_BREAKINST_SIZE is its size in bytes.

#define HAL_BREAKPOINT(_label_)                \
asm volatile (" .globl  " #_label_ ";"         \
              #_label_":"                      \
              " .word 0xE7FFDEFE"              \
    );

//#define HAL_BREAKINST           {0xFE, 0xDE, 0xFF, 0xE7}
#define HAL_BREAKINST           0xE7FFDEFE
#define HAL_BREAKINST_SIZE      4

//--------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

// GDB expects the registers in this structure:
//   r0..r10, fp, ip, sp, lr, pc  - 4 bytes each
//   f0..f7                       - 12 bytes each (N/A on ARM7)
//   fps                          - 4 bytes       (N/A on ARM7)
//   ps                           - 4 bytes

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure.
#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )  \
        (_regs_) = (HAL_SavedRegisters *)(_sp_)

// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.    
#define HAL_GET_GDB_REGISTERS( _aregval_, _regs_ )              \
    CYG_MACRO_START                                             \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);       \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ <= 10; _i_++ )                            \
        _regval_[_i_] = (_regs_)->d[_i_];                       \
                                                                \
    _regval_[11] = (_regs_)->fp;                                \
    _regval_[12] = (_regs_)->ip;                                \
    _regval_[13] = (_regs_)->sp;                                \
    _regval_[14] = (_regs_)->lr;                                \
    _regval_[15] = (_regs_)->pc;                                \
    _regval_[25] = (_regs_)->cpsr;                              \
    for( _i_ = 0; _i_ < 8; _i_++ )                              \
        _regval_[_i_+16] = 0;                                   \
    CYG_MACRO_END

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )             \
    CYG_MACRO_START                                             \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);       \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ <= 10; _i_++ )                            \
        (_regs_)->d[_i_] = _regval_[_i_];                       \
                                                                \
    (_regs_)->fp = _regval_[11];                                \
    (_regs_)->ip = _regval_[12];                                \
    (_regs_)->sp = _regval_[13];                                \
    (_regs_)->lr = _regval_[14];                                \
    (_regs_)->pc = _regval_[15];                                \
    (_regs_)->cpsr = _regval_[25];                              \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// HAL setjmp

#define CYGARC_JMP_BUF_SIZE 16  // Actually 11, but some room left over

typedef cyg_uint32 hal_jmp_buf[CYGARC_JMP_BUF_SIZE];

externC int hal_setjmp(hal_jmp_buf env);
externC void hal_longjmp(hal_jmp_buf env, int val);


//--------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor.

externC void hal_idle_thread_action(cyg_uint32 loop_count);

#define HAL_IDLE_THREAD_ACTION(_count_) hal_idle_thread_action(_count_)

//---------------------------------------------------------------------------

// Minimal and sensible stack sizes: the intention is that applications
// will use these to provide a stack size in the first instance prior to
// proper analysis.  Idle thread stack should be this big.

//    THESE ARE NOT INTENDED TO BE MICROMETRICALLY ACCURATE FIGURES.
//           THEY ARE HOWEVER ENOUGH TO START PROGRAMMING.
// YOU MUST MAKE YOUR STACKS LARGER IF YOU HAVE LARGE "AUTO" VARIABLES!

// This is not a config option because it should not be adjusted except
// under "enough rope" sort of disclaimers.

// A minimal, optimized stack frame, rounded up - no autos
#define CYGNUM_HAL_STACK_FRAME_SIZE (4 * 20)

// Stack needed for a context switch: this is implicit in the estimate for
// interrupts so not explicitly used below:
#define CYGNUM_HAL_STACK_CONTEXT_SIZE (4 * 20)

// Interrupt + call to ISR, interrupt_end() and the DSR
#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
    ((4 * 20) + 2 * CYGNUM_HAL_STACK_FRAME_SIZE)

// Space for the maximum number of nested interrupts, plus room to call functions
#define CYGNUM_HAL_MAX_INTERRUPT_NESTING 4

#define CYGNUM_HAL_STACK_SIZE_MINIMUM \
        (CYGNUM_HAL_MAX_INTERRUPT_NESTING * CYGNUM_HAL_STACK_INTERRUPT_SIZE + \
         2 * CYGNUM_HAL_STACK_FRAME_SIZE)

#define CYGNUM_HAL_STACK_SIZE_TYPICAL \
        (CYGNUM_HAL_STACK_SIZE_MINIMUM + \
         16 * CYGNUM_HAL_STACK_FRAME_SIZE)

//-----------------------------------------------------------------------------

#endif // CYGONCE_HAL_ARCH_H
// End of hal_arch.h
