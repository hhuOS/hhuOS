#include "SoundControlHelper.h"
#include "lib/util/math/Math.h"

const constexpr int16_t SILENCE           = 0x8000;
const constexpr int16_t LOWER_BOUND_SOUND = 0x8001;
const constexpr int16_t UPPER_BOUND_SOUND = 0x7FFF;
const constexpr double  LOWER_BOUND_DB    = -127.9961; 
const constexpr double  UPPER_BOUND_DB    = 127.9961;
const constexpr double  DB_STEP           = 0.00390625;

double convert_to_dB(int16_t sound_value){
    if(sound_value > UPPER_BOUND_SOUND || sound_value < LOWER_BOUND_SOUND) return 0;
    if(sound_value < 0){
        uint32_t abs  = Util::Math::absolute(LOWER_BOUND_SOUND - sound_value);
        uint16_t diff = abs / 0x0001;
        return LOWER_BOUND_DB + (diff * DB_STEP);
    }
    uint16_t diff = sound_value / 0x0001;
    return diff * DB_STEP;
}

int16_t convert_to_sound_value(double dB){
    if(dB < LOWER_BOUND_DB || dB > UPPER_BOUND_DB) return 0;
    if(dB < 0){
        double abs     = Util::Math::absolute(LOWER_BOUND_DB - dB);
        uint16_t diff  = abs / DB_STEP;
        return LOWER_BOUND_SOUND + diff;
    }
    uint16_t diff = dB / DB_STEP;
    return diff * 0x0001;
}

double get_upper_bound_dB(){
    return UPPER_BOUND_DB;
}

double get_lower_bound_dB(){
    return LOWER_BOUND_DB;
}

int16_t get_upper_bound_sound(){
    return UPPER_BOUND_SOUND;
}

int16_t get_lower_bound_sound(){
    return LOWER_BOUND_SOUND;
}

int16_t get_silence(){
    return SILENCE;
}