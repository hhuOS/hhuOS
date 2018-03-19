/*****************************************************************************
 *                                                                           *
 *                                M Y O B J                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Test-Objekt, welches dynamisch alloziert wird.           *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 25.9.2016                       *
 *****************************************************************************/

#include "lib/libc/printf.h"
#include "user/HeapApp/MyObj.h"

// Variablen des Objektes ausgeben
void MyObj::dump () {
    printf("MyObj{ a=%08x, b=%08x }", a, b);
}
