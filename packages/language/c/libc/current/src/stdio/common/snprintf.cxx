//===========================================================================
//
//      snprintf.cxx
//
//      ANSI Stdio snprintf() function
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include "clibincl/stdiosupp.hxx"   // Support functions for stdio

// EXPORTED SYMBOLS

externC int
snprintf( char *, size_t, const char *, ... )
    CYGPRI_LIBC_WEAK_ALIAS("_snprintf");


// FUNCTIONS

externC int
_snprintf( char *s, size_t size, const char *format, ... )
{
    int rc;      // return code
    va_list ap;  // for variable args

    va_start(ap, format); // init specifying last non-var arg

    rc = _vsnprintf(s, size, format, ap);

    va_end(ap); // end var args

    return rc;
} // _snprintf()



#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF snprintf.cxx
