/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_LIB_PULSAR_GAME_H
#define HHUOS_LIB_PULSAR_GAME_H

#include "audio/AudioChannel.h"
#include "util/collection/ArrayListQueue.h"
#include "pulsar/Scene.h"

namespace Pulsar {

/// The main game class, which holds the queue of scenes and manages audio playback.
/// A global instance of this class is created when the `Engine` is initialized.
/// It can be used to push new scenes, switch between scenes, and play audio tracks.
///
/// Scenes are managed in a first-in-last-out (FILO) queue, allowing for easy scene transitions.
/// All scenes must be heaped-allocated and the game instance takes ownership of them,
/// once they are pushed to the queue. This means that the game instance is responsible for deleting
/// the scenes when they are no longer needed.
///
/// The game instance also manages audio playback through a set of audio channels.
/// Audio tracks can be played on these channels, with support for looping playback.
/// There are a fixed number of 8 audio channels available for simultaneous playback.
class Game {

public:
    /// There should only be one instance of the Game class per game, so the copy constructor is deleted.
    Game(const Game &other) = delete;

    /// There should only be one instance of the Game class per game, so the assignment operator is deleted.
    Game &operator=(const Game &other) = delete;

    /// Destroy the game instance, cleaning up and deleting all scenes.
    ~Game();

    /// Get the global instance of the Game class.
    /// The global instance is created when the `Engine` is initialized.
    /// Calling this function before creating an engine instance will fire a panic.
    static Game& getInstance();

    /// Stop the game loop, causing the engine to exit.
    /// This method should be called to gracefully terminate the game.
    void stop();

    /// Check if the game is currently running.
    /// This method returns true while the game loop is active and false before it starts or after it has been stopped.
    [[nodiscard]] bool isRunning() const;

    /// Get the currently active scene.
    [[nodiscard]] Scene& getCurrentScene() const;

    /// Push a new scene onto the scene stack.
    /// The scene must be heaped-allocated and the game instance will take ownership of it.
    /// This means that the game instance is responsible for deleting the scene when it is no longer needed.
    void pushScene(Scene *scene);

    /// Switch to the next scene in the queue.
    /// The current scene will be removed from the queue and deleted.
    void switchToNextScene();

    /// Play an audio track on an available audio channel.
    /// The returned audio handle can be used to control playback (e.g., stop the track).
    /// If all audio channels are currently in use, the request will be ignored
    /// and an invalid AudioHandle will be returned.
    AudioHandle playAudioTrack(const AudioTrack &track, bool loop) const;

    /// Stop playback on all audio channels.
    /// This will immediately stop all currently playing audio tracks.
    /// The engine calls this method when switching scenes to ensure no audio from the previous scene continues playing.
    void stopAllAudioChannels() const;

    /// Get the screen transformation factor.
    /// This factor is used to scale from the game coordinate system to the actual screen resolution.
    /// The game uses a virtual coordinate system ranging from (-1, -1) to (1, 1) for the entire screen.
    /// The transformation factor can be multiplied to any coordinate in this system
    /// to get the corresponding pixel coordinate on the screen.
    /// The engine calculates the transformation factor by taking the smaller axis of the target screen resolution
    /// and dividing it by 2. For example, on a 800x600 screen, the transformation factor will be 300.
    [[nodiscard]] uint16_t getScreenTransformation() const;

    /// Get the dimensions of the game coordinate system.
    /// The game uses a virtual coordinate system ranging from (-1, -1) to (1, 1) for the entire screen.
    /// However, if the target screen resolution is not square,
    /// one axis will have a larger range to maintain the aspect ratio.
    /// For example, on a 800x600 screen, the coordinate system will range from (-1.33, -1) to (1.33, 1).
    [[nodiscard]] const Util::Math::Vector2<float>& getScreenDimensions() const;

private:

    friend class Engine;

    Game(uint16_t screenTransformation, const Util::Math::Vector2<float> &screenDimensions);

    void initializeNextScene(Graphics &graphics);

    static Game *instance;

    uint16_t screenTransformation;
    Util::Math::Vector2<float> screenDimensions;

    Util::ArrayListQueue<Scene*> scenes;
    bool firstScene = true;
    bool sceneSwitched = true;
    bool running = false;

    Util::Array<AudioChannel> audioChannels;

    static constexpr size_t AUDIO_CHANNELS = 8;
};

}

#endif
