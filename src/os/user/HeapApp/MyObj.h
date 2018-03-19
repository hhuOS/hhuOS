/*****************************************************************************
 *                                                                           *
 *                                M Y O B J                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Test-Objekt, welches dynamisch alloziert wird.           *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 25.9.2016                       *
 *****************************************************************************/

#ifndef __MyObj_include__
#define __MyObj_include__


class MyObj {
    
private:
    MyObj (const MyObj &copy); // Verhindere Kopieren

public:
    int a;
    int b;

    MyObj () {
        a=0;
        b=0;
    }
    
    MyObj (int p_a, int p_b) {
        a = p_a; b = p_b;
    }
    
    void dump();
 };

#endif
