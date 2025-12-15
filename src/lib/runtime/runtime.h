#ifndef HHUOS_LIB_RUNTIME_H
#define HHUOS_LIB_RUNTIME_H

/// Contains support functions for C runtime operations like `exit()` and `abort()`.
namespace LibcRuntime {
	/// Register a function to be called on exit. This is used to implement `atexit()`.
	void addExitCleanupFunc(void (*func)());

	/// Aborts the current process immediately, without running cleanup functions.
	void abort();

	/// Exits the current process, running all registered cleanup functions first.
	void exitWithCleanup(int32_t exitCode);
}

#endif
