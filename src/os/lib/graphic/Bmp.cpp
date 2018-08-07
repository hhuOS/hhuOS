#include <kernel/services/GraphicsService.h>
#include "kernel/Kernel.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "lib/graphic/Bmp.h"

Bmp::Bmp (File* file) {
    uint8_t info[54];
    uint8_t* row;
    int row_padded;
    
    //fread(info, sizeof(byte), 54, f);
    file->readBytes((char *) info, 54);

    uint32_t offset = *( (uint32_t*) &info[10]);
    width     = *( (int*) &info[18]);
    height    = *( (int*) &info[22]);
    int psize = *( (int*) &info[34]);
    
    data = new Pixel[width*height];
    
    row_padded = (psize/height);
    row = new uint8_t[row_padded];
    
    file->setPos(offset);
    
    for(int y = height-1; y >= 0; --y) {
        file->readBytes((char *) row, row_padded);
        
        for(int x = 0; x < width; ++x) {
            B(x, y) = row[(4*x)];
            G(x, y) = row[(4*x)+1];
            R(x, y) = row[(4*x)+2];
            A(x, y) = row[(4*x)+3];
        }
    }
    
    delete row;
}

Bmp::~Bmp () {
    delete data;
}

void Bmp::print (int xpos, int ypos) {
    LinearFrameBuffer *lfb = (Kernel::getService<GraphicsService>())->getLinearFrameBuffer();

    for(int y = height-1; y >= 0; --y) {
        for(int x = 0; x < width; ++x) {
            lfb->drawPixel(
                x+xpos, y+ypos,
                Color(R(x, y), G(x, y), B(x, y), A(x,y))
            );
        }
    }
}
