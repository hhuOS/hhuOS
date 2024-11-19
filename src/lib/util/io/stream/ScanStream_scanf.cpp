#include <stdarg.h>
#include <stdint.h>

 #include "ScanStream.h"
  #include "ByteArrayInputStream.h"
 #include "lib/util/base/WideChar.h"
  #include "lib/util/base/CharacterTypes.h"
 
 #define EOF -1
 
 
 wchar_t readWchar(Util::Io::ScanStream * ss) {
	uint8_t wc[4];
	wc[1] = 0;
	wc[2] = 0;
	wc[3] = 0;
	
	wc[0] = ss->read();
	for (int i=1; i<4; i++) {
		if (Util::WideChar::utf8Length( (const char * )wc, 4) != -1) break;
		wc[i] = ss->read();
	}
	
	if (Util::WideChar::utf8Length( (const char * )wc, 4) == -1) {
		//errno = EILSEQ;
		//TODO: Find solution for errno in lib/util
	}
	
	wchar_t ret;
	Util::WideChar::utf8ToWchar(&ret, (const char * )wc, 4);
	return ret;
}

 long scanf_strtol(const char* str, char **str_end, int base) {
	Util::Io::ByteArrayInputStream is((uint8_t*) str, 0);
	is.disableSizeLimit();
	Util::Io::ScanStream ss(is);
	
	long ret = ss.readInt(base);
	if (str_end) *str_end = (char*)(str + ss.getReadBytes());
	
	return ret;
}
 
namespace Util::Io {
	
int ScanStream::vscanf(const char* format, va_list vlist) {
	int ret = 0;
	//errno = 0; 
	//TODO: Find solution for errno in lib/util
	
	for (const char * c = format; *c != '\0' && this->peek() != EOF && this->peek() != '\0'; c++) {
		if (*c != '%') {
			
			if (!Util::CharacterTypes::isWhitespace(*c)) {
				//non-whitespace chars must match exactly
				if (this->peek() == *c) {
					this->read();
				} else {
					return EOF;
				}
				
			} else {
				//whitespace consumes all whitespace chars
				while (Util::CharacterTypes::isWhitespace(this->peek())) this->read();
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
				else if (Util::CharacterTypes::isDigit(*c)) {
					maxFieldWidth = scanf_strtol(c, (char**)&c, 10);
					c--;
				} 
				else if (*c == '*') suppressAssign = true;
				else if (*c == '\0') return EOF; 
				else break;
			}
			
			if (maxFieldWidth >= 0) {
				this->setReadLimit(this->getReadBytes() + maxFieldWidth);
			} else {
				this->setReadLimit(-1);
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
					if (this->peek() != '%' ) return EOF;
					this->read();
					break;
					
				case '\0':
					return EOF;
					
				case 'd':
					v = this->readInt(10);
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
					v = this->readInt();
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
					v2 = this->readUint(10);
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
					v2 = this->readUint(8);
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
					v2 = this->readUint(16);
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
						*(va_arg(vlist, unsigned long int*)) = this->getReadBytes();
					} else if (shortParam) {
						*(va_arg(vlist, unsigned short int*)) = this->getReadBytes();
					} else {
						*(va_arg(vlist, unsigned int*)) = this->getReadBytes();
					}
					break;
					
				case 'e':
				case 'E':
				case 'g':
				case 'G':
					d = this->readDouble();
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
					p =  (void*)this->readUint(16);
					if (!suppressAssign) *(va_arg(vlist,void**)) = p;
					break;
				
				case 'c': //match fixed length of chars 
					if (maxFieldWidth < 0) maxFieldWidth = 1;
					if (longParam) {
						if (!suppressAssign) ws = va_arg(vlist, wchar_t*);
						for (i=0; i<maxFieldWidth; i++, ws++) {
							if (!suppressAssign) {
								*ws = readWchar(this);
							} else {
								readWchar(this);
							} 
						}
					} else {
						if (!suppressAssign) s = va_arg(vlist, char*);
						for (i=0; i<maxFieldWidth; i++, s++) {
							if (!suppressAssign) {
								*s = this->read();
							} else {
								this->read();
							} 
						}
					}
					break;
				
				case 's': //string match until next wide char
					if (longParam) { //wide char
						if (!suppressAssign) ws = va_arg(vlist, wchar_t*);
						i = 0;
						c2 = this->peek();
						while (c2 != '\0' && !Util::CharacterTypes::isWhitespace(c2)) {
							if (maxFieldWidth >= 0 && i >= maxFieldWidth) break;
							w = readWchar(this);
							if (!suppressAssign) {
								*ws = w;
								ws++;
							}
							i++;
							c2 = this->peek();
						}
						
						if (!suppressAssign) *ws = '\0';
						
					} else { //normal char
						if (!suppressAssign) s = va_arg(vlist, char*);
						i = 0;
						c2 = this->peek();
						
						while (!Util::CharacterTypes::isWhitespace(c2)) {
							if (maxFieldWidth >= 0 && i >= maxFieldWidth) break;
							this->read();
							if (!suppressAssign) { 
								*s = c2;
								s++;
							}
							c2 = this->peek();
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
						c2 = this->peek();
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
								*s = this->read();
							} else {
								this->read();
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
			this->setReadLimit(-1);
		}
		
	}
	
	return ret;
}

int ScanStream::scanf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vscanf(format, args);
	va_end(args);
	return ret;
}
	
}