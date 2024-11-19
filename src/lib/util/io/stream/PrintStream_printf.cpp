#include <stdarg.h>
#include <stdint.h>

#include "PrintStream.h"
#include "ByteArrayInputStream.h"
#include "ScanStream.h"
#include "lib/util/base/WideChar.h"
#include "lib/util/base/CharacterTypes.h"
#include "lib/util/math/Math.h"


long printf_strtol(const char* str, char **str_end, int base) {
    Util::Io::ByteArrayInputStream is((uint8_t*) str, 0);
    is.disableSizeLimit();
    Util::Io::ScanStream ss(is);

    long ret = ss.readInt(base);
    if (str_end) *str_end = (char*)(str + ss.getReadBytes());

    return ret;
}

namespace Util::Io {

int PrintStream::vprintf(const char* format, va_list args) {
    const char * c = format;

    while (*c != '\0') {
        if (*c != '%') {
            this->write(*c);
        } else {

            bool shortParam = false;
            bool longParam = false;
            bool alternateConversion = false;
            bool zeroPadding = false;

            long int precision = -1;
            long int padding = -1;

            this->setBase(10);
            this->setNumberPadding(0);
            this->setIntegerPrecision(0);
            this->setDecimalPrecision(-1);
            this->setNumberJustification(false);
            this->setPositiveSign('\0');
            this->setNegativeSign('-');
            this->setIntegerPrefix("");
            this->setAlwaysPrintDecimalPoint(false);

            //parse conversion specification flags
            while (1) {
                c++;

                if (*c == '%') break;
                else if (*c == 'h') shortParam = true;
                else if (*c == 'l' || *c == 'L') longParam = true;
                else if (*c == '-') this->setNumberJustification(true);
                else if (*c == '+') this->setPositiveSign('+');
                else if (*c == ' ') this->setPositiveSign(' ');
                else if (*c == '#') alternateConversion = true;
                else if (*c == '0') zeroPadding = true;
                else if (Util::CharacterTypes::isDigit(*c)) {
                    padding = printf_strtol(c, (char**)&c, 10);
                    c--;
                } else if (*c == '*') padding = va_arg(args, int);
                else if (*c == '.') {
                    c++;
                    if (*c == '*') {
                        precision = va_arg(args, int);
                    } else if (Util::CharacterTypes::isDigit(*c)) {
                        precision = printf_strtol(c, (char**)&c, 10);
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
                    this->write('%');
                    break;

                case 'n':
                    ptr = va_arg(args, void*);
                    if (longParam) {
                        *(long*)ptr = this->getBytesWritten();
                    } else if (shortParam) {
                        *(short*)ptr = this->getBytesWritten();
                    } else {
                        *(int*)ptr = this->getBytesWritten();
                    }
                    break;

                case 'c':
                    if (!longParam) {
                        //print char
                        this->write(va_arg(args, int));
                    }  else {
                        //print wchar
                        int mbLen = Util::WideChar::wcharToUtf8(mbBuffer, (wchar_t)va_arg(args, int));
                        this->write((const uint8_t*)mbBuffer, 0, mbLen);
                    }
                    break;


                case 's':
                    if (!longParam) {
                        //print string
                        this->print(va_arg(args, char *), precision);
                    } else {
                        //print wide string
                        wchar_t * wstr = va_arg(args, wchar_t*);
                        while (1) {
                            int mbLen = Util::WideChar::wcharToUtf8(mbBuffer, *(wstr++));
                            if (mbBuffer[0] == 0) break;
                            this->write((const uint8_t*)mbBuffer, 0, mbLen);
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
                        precision = 8; //write this all bytes of the adress
                    }

                    if (zeroPadding) {
                        precision += padding;
                        padding = 0;
                    }
                    if (padding>=0) this->setNumberPadding(padding);
                    if (precision>=0) this->setIntegerPrecision(precision);


                    if (*c == 'o') { //octal
                        this->setBase(8);
                        if (alternateConversion) this->setIntegerPrefix("0");

                    } else if (*c == 'X') { //upper hex
                        this->setHexNumericBase('A');
                        if (alternateConversion) this->setIntegerPrefix("0X");
                        this->setBase(16);

                    } else if (*c == 'x' || *c == 'p') { //lower hex
                        this->setHexNumericBase('a');
                        if (alternateConversion) this->setIntegerPrefix("0x");
                        this->setBase(16);

                    } else { //decimal
                        this->setBase(10);
                    }

                    val = va_arg(args, unsigned long int);
                    if (shortParam) val &= 0xffff;

                    if (precision == 0 && val == 0) {
                        if (*c == 'o' && alternateConversion) this->write('0');
                        break; //no print if no precision and 0
                    }

                    if (*c == 'd' || *c == 'i') this->print((int32_t)val);
                    else this->print((uint32_t)val);

                    break;

                case 'f':
                case 'F':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                    if (padding>=0) this->setNumberPadding(padding);
                    if (precision>=0) this->setDecimalPrecision(precision);
                    else this->setDecimalPrecision(6);


                    if (alternateConversion) this->setAlwaysPrintDecimalPoint(true);
                    dval = va_arg(args, double);

                    if (Util::Math::isNan(dval)) {
                        this->print(Util::CharacterTypes::isUpper(*c) ? "NAN" : "nan");
                        break;
                    }
                    if (Util::Math::isInfinity(dval)) {
                        if (val > 0) {
                            this->print(Util::CharacterTypes::isUpper(*c) ? "INF" : "inf");
                        } else {
                            this->print(Util::CharacterTypes::isUpper(*c) ? "-INF" : "INF");
                        }
                        break;
                    }

                    scientificNotation = false;

                    exp = Util::Math::floor(Util::Math::log10(Util::Math::absolute(dval))); //calculate exponent

                    if (*c == 'e' || *c == 'E') scientificNotation = true;

                    if (*c == 'g' || *c == 'G')  {
                        if (precision == -1) precision = 6;
                        if (precision == 0) precision = 1;
                        if (precision > exp && exp >= -4) {
                            this->setDecimalPrecision(precision - exp - 1);
                            scientificNotation = false;
                        } else {
                            scientificNotation = true;
                            this->setDecimalPrecision(precision - 1);
                        }
                    }

                    if (!scientificNotation) {
                        this->print(dval);

                    } else {
                        this->print(dval / Util::Math::pow(10.0, exp));
                        this->write(Util::CharacterTypes::isLower(*c) ? 'e' : 'E');
                        this->print((int32_t)exp);
                    }
                    break;

                default:
                    return -1;
            }
        }
        c++;
    }

    return this->getBytesWritten();
}

int PrintStream::printf(const char * format, ...) {
    va_list list;
    va_start(list, format);
    int ret = this->vprintf(format, list);
    va_end(list);
    return ret;
}

}