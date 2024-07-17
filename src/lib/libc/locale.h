#ifndef LIBC_LOCALE
#define LIBC_LOCALE 


#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5

struct lconv {
	//numeric formatting
	const char * decimal_point;
	const char * thousands_sep;
	const char * grouping;
	
	//monetary numeric formatting
	const char * mon_decimal_point;
	const char * mon_thousands_sep;
	const char * mon_grouping;
	const char * positive_sign;
	const char * negative_sign;
	
	//local monetary numeric formatting
	const char * currency_symbol;
	char frac_digits;
	char p_cs_precedes;
	char n_cs_precedes;
	char p_sep_by_space;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	
	//internation monetary numerc formatting
	const char * int_curr_symbol;
	char int_frac_digits;
};


#ifdef __cplusplus
extern "C" {
#endif

char * setlocale(int category, const char* locale); //Currently a no-op
struct lconv * localeconv();

#ifdef __cplusplus
}
#endif

#endif