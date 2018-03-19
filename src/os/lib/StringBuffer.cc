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

#include "lib/StringBuffer.h"


/*****************************************************************************
 * Methode:         StringBuffer::put                                        *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Fuegt ein Zeichen in den Puffer ein. Wenn der Puffer     *
 *                  daraufhin voll ist, wird er durch Aufruf der Methode     *
 *                  flush geleert.                                           *
 *                                                                           *
 * Parameter:                                                                *
 *      c:          Einzufuegendes Zeichen.                                  * 
 *****************************************************************************/
void StringBuffer::put (char c) {
    buffer[pos] = c;
    pos++;
    if (pos == sizeof (buffer))
        flush ();
 }

