#include <stdint.h>

#include "lib/util/base/operators.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/libc/time.h"
#include "lib/libc/stdio.h"

// Export functions
extern "C" {
void initMemoryManager(uint8_t *startAddress);
void initLibc();
void _exit(int32_t);
}

//Import functions
extern "C" void _fini();

void initMemoryManager(uint8_t *startAddress) {
    new (&Util::System::getAddressSpaceHeader().heapMemoryManager) Util::FreeListMemoryManager(startAddress, reinterpret_cast<void*>(Util::USER_SPACE_STACK_MEMORY_START_ADDRESS));
	new (&Util::System::getAddressSpaceHeader().stackMemoryManager) Util::BitmapMemoryManager(reinterpret_cast<uint8_t*>(Util::USER_SPACE_STACK_MEMORY_START_ADDRESS), reinterpret_cast<uint8_t*>(Util::MEMORY_END_ADDRESS), Util::MAX_USER_STACK_SIZE);
	Util::System::getAddressSpaceHeader().stackMemoryManager.markBlock(reinterpret_cast<uint8_t*>(Util::USER_SPACE_STACK_MEMORY_START_ADDRESS), true);
}

void _sysExit(int32_t exitCode) {
	Util::Graphic::Ansi::cleanupGraphicalApplication();
	_fini(); //call deconstructors
    Util::System::call(Util::System::EXIT_PROCESS, 1, exitCode);
}


void initLibc() {
	_time_initialize();
	_init_stdio();
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