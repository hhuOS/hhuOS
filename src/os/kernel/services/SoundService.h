//
// Created by burak on 05.03.18.
//

#ifndef HHUOS_SOUNDSERVICE_H
#define HHUOS_SOUNDSERVICE_H


#include <kernel/KernelService.h>
#include <devices/Speaker.h>

class SoundService : public KernelService {

private:
    Speaker *speaker;


public:
    SoundService();

    static constexpr char* SERVICE_NAME = "SoundService";

    Speaker* getSpeaker();

};


#endif //HHUOS_SOUNDSERVICE_H
