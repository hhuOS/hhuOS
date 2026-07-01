/*
* Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 * The OpenGL demo has been created during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_APPLICATION_DEMO_DEMOMODEL_H
#define HHUOS_APPLICATION_DEMO_DEMOMODEL_H

#include <stddef.h>

#include <util/math/Vector3.h>
#include <util/graphic/Color.h>
#include <util/graphic/Colors.h>
#include <pulsar/3d/Model.h>

/// A 3D object loaded from a predefined model.
/// See the `Type` enum for more details on the different models.
class DemoModel : public Pulsar::D3::Model {

public:
    /// The supported model types.
    enum Type {
        /// A static tree model (does not move or rotate) without a texture.
        TREE,
        /// A static mode of a street lantern (does not move or rotate) without a texture.
        LANTERN,
        /// A textured icosphere-shaped model that rotates around its center and moves in a circle.
        ICOSPHERE
    };

    /// Create a new model instance of the given type.
    /// The color is used to fill the object if it has no texture.
    DemoModel(Type type, const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation,
        const Util::Math::Vector3<float> &scale, const Util::Graphic::Color &color = Util::Graphic::Colors::WHITE);

    /// Initialize the model instance by loadings its vertices and texture and creating a display list.
    void initialize() override;

    /// Translate and rotate the model according to its type and the given delta time.
    void onUpdate(float delta) override;

    /// Draw the model.
    void draw(Pulsar::Graphics &graphics) const override;

private:

    Type type;
    Util::Graphic::Color color;

    size_t drawListID = SIZE_MAX;

    static const char* pathForType(Type type);

    static size_t TREE_DRAW_LIST_ID;
    static size_t LANTERN_DRAW_LIST_ID;
    static size_t ICOSPHERE_DRAW_LIST_ID;
};



#endif
