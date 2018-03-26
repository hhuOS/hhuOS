/*****************************************************************************
 *                                                                           *
 *                          R A N D O M                                      *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung eines einfachen Zeitscheiben-Schedulers. *
 *                  Rechenbereite Threads werden in 'readQueue' verwaltet.   *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 22.8.2016                      *
 *****************************************************************************/

#ifndef __Random_include__
#define __Random_include__

#include <cstdint>

class Random {

private:
	Random(const Random &copy);

	uint32_t seed;
	uint32_t maxRand; // Obere Grenze für die Zufallszahl

public:
	// Konstruktoren
	Random(uint32_t seed, uint32_t maxRand);
	Random(uint32_t maxRand = 0xFFFFFFFF);

	// Liefert eine Zufallszahl zwischen 0 und maxRand - 1
	uint32_t rand();
	uint32_t rand(uint32_t maxRand);

    void setSeed(uint32_t seed);
};

#endif
