#include <ctype.h>


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "lib/util/io/stream/ScanStream.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"


wchar_t readWchar(Util::Io::ScanStream &ss) {
	uint8_t wc[4];
	memset(wc, 0, 4);
	wc[0] = ss.read();
	for (int i=1; i<4; i++) {
		if (mblen( (const char * )wc, 4) != -1) break;
		wc[i] = ss.read();
	}
	
	if (mblen( (const char * )wc, 4) == -1) {
		errno = EILSEQ;
	}
	
	wchar_t ret;
	mbtowc(&ret, (const char * )wc, 4);
	return ret;
}

int _stream_vscanf(Util::Io::InputStream &is, const char* format, va_list vlist) {
	auto ss = Util::Io::ScanStream(is);
	int ret = 0;
	errno = 0;
	
	for (const char * c = format; *c != '\0' && ss.peek() != EOF && ss.peek() != '\0'; c++) {
		if (*c != '%') {
			
			if (!isspace(*c)) {
				//non-whitespace chars must match exactly
				if (ss.peek() == *c) {
					ss.read();
				} else {
					return EOF;
				}
				
			} else {
				//whitespace consumes all whitespace chars
				while (isspace(ss.peek())) ss.read();
			}
			
		} else {
			bool suppressAssign = false;
			int maxFieldWidth = -1;
			bool longParam = false;
			bool shortParam = false;
			
			while (1) {
				c++;
				
				if (*c == '%') break;
				else if (*c == 'h') shortParam = true;
				else if (*c == 'l' || *c == 'L') longParam = true;
				else if (isdigit(*c)) {
					maxFieldWidth = strtol(c, (char**)&c, 10);
					c--;
				} 
				else if (*c == '*') suppressAssign = true;
				else if (*c == '\0') return EOF; 
				else break;
			}
			
			if (maxFieldWidth >= 0) {
				ss.setReadLimit(ss.getReadBytes() + maxFieldWidth);
			} else {
				ss.setReadLimit(-1);
			}
			
			int i;
			char * s = nullptr;
			char c2;
			wchar_t * ws = nullptr;
			wchar_t w;
			long v;
			unsigned long v2;
			double d;
			void * p;
			
			bool invert = false;
			char set[1024];
			char  * setp;
			bool matched = false;
			switch(*c) {
				case '%':
					if (ss.peek() != '%' ) return EOF;
					ss.read();
					break;
					
				case '\0':
					return EOF;
					
				case 'd':
					v = ss.readInt(10);
					if (suppressAssign) {
						break;
					} else if (longParam) {
						*(va_arg(vlist, long int*)) = v;
					} else if (shortParam) {
						*(va_arg(vlist, short int*)) = v;
					} else {
						*(va_arg(vlist, int*)) = v;
					}
					break;
				
				case 'i':
					v = ss.readInt();
					if (suppressAssign) {
						break;
					} else if (longParam) {
						*(va_arg(vlist, long int*)) = v;
					} else if (shortParam) {
						*(va_arg(vlist, short int*)) = v;
					} else {
						*(va_arg(vlist, int*)) = v;
					}
					break;
					
				case 'u':
					v2 = ss.readUint(10);
					if (suppressAssign) {

					} else if (longParam) {
						*(va_arg(vlist, long int*)) = v2;
					} else if (shortParam) {
						*(va_arg(vlist, short int*)) = v2;
					} else {
						*(va_arg(vlist, int*)) = v2;
					}
					break;
					
				case 'o':
					v2 = ss.readUint(8);
					if (suppressAssign) {
						break;
					} else if (longParam) {
						*(va_arg(vlist, long int*)) = v2;
					} else if (shortParam) {
						*(va_arg(vlist, short int*)) = v2;
					} else {
						*(va_arg(vlist, int*)) = v2;
					}
					break;
					
				case 'x':
				case 'X':
					v2 = ss.readUint(16);
					if (suppressAssign) {
						break;
					} else if (longParam) {
						*(va_arg(vlist, long int*)) = v2;
					} else if (shortParam) {
						*(va_arg(vlist, short int*)) = v2;
					} else {
						*(va_arg(vlist, int*)) = v2;
					}
					break;
					
				case 'n':
					if (suppressAssign) {
						break;
					} else if (longParam) {
						*(va_arg(vlist, unsigned long int*)) = ss.getReadBytes();
					} else if (shortParam) {
						*(va_arg(vlist, unsigned short int*)) = ss.getReadBytes();
					} else {
						*(va_arg(vlist, unsigned int*)) = ss.getReadBytes();
					}
					break;
					
				case 'e':
				case 'E':
				case 'g':
				case 'G':
					d = ss.readDouble();
					if (suppressAssign) {
						break;
					} else if (longParam) {
						*(va_arg(vlist, long double*)) =d;
					} else if (shortParam) {
						*(va_arg(vlist, float*)) = d;
					} else {
						*(va_arg(vlist, double*)) = d;
					}
					break;
					
				case 'p':
					p =  (void*)ss.readUint(16);
					if (!suppressAssign) *(va_arg(vlist,void**)) = p;
					break;
				
				case 'c': //match fixed length of chars 
					if (maxFieldWidth < 0) maxFieldWidth = 1;
					if (longParam) {
						if (!suppressAssign) ws = va_arg(vlist, wchar_t*);
						for (i=0; i<maxFieldWidth; i++, ws++) {
							if (!suppressAssign) {
								*ws = readWchar(ss);
							} else {
								readWchar(ss);
							} 
						}
					} else {
						if (!suppressAssign) s = va_arg(vlist, char*);
						for (i=0; i<maxFieldWidth; i++, s++) {
							if (!suppressAssign) {
								*s = ss.read();
							} else {
								ss.read();
							} 
						}
					}
					break;
				
				case 's': //string match until next wide char
					if (longParam) { //wide char
						if (!suppressAssign) ws = va_arg(vlist, wchar_t*);
						i = 0;
						c2 = ss.peek();
						while (c2 != '\0' && !isspace(c2)) {
							if (maxFieldWidth >= 0 && i >= maxFieldWidth) break;
							w = readWchar(ss);
							fflush(stdout);
							if (!suppressAssign) {
								*ws = w;
								ws++;
							}
							i++;
							c2 = ss.peek();
						}
						
						if (!suppressAssign) *ws = '\0';
						
					} else { //normal char
						if (!suppressAssign) s = va_arg(vlist, char*);
						i = 0;
						c2 = ss.peek();
						
						while (!isspace(c2)) {
							if (maxFieldWidth >= 0 && i >= maxFieldWidth) break;
							ss.read();
							if (!suppressAssign) { 
								*s = c2;
								s++;
							}
							c2 = ss.peek();
							i++;
						}
						
						if (!suppressAssign) *s = '\0';
					}
					break;
				
				case '[': //string set match
					c++;
					setp = set;
					invert = false;
					
					//parse invert character
					if (*c == '^') {
						invert = true;
						c++;
						if (*c == ']') {
							*setp = ']';
							setp++;
							c++;
						}
					} else if (*c == ']'){
						*setp = ']';
						setp++;
						c++;
					}
					
					//parse set literal
					for (;*c != ']' && (setp - set) < 1024; c++) {
						if (!longParam) {
							if (*c == '-') {
								for (c2 = *(c-1); c2 <= *(c+1); c2++) {
									*setp = c2;
									setp++;
									if ((setp - set) >= 1024) break;
								}
							} else {
								*setp = *c;
								setp++;
							}
						}
					}
					*setp = '\0';
					
					//match stream output
					if (!suppressAssign) s = va_arg(vlist, char*);
					for (;;) {
						c2 = ss.peek();
						if (c2 == EOF) break;
						matched = false;
						for (setp = set; *setp != '\0'; setp++) {
							if ((invert && *setp != c2) || (!invert && *setp == c2)) {
								matched = true;
								break;
							}
						}
						
						if (matched) {
							if (!suppressAssign) {
								*s = ss.read();
							} else {
								ss.read();
							}
						} else {
							if (!suppressAssign) *s = '\0';
							break;
						}
						 if (!suppressAssign) s++;
					}
					if (!suppressAssign) *s = '\0';
						
					break;
					
			}
			if (!suppressAssign) ret++;
			ss.setReadLimit(-1);
		}
		
	}
	
	return ret;
}


int vscanf(const char* format, va_list args) {
	return vfscanf(stdin, format, args);
}

int vfscanf(FILE * stream, const char * format, va_list args) {
	return _stream_vscanf(*stream, format, args);
}

int vsscanf(const char * s, const char * format, va_list args) {
	Util::Io::ByteArrayInputStream bs((uint8_t*)s, 0);
	bs.disableSizeLimit();
	bs.makeNullTerminated();
	return _stream_vscanf(bs, format, args);
}

int scanf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vscanf(format, args);
	va_end(args);
	return ret;
}

int fscanf(FILE * stream, const char * format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vfscanf(stream, format, args);
	va_end(args);
	return ret;
}

int sscanf(const char * s, const char * format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vsscanf(s, format, args);
	va_end(args);
	return ret;
}