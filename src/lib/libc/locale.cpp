#include "lib/libc/locale.h"

#include <stddef.h>


struct lconv current_locale = {
	//numeric formatting
	.decimal_point = ".",
	.thousands_sep = ",",
	.grouping = "",
	
	//monetary numeric formatting
	.mon_decimal_point = ".",
	.mon_thousands_sep = ",",
	.mon_grouping = "",
	.positive_sign = "",
	.negative_sign = "-",
	
	//local monetary numeric formatting
	.currency_symbol = "€",
	.frac_digits = 2,
	.p_cs_precedes = 0,
	.n_cs_precedes = 0,
	.p_sep_by_space = 0,
	.n_sep_by_space = 0,
	.p_sign_posn = 1,
	.n_sign_posn = 1,
	
	//internation monetary numerc formatting
	.int_curr_symbol = "EUR",
	.int_frac_digits = 2
};


char * setlocale([[maybe_unused]] int category, [[maybe_unused]] const char* locale) {
	return NULL; //Always fails
}

struct lconv * localeconv() {
	return &current_locale;
}


