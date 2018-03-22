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
#ifndef __OutputStream_include__
#define __OutputStream_include__

#include "lib/StringBuffer.h"

class OutputStream : public StringBuffer {

private:
      OutputStream(const OutputStream &copy); // Verhindere Kopieren

public:
      int base;   // Basis des Zahlensystems: z.B. 2, 8, 10 oder 16

      OutputStream () : StringBuffer () { base = 10; }   // initial Dezimalsystem
      virtual ~OutputStream () {}

      virtual void flush () = 0;                    // weiterhin undefiniert

      // OPERATOR << : Umwandlung des angegebenen Datentypes in eine
      //               Zeichenkette.

      // Darstellung eines Zeichens (trivial)
      OutputStream& operator << (char c);
      OutputStream& operator << (unsigned char c);

      // Darstellung einer nullterminierten Zeichenkette
      OutputStream& operator << (char* string);
      OutputStream& operator << (const char* string);

      //  Darstellung ganzer Zahlen im Zahlensystem zur Basis base
      OutputStream& operator << (short ival);
      OutputStream& operator << (unsigned short ival);
      OutputStream& operator << (int ival);
      OutputStream& operator << (unsigned int ival);
      OutputStream& operator << (long ival);
      OutputStream& operator << (unsigned long ival);

      // Darstellung eines Zeigers als hexadezimale ganze Zahl
      OutputStream& operator << (void* ptr);

      // Aufruf einer Manipulatorfunktion
      OutputStream& operator << (OutputStream& (*f) (OutputStream&));

};


//
// Manipulatorfunktionen
//
// Die folgenden Funktionen erhalten und liefern jeweils eine Referenz auf
// ein OutputStream Objekt. Da die Klasse OutputStream einen Operator << fuer
// derartige Funktionen definiert, koennen sie mit Hilfe dieses Operators
// aufgerufen und sogar in weitere Eingaben eingebettet werden.
// Aufgabe der Manipulatoren ist, die Darstellung der nachfolgenden Ausgaben
// zu beeinflussen, z.B durch die Wahl des Zahlensystems.

// Zeilenumbruch in Ausgabe einfuegen.
OutputStream& endl (OutputStream& os);

// Waehle binaeres Zahlensystem aus.
OutputStream& bin (OutputStream& os);

// Waehle oktales Zahlensystem aus.
OutputStream& oct (OutputStream& os);

// Waehle dezimales Zahlensystem aus.
OutputStream& dec (OutputStream& os);

// Waehle hexadezimales Zahlensystem aus.
OutputStream& hex (OutputStream& os);

#endif
