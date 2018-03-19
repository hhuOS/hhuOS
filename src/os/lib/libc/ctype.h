/*****************************************************************************
 *                                                                           *
 *                                C T Y P E                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Description:     Provides several functions, that are useful for testing  *
 *                  characters.                                              *
 *                                                                           *
 *                                                                           *
 * Author:          Fabian Ruhland, HHU, 16.10.2017                          *
 *****************************************************************************/

#ifndef __ctype_include__
#define __ctype_include__

extern int isalnum(int);
extern int isalpha(int);
extern int iscntrl(int);
extern int isdigit(int);
extern int isgraph(int);
extern int islower(int);
extern int isprint(int);
extern int ispunct(int);
extern int isspace(int);
extern int isupper(int);
extern int isxdigit(int);

#endif
