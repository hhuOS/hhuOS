/*****************************************************************************
 *                                                                           *
 *                           O U T S T R E A M                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Die Klasse OutputStream enthaelt die Definition des      *
 *                  << Operators fuer die wichtigsten der vordefinierten     *
 *                  Datentypen und realisiert somit die bekannte Ausgabe-    *
 *                  funktion der C++ iO_Stream Bibliothek. Zur Zeit wird     *
 *                  die Darstellung von Zeichen, Zeichenketten und ganzen    *
 *                  Zahlen unterstuetzt. Ein weiterer << Operator erlaubt    *
 *                  die Verwendung von Manipulatoren.                        *
 *                                                                           *
 *                  Neben der Klasse OutputStream sind hier auch die         *
 *                  Manipulatoren hex, dec, oct und bin fuer die Wahl der    *
 *                  Basis bei der Zahlendarstellung, sowie endl fuer den     *
 *                  Zeilenumbruch definiert.                                 *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Aenderungen von Michael Schoettner, HHU, 1.8.16          *
 *****************************************************************************/

#include "lib/OutputStream.h"


//
// Zeichen und Zeichenketten in Stream ausgeben
//
OutputStream& OutputStream::operator << (char c) {
    put(c);
    return *this;
}

OutputStream& OutputStream::operator << (unsigned char c) {
    return *this << (char) c;
}

OutputStream& OutputStream::operator << (char* string) {
    char* pos = string;
    while (*pos) {
        put (*pos);
        pos++;
    }
    return *this;
}

OutputStream& OutputStream::operator << (const char* string) {
    return *this << (char *) string;
}

//
//  Ganzer Zahlen im Zahlensystem zur Basis base in Stream ausgeveb
//  Alle vorzeichenbehafteten Datentypen werden als long dargestellt,
//  Alle vorzeichenlosen als unsigned long.
OutputStream& OutputStream::operator << (short ival) {
    return *this << (long) ival;
}

OutputStream& OutputStream::operator << (unsigned short ival) {
    return *this << (unsigned long) ival;
}

OutputStream& OutputStream::operator << (int ival) {
    return *this << (long) ival;
}

OutputStream& OutputStream::operator << (unsigned int ival) {
    return *this << (unsigned long) ival;
}

// Darstellung eine vorzeichenbehafteten ganzen Zahl.
OutputStream& OutputStream::operator << (long ival) {
    // Bei negativen Werten wird ein Minuszeichen ausgegeben.
    if (ival < 0) {
        put ('-');
        ival = -ival;
    }
    // Dann wird der Absolutwert als vorzeichenlose Zahl ausgegeben.
    return *this << (unsigned long) ival;
}

// Darstellung einer vorzeichenlosen ganzen Zahl.
OutputStream& OutputStream::operator << (unsigned long ival) {
    unsigned long div;
    char digit;

    if (base == 8)
        put ('0');         // oktale Zahlen erhalten eine fuehrende Null
    else if (base == 16) {
        put ('0');         // hexadezimale Zahlen ein "0x"
        put ('x');
    }

    // Bestimmung der groessten Potenz der gewaehlten Zahlenbasis, die
    // noch kleiner als die darzustellende Zahl ist.
    for (div = 1; ival/div >= (unsigned long) base; div *= base);

    // ziffernweise Ausgabe der Zahl
    for (; div > 0; div /= (unsigned long) base) {
        digit = ival / div;
        if (digit < 10)
                put ('0' + digit);
        else
                put ('A' + digit - 10);
        ival %= div;
    }
    return *this;
}

// Darstellung eines Zeigers als hexadezimale ganze Zahl
OutputStream& OutputStream::operator << (void* ptr) {
    int oldbase = base;
    base = 16;
    *this << (unsigned long) ptr;
    base = oldbase;
    return *this;
 }

//   Aufruf einer Manipulatorfunktion
OutputStream& OutputStream::operator << (OutputStream& (*f) (OutputStream&)) {
    return f(*this);
}

//
// Manipulatorfunktionen
//
// Die folgenden Funktionen erhalten und liefern jeweils eine Referenz auf
// ein OutputStream Objekt. Da die Klasse O_Stream einen Operator << fuer
// derartige Funktionen definiert, koennen sie mit Hilfe dieses Operators
// aufgerufen und sogar in weitere Eingaben eingebettet werden.
// Aufgabe der Manipulatoren ist, die Darstellung der nachfolgenden Ausgaben
// zu beeinflussen, z.B durch die Wahl des Zahlensystems.

// Fuege einen Zeilenumbruch in die Ausgabe ein.
OutputStream& endl (OutputStream& os) {
    os << '\n';
    os.flush ();
    return os;
}

// Waehlt das binaere Zahlensystem aus.
OutputStream& bin (OutputStream& os) {
    os.base = 2;
    return os;
}

// Waehlt das oktale Zahlensystem aus.
OutputStream& oct (OutputStream& os) {
    os.base = 8;
    return os;
}

// Waehlt das dezimale Zahlensystem aus.
OutputStream& dec (OutputStream& os) {
    os.base = 10;
    return os;
}

// Waehlt das hexadezimale Zahlensystem aus.
OutputStream& hex (OutputStream& os) {
    os.base = 16;
    return os;
}
