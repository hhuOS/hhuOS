 #include "CharacterTypes.h"
 
 
 namespace Util {
 
 bool CharacterTypes::isAlphaNumeric(int c) {
	 return (c>=48 && c<=57) || (c>=65 && c<=90) || (c>=97 && c<=122);
 }
 
 bool CharacterTypes::isAlphabet(int c) {
	 return (c>=65 && c<=90) || (c>=97 && c<=122);
 }
 
 bool CharacterTypes::isLower(int c) {
	 return (c>=97 && c<=122);
 }
 
 bool CharacterTypes::isUpper(int c) {
	 return (c>=65 && c<=90);
 }
 
 bool CharacterTypes::isDigit(int c) {
	 return (c>=48 && c<=57);
 }
 
 bool CharacterTypes::isHexDigit(int c) {
	 return (c>=48 && c<=57) || (c>=65 && c<=70) || (c>=97 && c<=102);
 }
 
 bool CharacterTypes::isControl(int c) {
	 return (c>=0 && c<=31) || c==127;
 }
 
 bool CharacterTypes::isGraphical(int c) {
	 return c>=33 && c <=126;
 }
 
 bool CharacterTypes::isWhitespace(int c) {
	 return (c>=9 && c <=13) || c==32;
 }
 
 bool CharacterTypes::isBlank(int c) {
	 return c==9 || c==32;
 }
 
 bool CharacterTypes::isPrintable(int c) {
	 return c>=32 && c<=126;
 }
 
 bool CharacterTypes::isPunctuation(int c) {
	 return (c>=33 && c<=47) || (c>=58 && c<=64) || (c>=91 && c<=96) || (c>=123 && c<=126);
 }
 
 }