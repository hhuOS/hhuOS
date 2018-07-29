#ifndef __BeepFile_include__
#define __BeepFile_include__

#include <devices/Speaker.h>

class BeepFile {

    struct Sound {
        float frequency;
        uint32_t length;

        bool operator!=(const Sound &other) const;
        bool operator==(const Sound &other) const;
    };

    struct Content {
        uint32_t magic;
        uint32_t length;
        Sound sounds[];
    };

public:

    static BeepFile* load(const String &path);

    BeepFile(const BeepFile &other) = delete;

    BeepFile &operator=(const BeepFile &other) = delete;

    ~BeepFile() = default;

    void play();

private:

    explicit BeepFile(const Content &content);

    const Content &content;

    Speaker *speaker = nullptr;

    static constexpr const uint32_t MAGIC = 0x42454550;

};


#endif
