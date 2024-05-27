#include <cstdint>

#include <stdlib.h>

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintStream.h"


void onEnd() {
	Util::System::out << "Bye!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
}

int32_t main(int32_t argc, char *argv[]) {
	atexit(&onEnd);
	//exit(EXIT_SUCCESS);
    return 0;
}