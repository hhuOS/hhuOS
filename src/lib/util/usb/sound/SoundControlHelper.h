#ifndef SOUND_CONTROL_HELPER__INCLUDE
#define SOUND_CONTROL_HELPER__INCLUDE

#include <cstdint>
#include "lib/util/base/String.h"

double convert_to_dB(int16_t sound_value);
int16_t convert_to_sound_value(double dB);

double get_upper_bound_dB();
double get_lower_bound_dB();

int16_t get_upper_bound_sound();
int16_t get_lower_bound_sound();

int16_t get_silence();

#endif