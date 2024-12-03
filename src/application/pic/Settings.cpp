//
// Created by Rafael Reip on 02.12.24.
//

#include "Settings.h"

Settings::Settings() {
    // TODO load and save settings from/to file
    this->checkeredBackground = true;
    this->optimizeRendering = true;
    this->currentLayerOverlay = true;
    this->activateHotkeys = true;
    this->showFPS = false;
    this->textCaptureAfterUse = false;
    this->resetValuesAfterConfirm = false;
    this->useBufferedBuffer = false;
}
