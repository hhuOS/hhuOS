#include <cstdint>

#include "lib/util/base/operators.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/ArrayList.h"

// Export functions
extern "C" {
void initMemoryManager(uint8_t *startAddress);
void initLibc();
void _exit(int32_t);
}

//Import functions
extern "C" void _fini();

void initMemoryManager(uint8_t *startAddress) {
    auto *memoryManager = new (reinterpret_cast<void*>(Util::USER_SPACE_MEMORY_MANAGER_ADDRESS)) Util::FreeListMemoryManager();
    memoryManager->initialize(startAddress, reinterpret_cast<uint8_t*>(Util::MAIN_STACK_START_ADDRESS - 1));
}

void _sysExit(int32_t exitCode) {
	_fini(); //call deconstructors
    Util::System::call(Util::System::EXIT_PROCESS, 1, exitCode);
}


void initLibc() {
}



//Libc support
namespace LibcRuntime {

Util::ArrayList<void (*)(void)> callOnExitCleanup;

void addExitCleanupFunc(void (*func)(void)) {
	callOnExitCleanup.add(func);
}

void abort() {
	_sysExit(-1);
}

void exitWithCleanup(int32_t exitCode) {
	
	for(auto it = callOnExitCleanup.begin(); it != callOnExitCleanup.end(); ++it) {
		(*it)();
	}
	_sysExit(exitCode);
}

}

void _exit(int32_t exitCode) {
	LibcRuntime::exitWithCleanup(exitCode);
}