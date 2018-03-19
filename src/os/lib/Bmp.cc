#include <kernel/services/GraphicsService.h>
#include "kernel/Kernel.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "lib/Bmp.h"

Bmp::Bmp (File* file) {
    byte info[54];
    byte * row;
    int row_padded;
    
    //fread(info, sizeof(byte), 54, f);
    file->readBytes((char *) info, 54);
    
    width     = *( (int*) &info[18]);
    height    = *( (int*) &info[22]);
    int psize = *( (int*) &info[34]);
    
    data = new Pixel[width*height];
    
    row_padded = (psize/height);
    row = new byte[row_padded];
    
    /** sometimes the fileheader is bigger?!?! */
    //fread(row, sizeof(byte), 68, f);
    file->readBytes((char *) row, 68);
    
    for(int y = height-1; y >= 0; --y) {
        
        //fread(row, sizeof(byte), row_padded, f);
        file->readBytes((char *) row, row_padded);
        
        for(int x = 0; x < width; ++x) {
            B(x, y) = row[(3*x)];
            G(x, y) = row[(3*x)+1];
            R(x, y) = row[(3*x)+2];
        }
    }
    
    delete row;
}

Bmp::~Bmp () {
    delete data;
}

void Bmp::print (int xpos, int ypos) {
    LinearFrameBuffer *lfb = ((GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME))->getLinearFrameBuffer();

    for(int y = height-1; y >= 0; --y) {
        for(int x = 0; x < width; ++x) {
            lfb->drawPixel(
                x+xpos, y+ypos,
                Color(R(x, y), G(x, y), B(x, y))
            );
        }
    }
}
