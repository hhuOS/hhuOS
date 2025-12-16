#include <stdint.h>

#include "runtime.h"

#include "lib/util/base/operators.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/libc/time.h"
#include "lib/libc/stdio.h"

// Export functions
extern "C" {
void initMemoryManager(uint8_t *startAddress);
void initLibc();
void appExit(int32_t);
}

//Import functions
extern "C" void _fini();

void initMemoryManager(uint8_t *startAddress) {
    new (&Util::System::getAddressSpaceHeader().heapMemoryManager)
		Util::FreeListMemoryManager(startAddress,
			reinterpret_cast<void*>(Util::USER_SPACE_STACK_MEMORY_START_ADDRESS));

	new (&Util::System::getAddressSpaceHeader().stackMemoryManager)
		Util::BitmapMemoryManager(reinterpret_cast<uint8_t*>(Util::USER_SPACE_STACK_MEMORY_START_ADDRESS),
			reinterpret_cast<uint8_t*>(Util::MEMORY_END_ADDRESS), Util::MAX_USER_STACK_SIZE);

	Util::System::getAddressSpaceHeader().stackMemoryManager
		.markBlock(reinterpret_cast<uint8_t*>(Util::USER_SPACE_STACK_MEMORY_START_ADDRESS), true);
}


void initLibc() {
	libc_initialize_time();
	libc_init_stdio();
}

void appExit(const int32_t exitCode) {
	LibcRuntime::exitWithCleanup(exitCode);
}

//Libc support
namespace LibcRuntime {

Util::ArrayList<void (*)()> callOnExitCleanup;

void addExitCleanupFunc(void (*func)()) {
	callOnExitCleanup.add(func);
}

void abort() {
	// Exit process immediately with error code
    Util::System::call(Util::System::EXIT_PROCESS, 1, -1);
}

void exitWithCleanup(const int32_t exitCode) {
	// Call registered C cleanup functions (atexit)
	for(const auto &func : callOnExitCleanup) {
		func();
	}

	// Call C++ destructors
	_fini();

	// Exit process
	Util::System::call(Util::System::EXIT_PROCESS, 1, exitCode);
}

}