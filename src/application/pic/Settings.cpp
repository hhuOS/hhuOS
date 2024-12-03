//
// Created by Rafael Reip on 02.12.24.
//

#include "Settings.h"

Settings::Settings(MessageHandler *mHandler) {
    // TODO load and save settings from/to file
    this->mHandler = mHandler;
    this->path = "/pic/settings";

    resetToDefault();
    loadFromFile();
    saveToFile();
}

void Settings::resetToDefault() {
    checkeredBackground = true;
    optimizeRendering = true;
    currentLayerOverlay = true;
    activateHotkeys = true;
    showFPS = false;
    textCaptureAfterUse = false;
    resetValuesAfterConfirm = false;
    useBufferedBuffer = false;
}

void Settings::loadFromFile() {
    if (Util::String(path).length() == 0) {
        mHandler->addMessage("Settings Error: No path given");
        return;
    }
    auto file = Util::Io::File(path);
    if (!file.exists()) {
        mHandler->addMessage("Settings Error: File not found: " + path);
        return;
    }
    if (file.isDirectory()) {
        mHandler->addMessage("Settings Error: File is a directory: " + path);
        return;
    }

    auto fileStream = Util::Io::FileInputStream(file);
    auto bufferedStream = Util::Io::BufferedInputStream(fileStream);
    auto &stream = (file.getType() == Util::Io::File::REGULAR) ? static_cast<Util::Io::InputStream &>(bufferedStream)
                                                               : static_cast<Util::Io::InputStream &>(fileStream);
    bool eof = false;
    auto line = stream.readLine(eof);
    while (!eof) {
        if (line.length() > 0) {
            auto parts = line.split(" ");
            if (parts[0] == "checkeredBackground") checkeredBackground = parts[1] == "true";
            else if (parts[0] == "optimizeRendering") optimizeRendering = parts[1] == "true";
            else if (parts[0] == "currentLayerOverlay") currentLayerOverlay = parts[1] == "true";
            else if (parts[0] == "activateHotkeys") activateHotkeys = parts[1] == "true";
            else if (parts[0] == "showFPS") showFPS = parts[1] == "true";
            else if (parts[0] == "textCaptureAfterUse") textCaptureAfterUse = parts[1] == "true";
            else if (parts[0] == "resetValuesAfterConfirm") resetValuesAfterConfirm = parts[1] == "true";
            else if (parts[0] == "useBufferedBuffer") useBufferedBuffer = parts[1] == "true";
            else mHandler->addMessage("Settings Error: Unknown setting: " + parts[0]);
        }
        line = stream.readLine(eof);
    }

}

void Settings::saveToFile() {
    if (Util::String(path).length() == 0) {
        mHandler->addMessage("Settings Error: No path given");
        return;
    }
    auto picfolder = Util::Io::File("pic");
    if (!picfolder.exists()) {
        auto success = picfolder.create(Util::Io::File::DIRECTORY);
        if (!success) {
            mHandler->addMessage("Settings Error: Could not create directory: pic");
            return;
        }
    }

    FILE *file = fopen(path.operator const char *(), "w");
    if (!file) {
        mHandler->addMessage("Settings Error: Could not open file: " + path);
        return;
    }
    fputs(Util::String::format("checkeredBackground %s\n", checkeredBackground ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("optimizeRendering %s\n", optimizeRendering ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("currentLayerOverlay %s\n", currentLayerOverlay ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("activateHotkeys %s\n", activateHotkeys ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("showFPS %s\n", showFPS ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("textCaptureAfterUse %s\n", textCaptureAfterUse ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("resetValuesAfterConfirm %s\n", resetValuesAfterConfirm ? "true" : "false").operator const char *(), file);
    fputs(Util::String::format("useBufferedBuffer %s\n", useBufferedBuffer ? "true" : "false").operator const char *(), file);
    fflush(file);
    fclose(file);
}
