//
// Created by burak on 05.03.18.
//

#include "SoundService.h"

SoundService::SoundService() {
    speaker = new Speaker();
}

Speaker* SoundService::getSpeaker() {
    return speaker;
}