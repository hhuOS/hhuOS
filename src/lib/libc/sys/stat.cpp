#include "lib/libc/sys/stat.h"

#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"

int mkdir(const char *pathname, [[maybe_unused]] mode_t mode) {
	Util::Io::File dir =  Util::Io::File(Util::String(pathname));
	return dir.create(Util::Io::File::DIRECTORY) ? 0 : -1;	
}