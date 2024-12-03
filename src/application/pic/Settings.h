//
// Created by Rafael Reip on 02.12.24.
//

#ifndef HHUOS_SETTINGS_H
#define HHUOS_SETTINGS_H


class Settings {
public:
    Settings();

    bool checkeredBackground;
    bool optimizeRendering;
    bool currentLayerOverlay;
    bool activateHotkeys;
    bool showFPS;
    bool textCaptureAfterUse;
    bool resetValuesAfterConfirm;
    bool useBufferedBuffer;
};


#endif //HHUOS_SETTINGS_H
