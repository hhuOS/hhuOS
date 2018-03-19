/*****************************************************************************
 *                                                                           *
 *                                E R R N O                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Description:     Defines several macros, which are related to errors.     *
 *                                                                           *
 *                                                                           *
 * Author:          Fabian Ruhland, HHU, 16.10.2017                          *
 *****************************************************************************/

#ifndef __errno_include__
#define __errno_include__

#define EDOM    1
#define ERANGE  2

static int errno = 0;

#endif
