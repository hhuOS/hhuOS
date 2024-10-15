#include "lib/libc/stdio.h"

#include "lib/libc/ctype.h"
#include "lib/libc/stdlib.h"
#include "lib/libc/math.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/System.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"

#include <stdarg.h>
#include <stddef.h>



int _stream_vprintf(Util::Io::OutputStream &os, const char* format, va_list args) {
	Util::Io::PrintStream out(os);
	const char * c = format;
	
	while (*c != '\0') {
		if (*c != '%') {
			out.write(*c);
		} else {
			
			bool shortParam = false;
			bool longParam = false;
			bool alternateConversion = false;
			bool zeroPadding = false;
			
			long int precision = -1;
			long int padding = -1;
			
			out.setBase(10);
			out.setNumberPadding(0);
			out.setIntegerPrecision(0);
			out.setDecimalPrecision(-1);
			out.setNumberJustification(false);
			out.setPositiveSign('\0');
			out.setNegativeSign('-');
			out.setIntegerPrefix("");
			out.setAlwaysPrintDecimalPoint(false);
			
			//parse conversion specification flags
			while (1) {
				c++;
				
				if (*c == '%') break;
				else if (*c == 'h') shortParam = true;
				else if (*c == 'l' || *c == 'L') longParam = true;
				else if (*c == '-') out.setNumberJustification(true);
				else if (*c == '+') out.setPositiveSign('+');
				else if (*c == ' ') out.setPositiveSign(' ');
				else if (*c == '#') alternateConversion = true;
				else if (*c == '0') zeroPadding = true;
				else if (isdigit(*c)) {
					padding = strtol(c, (char**)&c, 10);
					c--;
				} else if (*c == '*') padding = va_arg(args, int);
				else if (*c == '.') {
					c++;
					if (*c == '*') {
						precision = va_arg(args, int);
					} else if (isdigit(*c)) {
						precision = strtol(c, (char**)&c, 10);
						c--;
					} else {
						precision = 0;
					}
				}
				else break;
			}
			
			char mbBuffer[4];
			unsigned long int val = 0;
			double dval;
			int exp;
			bool scientificNotation;
			void * ptr;
			//Execute conversion format specifier
			switch (*c) {
				case '%':
					out.write('%');
					break;
					
				case 'n':
					ptr = va_arg(args, void*);
					if (longParam) {
						*(long*)ptr = out.getBytesWritten();
					} else if (shortParam) {
						*(short*)ptr = out.getBytesWritten();
					} else {
						*(int*)ptr = out.getBytesWritten();
					}
					break;
				
				case 'c':
					if (!longParam) {
						//print char
						out.write(va_arg(args, int));
					}  else {
						//print wchar
						int mbLen = wctomb(mbBuffer, (wchar_t)va_arg(args, int));
						out.write((const uint8_t*)mbBuffer, 0, mbLen);
					}
					break;
					
				
				case 's':
					if (!longParam) {
						//print string
						out.print(va_arg(args, char *), precision);
					} else {
						//print wide string
						wchar_t * wstr = va_arg(args, wchar_t*);
						while (1) {
							int mbLen = wctomb(mbBuffer, *(wstr++));
							if (mbBuffer[0] == 0) break;
							out.write((const uint8_t*)mbBuffer, 0, mbLen);
						}
					}
					break;
					
				case 'd':
				case 'i':
				case 'u':
				case 'o':
				case 'x':
				case 'X':
				case 'p':
					if (*c == 'p') {
						precision = 8; //write out all bytes of the adress
					}
				
					if (zeroPadding) {
						precision += padding;
						padding = 0;
					}
					if (padding>=0) out.setNumberPadding(padding);
					if (precision>=0) out.setIntegerPrecision(precision);
					
					
					if (*c == 'o') { //octal
						out.setBase(8);
						if (alternateConversion) out.setIntegerPrefix("0");
						
					} else if (*c == 'X') { //upper hex
						out.setHexNumericBase('A');
						if (alternateConversion) out.setIntegerPrefix("0X");
						out.setBase(16);
						
					} else if (*c == 'x' || *c == 'p') { //lower hex
						out.setHexNumericBase('a');
						if (alternateConversion) out.setIntegerPrefix("0x");
						out.setBase(16);
						
					} else { //decimal
						out.setBase(10);
					}
					
					val = va_arg(args, unsigned long int);
					if (shortParam) val &= 0xffff;
					
					if (precision == 0 && val == 0) {
						if (*c == 'o' && alternateConversion) out.write('0');
						break; //no print if no precision and 0
					}
					
					if (*c == 'd' || *c == 'i') out.print((int32_t)val);
					else out.print((uint32_t)val);
					
					break;
					
				case 'f':
				case 'F':
				case 'e':
				case 'E':
				case 'g':
				case 'G':
					if (padding>=0) out.setNumberPadding(padding);
					if (precision>=0) out.setDecimalPrecision(precision);
					else out.setDecimalPrecision(6);
					
					
					if (alternateConversion) out.setAlwaysPrintDecimalPoint(true);
					dval = va_arg(args, double);
					
					if (isnan(dval)) {
						out.print(isupper(*c) ? "NAN" : "nan");
						break;
					}
					if (isinf(dval)) {
						if (val > 0) {
							out.print(isupper(*c) ? "INF" : "inf");
						} else {
							out.print(isupper(*c) ? "-INF" : "INF");
						}
						break;
					}
					
					scientificNotation = false;
					
					exp = floor(log10(fabs(dval))); //calculate exponent
					
					if (*c == 'e' || *c == 'E') scientificNotation = true;
					
					if (*c == 'g' || *c == 'G')  {
						if (precision == -1) precision = 6;
						if (precision == 0) precision = 1;
						if (precision > exp && exp >= -4) {
							out.setDecimalPrecision(precision - exp - 1);
							scientificNotation = false;
						} else {
							scientificNotation = true;
							out.setDecimalPrecision(precision - 1);
						}
					}
					
					if (!scientificNotation) {
						out.print(dval);
						
					} else {
						out.print(dval / pow(10, exp));
						out.write(islower(*c) ? 'e' : 'E');
						out.print((int32_t)exp);
					}
					break;
					
				default:
					return -1;
				}
			}
			c++;
		}
		
		return out.getBytesWritten();
	}
	


int vfprintf(FILE * stream, const char * format, va_list vlist) {
	return _stream_vprintf(*stream, format, vlist);
}

int vprintf(const char * format, va_list vlist) {
	return vfprintf(stdout, format, vlist);
}

int vsprintf(char * buffer, const char * format, va_list vlist) {
	Util::Io::ByteArrayOutputStream os((uint8_t*)buffer, -1);
	int ret = _stream_vprintf(os, format, vlist);
	os.write('\0');
	return ret;
}

int vsnprintf(char * buffer, size_t bufsz, const char * format, va_list vlist) {
	Util::Io::ByteArrayOutputStream os((uint8_t*)buffer, bufsz);
	os.setEnforceSizeLimit(true);
	int ret = _stream_vprintf(os, format, vlist);
	os.write('\0');
	return ret;
}

int printf(const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vprintf(format, list);
	va_end(list);
	return ret;
}

int fprintf(FILE * stream, const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vfprintf(stream, format, list);
	va_end(list);
	return ret;
}

int sprintf(char * buffer, const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vsprintf(buffer, format, list);
	va_end(list);
	return ret;
}

int snprintf(char * buffer, size_t bufsz, const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vsnprintf(buffer, bufsz, format, list);
	va_end(list);
	return ret;
}
	
