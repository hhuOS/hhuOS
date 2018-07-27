#ifndef __Hidi_include__
#define __Hidi_include__

#include <lib/file/File.h>
#include <devices/Speaker.h>

class Hidi {

    struct Sound {
        uint32_t frequency;
        uint32_t length;

        bool operator!=(const Sound &other) const;
        bool operator==(const Sound &other) const;
    };

public:

    static Hidi* load(const String &path);

    Hidi(const Hidi &other) = delete;

    Hidi &operator=(const Hidi &other) = delete;

    ~Hidi() = default;

    void play();

private:

    explicit Hidi(const Util::ArrayList<Sound> &buffer);

    const Util::ArrayList<Sound> *soundBuffer = nullptr;

    Speaker *speaker = nullptr;

};


#endif
