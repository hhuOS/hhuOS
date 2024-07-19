#include "lib/libc/ctype.h"
#include "lib/util/base/CharacterTypes.h"


int isalnum(int c) {
	return Util::CharacterTypes::isAlphaNumeric(c);
}

int isalpha(int c) {
	return Util::CharacterTypes::isAlphabet(c);
}

int isdigit(int c) {
	return Util::CharacterTypes::isDigit(c);
}

int islower(int c) {
	return Util::CharacterTypes::isLower(c);
}

int isupper(int c) {
	return Util::CharacterTypes::isUpper(c);
}

int isxdigit(int c) {
	return Util::CharacterTypes::isHexDigit(c);
}

int iscntrl(int c) {
	return Util::CharacterTypes::isControl(c);
}

int isgraph(int c) {
	return Util::CharacterTypes::isGraphical(c);
}

int isspace(int c) {
	return Util::CharacterTypes::isWhitespace(c);
}

int isblank(int c) {
	return Util::CharacterTypes::isBlank(c);
}

int isprint(int c) {
	return Util::CharacterTypes::isPrintable(c);
}

int ispunct(int c) {
	return Util::CharacterTypes::isPunctuation(c);
}

int tolower(int c) {
	return isupper(c) ? c + 32 : c;
}
int toupper(int c) {
	return islower(c) ? c - 32 : c;
}