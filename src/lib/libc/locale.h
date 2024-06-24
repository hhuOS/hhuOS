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
	char * decimal_point;
	char * thousands_sep;
	char * grouping;
	
	//monetary numeric formatting
	char * mon_decimal_point;
	char * mon_thousands_sep;
	char * mon_grouping;
	char * positive_sign;
	char * negative_sign;
	
	//local monetary numeric formatting
	char * currency_symbol;
	char frac_digits;
	char p_cs_precedes;
	char n_cs_precedes;
	char p_sep_by_space;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	
	//internation monetary numerc formatting
	char * int_curr_symbol;
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