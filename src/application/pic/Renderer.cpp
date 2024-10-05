//
// Created by Rafael Reip on 04.10.24.
//

#include "Renderer.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush


Renderer::Renderer(Layer **layers, int layerCount, Util::Graphic::LinearFrameBuffer *lfb, MouseInfo *mouse) {
    this->layers = layers;
    this->layerCount = layerCount;
    this->lfb = lfb;
    this->buffer = reinterpret_cast<uint32_t *>(lfb->getBuffer().get());
    this->screenX = lfb->getResolutionX();
    this->screenY = lfb->getResolutionY();
    this->pitch = lfb->getPitch();
    this->base = new uint32_t[screenX * screenY];
    this->mouse = mouse;
    prepareBase();

}

void Renderer::run() {

    while (true) {

        for (int j = 0; j < screenY; j++) {
            for (int i = 0; i < screenX; i++) {
                int offset = i + j * (pitch / 4);
                reinterpret_cast<uint32_t *>(buffer)[offset] = base[offset];

                for (int k = 0; k < layerCount; k++) {
                    Layer *item = layers[k];
                    uint32_t *data = item->getPixelData();

                    int relX = i - 200 - item->getPosX();
                    int relY = j - item->getPosY();
                    if (relX < 0 || relY < 0 || relX >= item->getWidth() || relY >= item->getHeight()) {
                        continue;
                    }

                    reinterpret_cast<uint32_t *>(buffer)[offset] = data[relX + relY * item->getWidth()];

                }

                if (abs(i - mouse->X) < 10 && abs(j - mouse->Y) < 10) {
                    reinterpret_cast<uint32_t *>(buffer)[offset] = mouse->leftButtonPressed ? 0xFFFF0000 : 0xFF00FF00;
                }
            }
        }
//        lfb->flush();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

}

void Renderer::prepareBase() {
    const int squareSize = 10;
    const uint32_t lightGray = 0xFFC0C0C0, darkGray = 0xFF404040, Gray = 0xFF808080;
    for (int y = 0; y < screenY; y++) {
        for (int x = 0; x < 200; x++) { // Gray for GUI
            int offset = x + y * (pitch / 4);
            base[offset] = Gray;
        }
        bool light = (y / squareSize) % 2;
        for (int x = 200; x < screenX;) { // Alternating squares for Background
            for (int k = 0; k < 10 && x < screenX; k++, x++) {
                int offset = x + y * (pitch / 4);
                base[offset] = light ? lightGray : darkGray;
            }
            light = !light;
        }
    }
}
