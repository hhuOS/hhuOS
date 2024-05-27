

namespace LibcRuntime {
	void addExitCleanupFunc(void (*func)(void));
	void abort();
	void exitWithCleanup(int32_t exitCode);
}