/*****************************************************************************
 *                                                                           *
 *                               S O U N D                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 26.8.2016                       *
 *****************************************************************************/

#include <kernel/services/SoundService.h>
#include <kernel/Kernel.h>
#include "user/LoopSoundApp/Sound.h"



/*****************************************************************************
 * Methode:         Sound::run                                               *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Code des Threads.                                        *
 *****************************************************************************/
void Sound::run () {
    Kernel::getService<SoundService>()->getSpeaker()->aerodynamic();
}
