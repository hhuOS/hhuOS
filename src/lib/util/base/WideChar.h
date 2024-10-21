#ifndef HHUOS_WIDECHAR_H
#define HHUOS_WIDECHAR_H

#include <stddef.h>

namespace Util {
	
class WideChar {
	public:
		static int utf8Length(const char* bytes, size_t maxLength);
		static int utf8ToWchar(wchar_t * wideChar, const char* bytes, size_t maxLength);
		static int wcharToUtf8(char * bytes, wchar_t wideChar);
};

}


#endif