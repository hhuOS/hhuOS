/* Memory manager for the kernel heap
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 */

#include <lib/libc/printf.h>
#include "HeapMemoryManager.h"

#include "../SystemManagement.h"
#include "kernel/memory/MemLayout.h"


HeapMemoryManager::HeapMemoryManager(uint32_t startAddress, uint32_t endAddress) : MemoryManager(startAddress, endAddress) {
}

HeapMemoryManager::~HeapMemoryManager() {
	if(lock){
		delete lock;
	}
}

void HeapMemoryManager::init() {
    freeMemory = endAddress - startAddress;

    if(freeMemory < sizeof(Chunk) + (sizeof(Chunk) + sizeof(Spinlock))) {
        // Available Kernel-Memory is too small for a Chunk
		firstChunk = 0;
	}else {
		firstChunk = (Chunk*)startAddress;
		firstChunk->next = 0;
		firstChunk->prev = 0;
		firstChunk->allocated = false;
		freeMemory -= sizeof(Chunk);
		firstChunk->size = freeMemory;
	}
	
	lock = new Spinlock();

    initialized = true;
}


void* HeapMemoryManager::alloc(uint32_t size) {

	// Zu kleine Blöcke nicht anfordern
	if(size < 1) {
		return 0;
	}

	if(initialized) {
		lock->lock();
	}

	Chunk* returnChunk = 0;
	Chunk* lastVisited = firstChunk;

    bool firstRun = true;
	
	// Suche einen passenden Chunk - first fit Modell
	do{

        if (!firstRun && lastVisited == firstChunk) {
            return nullptr;
        }

        firstRun = false;

		// prüfe, ob der Chunk groß genug ist
		if(lastVisited->size >= size && !lastVisited->allocated) {
			returnChunk = lastVisited;
			break;
		}

		lastVisited = lastVisited->next;
	// solange kein passender freier Chunk gefunden wurde UND der Chunk, mit dem die Suche
	// gestartet wurde, nicht erreicht ist, suche weiter
	} while(lastVisited != 0);

	// kein freien Chunk gefunden
	if(returnChunk == 0) {
		if(initialized) {
			lock->unlock();
		}
		return 0;
	}
	
	// gefundener Chunk größer als benötigt -> zerteile mit passender Größe
	if(returnChunk-> size >= sizeof(Chunk) + size + 1) {
		Chunk* tmp = (Chunk*)((unsigned int)returnChunk + sizeof(Chunk) + size); // "nächsten" Chunk holen - überspringe den allozierten Bereich
		tmp->prev = returnChunk; 				// prev und next Zeiger vom nächsten Chunk aktualisieren
		tmp->next = returnChunk->next;
		if(returnChunk->next) {
			returnChunk->next->prev = tmp;
		}
		returnChunk->next = tmp; 				// zeiger vom allozierten Chunk aktualisieren
		tmp->allocated = false; 				// nächsten Block (neuer Chunk mit Restspeicher) als allozierbar markieren
		freeMemory -= sizeof(Chunk);
		tmp->size = returnChunk->size - size - sizeof(Chunk); // Chunk size initialisieren
		returnChunk->size = size; // größe des chunks aktuallisieren
	}
	// allozierter Bereich ist nicht mehr zu vergeben
	returnChunk->allocated = true;
	freeMemory -= returnChunk->size;
	// gib Pointer auf den reservierten Speicher hinter dem Chunkeintrag zurück
	if(initialized) {
		lock->unlock();
	}
	return (void*)((unsigned int)returnChunk + sizeof(Chunk));
}

void HeapMemoryManager::free(void* ptr) {

	// Prevent null pointer access
	if (ptr == nullptr) {
		return;
	}

	// calculate ptr to chunk with metadata
	Chunk* chunk = (Chunk*)((unsigned int)ptr - sizeof(Chunk));

	lock->lock();
	// Chunk is no longer reserved/allocated
	chunk->allocated = false;
	freeMemory += chunk->size;

	//if preceding chunks are also free -> merge them
	while(chunk->prev) {
		if(chunk->prev->allocated) {
			break;
		}
		freeMemory += sizeof(Chunk);
		// calculate size of new chunk
		chunk->prev->size += chunk->size + sizeof(Chunk);
		// update next pointer
		chunk->prev->next = chunk->next;
		// biege ggfs. prev-Zeiger um ???
		if(chunk->next) 
			chunk->next->prev = chunk->prev;

		chunk = chunk->prev;
	}
	
	//if succeeding chunks are also free -> merge them
	while(chunk->next) {
		// nachfolgender Block frei - verschmelzen
		if(chunk->next->allocated) {
			break;
		}
		freeMemory += sizeof(Chunk);
		chunk->size += chunk->next->size + sizeof(Chunk);
		chunk->next = chunk->next->next;
		
		if(chunk->next) 
			chunk->next->prev = chunk;
	
	}

	if(chunk->size >= 4*1024 && SystemManagement::getInstance()->isInitialized()) {
		uint32_t addr = (uint32_t) chunk;
		uint32_t chunkEndAddr = addr + (sizeof(Chunk) + chunk->size);
//TODO fix unmap bug
#if CGA_DEBUG
    cga_dbg.puts("CALL UNMAP\n",11);
#endif
    SystemManagement::getInstance()->unmap(addr + sizeof(Chunk), chunkEndAddr - 1);
#if CGA_DEBUG
		cga_dbg.puts("RETURNED FROM UNMAP\n",20);
#endif
	}


	lock->unlock();


}

void HeapMemoryManager::dump() {
    printf("  Memory dump\n");
    printf("  ===========\n");

    Chunk* tmp = firstChunk;

    while ( tmp != 0 ) {
		printf("    Start: %x" , (Chunk*)((uint8_t*)tmp + sizeof(Chunk)));
		printf(", End: %x" , (Chunk*)((uint8_t*)tmp + sizeof(Chunk) + tmp->size));
		printf(", Allocated: %d" , tmp->allocated);
		printf("  Next: %x", tmp->next);
		printf(", Size: %d\n" , tmp->size);

        tmp = tmp->next;
    }

    printf("\n");
}
