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
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_UTIL_GAME_AUDIOTRACK_H
#define HHUOS_LIB_UTIL_GAME_AUDIOTRACK_H

#include <stddef.h>

#include "async/IdGenerator.h"
#include "pulsar/audio/AudioBuffer.h"

namespace Util::Pulsar {

class AudioHandle;

/// An audio track represents a sound that can be played in the game.
/// It encapsulates an audio buffer (i.e. loaded from a WAV file) and provides
/// methods to play the sound, optionally in a loop.
/// Playback of an audio track returns an `AudioHandle` that can be used to control
/// the playback (e.g. stop it).
///
/// ## Example
/// ```c++
/// class TestScene final : public Util::Pulsar::D2::Scene {
///
/// public:
///
/// TestScene() = default;
///
/// // This method is called once when the scene is initialized.
/// // Here we load the audio track from a WAV file.
/// void initialize() override {
///     // Load background music
///     backgroundMusic = Util::Pulsar::AudioTrack("/user/bug/music.wav");
/// }
///
/// // This method is called periodically to update the scene.
/// // Here we start playing the background music if not already playing, and stop it after 60 seconds.
/// void update(const double delta) override {
///     // Start playing background music if not already playing
///     if (!backgroundMusicHandle.isPlaying()) {
///         backgroundMusicHandle = backgroundMusic.play(true);
///     }
///
///     backgroundMusicTimer += delta;
///
///     // Stop the music after 60 seconds
///     if (backgroundMusicTimer >= 60.0 && backgroundMusicHandle.isPlaying()) {
///         backgroundMusicHandle.stop();
///     }
/// }
///
/// void initializeBackground(Util::Pulsar::Graphics &graphics) override {
///     graphics.clear();
/// }
///
/// private:
///
/// double backgroundMusicTimer = 0.0;
/// Util::Pulsar::AudioTrack backgroundMusic;
/// Util::Pulsar::AudioHandle backgroundMusicHandle;
///
/// };
///
/// void main() {
///     /// Open frame buffer and create game engine instance
///     auto lfbFile = Util::Io::File("/device/lfb");
///     auto lfb = Util::Graphic::LinearFrameBuffer::open(lfbFile);
///     auto engine = Util::Pulsar::Engine(lfb, 60, scaleFactor);
///
///     /// Instantiate our test scene and start the game engine
///     Util::Pulsar::Game::getInstance().pushScene(new TestScene());
///     engine.run();
///
///     return 0;
/// }
/// ```
class AudioTrack {

public:
    /// Create an audio track instance with no audio buffer assigned.
    AudioTrack() = default;

    /// Create an audio track instance that loads a WAV file from the specified path.
    explicit AudioTrack(const String &waveFilePath);

    /// Start playback of the audio track.
    /// If `loop` is true, the audio track will loop continuously until stopped.
    /// The returned `AudioHandle` can be used to control playback (e.g. stop).
    [[nodiscard]] AudioHandle play(bool loop) const;

    /// Get the audio buffer associated with this track.
    [[nodiscard]] const AudioBuffer& getBuffer() const;

    /// Get the unique ID of this audio track instance.
    [[nodiscard]] size_t getId() const;

private:

    static Async::IdGenerator idGenerator;

    const AudioBuffer *buffer = nullptr;
    size_t id = 0;
};

}

#endif