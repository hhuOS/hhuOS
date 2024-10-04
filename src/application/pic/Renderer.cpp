//
// Created by Rafael Reip on 04.10.24.
//

#include "Renderer.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush


Renderer::Renderer(Layer **layers, int layerCount) {
    this->layers = layers;
    this->layerCount = layerCount;
    auto lfbFile = Util::Io::File("/device/lfb");
    lfb = new Util::Graphic::LinearFrameBuffer(lfbFile);
//    blfb = new Util::Graphic::BufferedLinearFrameBuffer(lfb, true);
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    buffer = reinterpret_cast<uint32_t *>(lfb->getBuffer().get());
    screenX = lfb->getResolutionX();
    screenY = lfb->getResolutionY();
    pitch = lfb->getPitch();

}

void Renderer::run() {

    while (true) {

        for (int j = 0; j < screenY; j++) {
            for (int i = 0; i < screenX; i++) {
                int offset = i + j * (pitch / 4);

                for (int k = 0; k < layerCount; k++) {
                    Layer *item = layers[k];
                    uint32_t * data = item->getPixelData();

                    int relX = i - item->getPosX();
                    int relY = j - item->getPosY();
                    if (relX < 0 || relY < 0 || relX >= item->getWidth() || relY >= item->getHeight()) {
                        continue;
                    }

                    reinterpret_cast<uint32_t *>(buffer)[offset] = data[relX + relY * item->getWidth()];

                }
            }
        }
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1000));
    }

}
