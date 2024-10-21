#ifndef HELPER_H
#define HELPER_H
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-macro-parentheses"

#include <cstdint>

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush

uint32_t blendPixels(uint32_t lower, uint32_t upper);

const char *int_to_string(int value);

const char *double_to_string(double value, int decimal_places);

#pragma clang diagnostic pop
#endif // HELPER_H

