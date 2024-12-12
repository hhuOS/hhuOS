#ifndef PIC_H
#define PIC_H

namespace Util::Io {
    class File;
}

class DataWrapper;

class Renderer;

class Pic {
public:
    explicit Pic(Util::Io::File *lfbFile);

    ~Pic();

    void run();

private:
    DataWrapper *data;
    Renderer *renderer;

    void init_gui();

    void checkMouseInput();

    void checkKeyboardInput();

    void parseMouse(bool clicked) const;
};

#endif // PIC_H
