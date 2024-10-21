 #include "WideChar.h"
 
 namespace Util {

int WideChar::utf8Length(const char* bytes, size_t maxLength) {
	if (!bytes) return 0;
	if ((*bytes) == '\0') return 0;
	if (maxLength==0) return -1;
	
	size_t len = 0;
	
	if (((*bytes) & 0b10000000) == 0b00000000) len = 1; //of type 0b0xxxxxxx
	else if (((*bytes) & 0b11100000) == 0b11000000) len=2; //of type 0b110xxxxx
	else if (((*bytes) & 0b11110000) == 0b11100000) len=3; //of type 0b1110xxxx
	else if (((*bytes) & 0b11111000) == 0b11110000) len=4; //of type 0b11110xxx
	
	if (len > maxLength) return -1;
	
	for (size_t i=1; i<len; i++) {
		if (((*(bytes + i)) & 0b11000000) != 0b10000000) return -1; //check that following bytes have 0x10xxxxxx form
	}
	
	return len;
}


int WideChar::utf8ToWchar(wchar_t * wideChar, const char* bytes, size_t maxLength) {
	size_t len = WideChar::utf8Length(bytes, maxLength);
	
	if ((*bytes) == '\0') return len; //same error handling as mblen
	if (!wideChar) return len;
	
	size_t first_byte_len = 0;
	switch (len) {
		case 1:
			(*wideChar) = (wchar_t)(*bytes);
			if ((*bytes) == 0) return 0;
			return len;
			
		case 2:
			first_byte_len = 5;
			break;
		case 3:
			first_byte_len = 4;
			break;
		case 4:
			first_byte_len = 3;
			break;
	}
	
	(*wideChar) = 0;
	size_t offset = 0;
	for (int i = len-1; i>0; i--, offset += 6) {
		(*wideChar) |= (((wchar_t)(*(bytes + i))) & 0b00111111) << offset; // assemble the values of the following bytes
	}
	
	(*wideChar) |= (((wchar_t)(*bytes)) & ((1 << first_byte_len) - 1)) << offset; // add first byte value
	return len;
}

int WideChar::wcharToUtf8(char * bytes, wchar_t wideChar) {
	if (wideChar >= 0x110000) return -1;
	if (!bytes) return 0;
	
	size_t len = 1;
	if (wideChar >= 0x10000) len = 4;
	else if (wideChar >= 0x800) len = 3;
	else if (wideChar >= 0x80) len=2;
	
	switch (len) {
		case 1:
			(*bytes) = (char)wideChar;
			return len;
			
		case 2:
			(*bytes) = 0b11000000 | ((wideChar >> 6) & 0b00011111);
			(*(bytes+1)) = 0b10000000 | (wideChar & 0b00111111);
			return 2;
		case 3:
			(*bytes) = 0b11100000 | ((wideChar >> 12) & 0b1111);
			(*(bytes+1)) = 0b10000000 | ((wideChar >> 6) & 0b111111);
			(*(bytes+2)) = 0b10000000 | (wideChar & 0b111111);
			return 3;
		case 4:
			(*bytes) = 0b11110000 | ((wideChar >> 18) & 0b111);
			(*(bytes+1)) = 0b10000000 | ((wideChar >> 12) & 0b111111);
			(*(bytes+2)) = 0b10000000 | ((wideChar >> 6) & 0b111111);
			(*(bytes+3)) = 0b10000000 | (wideChar & 0b111111);		
			return 4;
	}
	return -1;
}

}