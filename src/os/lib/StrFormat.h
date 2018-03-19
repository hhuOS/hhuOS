#ifndef _LIB_STR_FORMAT_H
#define _LIB_STR_FORMAT_H 1

#include <cstdarg>

class StrFormat {

public:

    static char * convert(unsigned int number, unsigned int base, unsigned int padding = 0) {

        unsigned int length = 0;

        static const char* hex = "0123456789ABCDEF";

        static char buffer[50];

        char *ptr;

        ptr = &buffer[49];

        *ptr = '\0';

        do {

            *--ptr = hex[number % base];

            number /= base;

            length++;

        } while (number != 0);
        
        while (length < padding) {

            *--ptr = hex[number % base];

            length++;
        }
        
        return(ptr);
    }
    
    static char * _binar(unsigned int number) {
        static const char* digi = "01";
        static char buffer[50];
        char *ptr = &(buffer[49]);
        *ptr = '\0';

        do {
            *--ptr = digi[number % 2];
            number /= 2;
        } while (number != 0);

        return(ptr);
    }

    static void _putc(char * strout, const char & character, int & n) {
        strout[n++] = character;
    }
    
    static void _puts(char* strout, int nmax, const char* str, int & n) {
        for (unsigned int i = 0; str[i] != '\0'; ++i) {
            if ( n == (nmax-1) ) {
                // early terminating
                //strout[n] = '\0';
                // written chars, but as negative number, because we want to write 1 more, but we can't
                n = -1 * (n-1); 
                return;
            }
            strout[n++] = str[i];
        }
        strout[n] = '\0';
    }

    typedef struct Result_s {
        bool         success;
        int          lastHandledId;
        int          argCount;
        char *       lastHandledArgS;
        int          lastHandledArgI;
        char         lastHandledType;
        char         overwrittenChar;
        const char * unhandledFormat;
    } Result;


    static int _fc(char * strout, int nmax, const char * format, void * val) {
        int n = 0;
        if (format[n] != 'c') return 0;
        if (format[n] == '\0') return -1;
        _putc(strout, *((char *)val), n);
        return n;
    }

    static int _fs(char * strout, int nmax, const char * format, void * val) {
        int n = 0;
        if (format[n] != 's') return 0;
        if (format[n] == '\0') return -1;
        _puts(strout, nmax, (char *)val, n);
        return n;
    }
    
    static int _fd(char * strout, int nmax, const char * format, void * val) {
        int n = 0;
        if (format[n] != 'd') return 0;
        if (format[n] == '\0') return -1;
        int i = *((int *)val);
        if (i < 0) {
            i = -i;
            _putc(strout, '-', n);
        }
        if (n >= nmax) return -1;
        _puts(strout, nmax, convert(i, 10), n);
        return n;
    }

    static int _fo(char * strout, int nmax, const char * format, void * val) {
        int n = 0;
        if (format[n] != 'o') return 0;
        if (format[n] == '\0') return -1;
        unsigned int i = *((unsigned int *)val);
        _putc(strout, '0', n);
        if (n >= nmax) return -1;
        _puts(strout, nmax, convert(i, 8), n);
        return n;
    }

    static int _fx(char * strout, int nmax, const char * format, void * val) {
        int n = 0;
        if (format[n] != 'x') return 0;
        if (format[n] == '\0') return -1;
        unsigned int i = *((unsigned int *)val);
        _putc(strout, '0', n);
        if (n >= nmax) return -1;
        _putc(strout, 'x', n);
        if (n >= nmax) return -1;
        _puts(strout, nmax, convert(i, 16), n);
        return n;
    }

    static int _fb(char * strout, int nmax, const char * format, void * val) {
        int n = 0;
        if (format[n] != 'b') return 0;
        if (format[n] == '\0') return -1;
        unsigned int i = *((unsigned int *)val);
        _putc(strout, '0', n);
        if (n >= nmax) return -1;
        _putc(strout, 'b', n);
        if (n >= nmax) return -1;
        _puts(strout, nmax, _binar(i), n);
        return n;
    }
    
};

#endif
