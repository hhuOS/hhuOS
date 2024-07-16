#ifndef HHUOS_CHARACTERTYPES_H
#define HHUOS_CHARACTERTYPES_H


namespace Util {
	
class CharacterTypes {
	public:
		static bool isAlphaNumeric(int c);
		static bool isAlphabet(int c);
		static bool isLower(int c);
		static bool isUpper(int c);
		static bool isDigit(int c);
		static bool isHexDigit(int c);
		static bool isControl(int c);
		static bool isGraphical(int c);
		static bool isWhitespace(int c);
		static bool isBlank(int c);
		static bool isPrintable(int c);
		static bool isPunctuation(int c);
};

}


#endif