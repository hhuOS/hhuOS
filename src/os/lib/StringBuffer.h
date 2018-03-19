/*****************************************************************************
 *                                                                           *
 *                       S T R I N G B U F F E R                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Die Klasse StringBuffer stellt einen Puffer fuer die     *
 *                  Sammlung von Zeichen zur Darstellung auf dem Bildschirm  * 
 *                  oder anderen Ausgabegeraeten bereit. Die Ausgabe der     *
 *                  Zeichen erfolgt, sobald der Puffer voll ist oder wenn    *
 *                  explizit die Methode flush() aufgerufen wird.            *
 *                  Da StringBuffer geraeteunabhaengig sein soll, ist        *
 *                  flush() eine virtuelle Methode, die von den abgeleiteten *
 *                  Klassen definiert werden muss.                           *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Aenderungen von Michael Schoettner, HHU, 1.8.16          *
 *****************************************************************************/
#ifndef __StringBuffer_include__
#define __StringBuffer_include__

#include <cstdint>

class StringBuffer {
    
private:
    StringBuffer(const StringBuffer &copy); // Verhindere Kopieren

    // Alle Variablen und Methoden dieser Klasse sind "protected",
    // da die abgeleiteten Klassen einen direkten Zugriff auf den
    // Puffer, den Konstruktor, den Destruktor und die Methode put
    // benoetigen. Die Methode flush() muss sowieso neu definiert
    // werden und kann dann auch public werden.

protected:
    char buffer[80];
    uint32_t pos;

    // StringBuffer: Im Konstruktor wird der Puffer als leer markiert.
    StringBuffer () : pos(0) {}

    // ~StringBuffer: Im Destruktor gibt es nichts mehr zu tun.
    virtual ~StringBuffer() = default;

    // Fuegt ein Zeichen in den Puffer ein. Wenn der Puffer
    void put (char c);

    // Methode zur Ausgabe des Pufferinhalts
    virtual void flush () = 0;
    
};

#endif
