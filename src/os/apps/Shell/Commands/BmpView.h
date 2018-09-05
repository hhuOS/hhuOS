#ifndef HHUOS_BMPVIEW_H
#define HHUOS_BMPVIEW_H

#include <lib/file/bmp/Bmp.h>
#include "Command.h"

class BmpView : public Command, Receiver {

private:

    Bmp *bmp = nullptr;

    uint8_t initScalingDividor = 1;

    uint8_t scalingFactor = 1;
    uint8_t scalingDividor = 1;

    bool isRunning = true;

private:

    void scaleUp();

    void scaleDown();

    void drawBitmap();

public:
    /**
     * Default-constructor.
     */
    BmpView() = delete;

    /**
     * Copy-constructor.
     */
    BmpView(const BmpView &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit BmpView(Shell &shell);

    /**
     * Destructor.
     */
    ~BmpView() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;
};

#endif
