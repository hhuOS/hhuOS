#include "lib/libc/locale.h"

#include <stddef.h>


struct lconv current_locale = {
	//numeric formatting
	".",
	",",
	"",
	
	//monetary numeric formatting
	".",
	",",
	"",
	"",
	"-",
	
	//local monetary numeric formatting
	"€",
	2,
	0,
	0,
	0,
	0,
	1,
	1,
	
	//internation monetary numerc formatting
	"EUR",
	2
};


char * setlocale([[maybe_unused]] int category, [[maybe_unused]] const char* locale) {
	return NULL; //Always fails
}

struct lconv * localeconv() {
	return &current_locale;
}


