/*****************************************************************************
 *                                                                           *
 *                              R A N D O M                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Generiert einfache Zufallszahlen zu Testzwecken.         *
 *                  Nicht für Krypto benutzen :-)                            *
 *                                                                           *
 * Autor:           Burak Akgül und Christian Gesse, HHU, 2017               *
 *****************************************************************************/

#include <kernel/Kernel.h>
#include <kernel/services/TimeService.h>
#include "lib/Random.h"



Random::Random(uint32_t seed, uint32_t maxRand) {
	this->seed = seed;
	this->maxRand = maxRand;
}

Random::Random(uint32_t maxRand) {
	this->seed = ((TimeService*) Kernel::getService(TimeService::SERVICE_NAME))->getSystemTime();
	this->maxRand = maxRand;
}


// Liefert eine Zufallszahl zwischen 0 und maxRand - 1
unsigned int Random::rand() {
	this->seed = this->seed * 1103515255 + 12345;
	return (unsigned int)(seed / 65536) % maxRand;
}

unsigned int Random::rand(unsigned int maxRand) {
	this->seed = this->seed * 1103515255 + 12345;
	return (unsigned int)(seed / 65536) % maxRand;
}

void Random::setSeed(uint32_t seed) {
	this->seed = seed;
}
