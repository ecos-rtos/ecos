#ifndef CYGONCE_DEVS_COMMON_TABLE_H
#define CYGONCE_DEVS_COMMON_TABLE_H

//==========================================================================
//
//      table.h
//
//      Header file for the device table
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:  proven
// Date:        1998-04-20
// Purpose:     Define Device Table
// Description: This file contains the device table structure
// Usage:       #include <cyg/devs/common/table.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

/*
 * devs.h should be replaced by an appropriate ECC Hardy package.
 * and this file should mostly be auto generated -- proven 19980625
 */
#include <pkgconf/devs.h>		// What devices do we need.

#include <cyg/infra/cyg_type.h>         // Common type definitions and support
#include <cyg/devs/common/iorb.h>       // Cyg_IORB definition
#include <cyg/error/codes.h>            // Standard error codes

// Handles to some of the device entries.
// These shouldn't go here and should be generated by ECC-Hardy
// --proven 19980625
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1
extern CYG_ADDRWORD stdeval1_serial1;
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2
extern CYG_ADDRWORD stdeval1_serial2;
#endif
#else
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2
extern CYG_ADDRWORD stdeval1_serial2;
#endif
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_TX39
extern CYG_ADDRWORD jmr3904_serial;
#endif

// Mode for opening

typedef enum {
    CYG_DEVICE_OPEN_MODE_TEXT,
    CYG_DEVICE_OPEN_MODE_RAW
} Cyg_DeviceOpenMode;

// -------------------------------------------------------------------------
// Cyg_Device_Table_t

/*
 * Device table.
 */
struct Cyg_Device_Table_t
{
    char *              name;
    CYG_ADDRWORD        cookie;
//    cyg_off_t         seek_position;  // Not implemented yet

    /*
     * Note: The cookie is necessary for the function to differentiate 
     * between various instantiations of the same device type.
     *
     * Also note that open() will be called before the kernel scheduler
     * (and hence the interrupt system) starts, so it must be safe for this
     */
    Cyg_ErrNo           (*open) (CYG_ADDRWORD cookie, Cyg_DeviceOpenMode);
    Cyg_ErrNo           (*read_cancel) (CYG_ADDRWORD cookie, Cyg_IORB *);
    Cyg_ErrNo           (*write_cancel) (CYG_ADDRWORD cookie, Cyg_IORB *);
    Cyg_ErrNo           (*read_blocking) (CYG_ADDRWORD cookie, Cyg_IORB *);
    Cyg_ErrNo           (*write_blocking) (CYG_ADDRWORD cookie, Cyg_IORB *);
    Cyg_ErrNo           (*read_asynchronous) (CYG_ADDRWORD cookie, Cyg_IORB *);
    Cyg_ErrNo           (*write_asynchronous) (CYG_ADDRWORD cookie, Cyg_IORB *);
    Cyg_ErrNo		(*close) (CYG_ADDRWORD cookie);

    CYG_ADDRWORD	ioctl;
};

externC struct Cyg_Device_Table_t Cyg_Device_Table[];

struct Cyg_Device_Serial_RS232_Table_t
{
    Cyg_ErrNo           (*set_kmode) (CYG_ADDRWORD cookie, cyg_uint32);
    Cyg_ErrNo           (*get_baud_rate) (CYG_ADDRWORD cookie, cyg_int32 * );
    Cyg_ErrNo           (*set_baud_rate) (CYG_ADDRWORD cookie, cyg_int32, cyg_int32 *);
    Cyg_ErrNo           (*get_line_mode) (CYG_ADDRWORD cookie, cyg_int32 * );
    Cyg_ErrNo           (*set_line_mode) (CYG_ADDRWORD cookie, cyg_int32, cyg_int32 *);

    Cyg_ErrNo           (*get_read_mode) (CYG_ADDRWORD cookie, cyg_int32 * );
    Cyg_ErrNo           (*set_read_mode) (CYG_ADDRWORD cookie, cyg_int32, cyg_int32 *);
    Cyg_ErrNo           (*set_read_data) (CYG_ADDRWORD cookie, cyg_int32, char *, cyg_uint32);
};

/*
 * C API is just a set of macros
 */
#define cyg_read_cancel(x, y)	\
	((struct Cyg_Device_Table_t *)x)->read_cancel(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_write_cancel(x, y)	\
	((struct Cyg_Device_Table_t *)x)->write_cancel(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_read_blocking(x, y)	\
	((struct Cyg_Device_Table_t *)x)->read_blocking(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_write_blocking(x, y)	\
	((struct Cyg_Device_Table_t *)x)->write_blocking(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_read_asynchronous(x, y)	\
	((struct Cyg_Device_Table_t *)x)->read_asynchronous(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_write_asynchronous(x, y)	\
	((struct Cyg_Device_Table_t *)x)->write_asynchronous(((struct Cyg_Device_Table_t *)x)->cookie, y)

/*
 * C API for Serial RS232 devices
 */
#define cyg_serial_rs232_set_kmode(x, y)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->set_kmode(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_serial_rs232_get_baud_rate(x, y)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->get_baud_rate(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_serial_rs232_set_baud_rate(x, y, z)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->set_baud_rate(((struct Cyg_Device_Table_t *)x)->cookie, y, z)

#define cyg_serial_rs232_get_line_mode(x, y)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->get_line_mode(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_serial_rs232_set_line_mode(x, y, z)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->set_line_mode(((struct Cyg_Device_Table_t *)x)->cookie, y, z)

#define cyg_serial_rs232_get_read_mode(x, y)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->get_read_mode(((struct Cyg_Device_Table_t *)x)->cookie, y)

#define cyg_serial_rs232_set_read_mode(x, y, z)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->set_read_mode(((struct Cyg_Device_Table_t *)x)->cookie, y, z)

#define cyg_serial_rs232_set_read_data(x, y, z, a)	\
	((struct Cyg_Device_Serial_RS232_Table_t *)((struct Cyg_Device_Table_t *)x)->ioctl)->set_read_data(((struct Cyg_Device_Table_t *)x)->cookie, y, z, a)



#endif  // CYGONCE_DEVS_COMMON_TABLE_H
// End of table.h

