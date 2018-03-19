/*****************************************************************************
 *                                                                           *
 *                          T H R E A D S T A T E                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Die Struktur ThreadState dient dazu, bei einem Thread-   *
 *                  wechsel die Werte aller Register zu sichern.             *
 *                                                                           *
 * Achtung:         Fuer den Zugriff auf die Elemente von                    *
 *                  struct ThreadState aus einer Assemblerfunktion           *
 *                  heraus werden in der Datei Thread.inc Namen fuer die     *
 *                  benoetigten Abstaende der einzelnen Elemente zum Anfang  *
 *                  der Struktur definiert. Damit dann auch auf die richtigen*
 *                  Elemente zugegriffen wird, sollten sich die Angaben von  *
 *                  ThreadState.h und Thread.inc exakt entsprechen.          *
 *                  Wer also ThreadState.h aendert, muss auch                *
 *                  Thread.inc anpassen (und umgekehrt.)                     *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Michael Schoettner, HHU, 25.8.2016                       *
 *****************************************************************************/

#ifndef __ThreadState_include__
#define __ThreadState_include__

#include <cstdint>

struct Context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
} __attribute__((packed));

struct InterruptFrame {
    uint16_t gs;
    uint16_t pad6;
    uint16_t fs;
    uint16_t pad5;
    uint16_t es;
    uint16_t pad4;
    uint16_t ds;
    uint16_t pad3;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt;
    uint32_t error;
    uint32_t eip;
    uint16_t cs;
    uint16_t pad2;
    uint32_t eflags;
    uint32_t uesp;
    uint16_t ss;
    uint16_t pad1;
} __attribute__((packed));

#endif

