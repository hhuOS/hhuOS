/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#include "Model.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"

namespace Util::Game::D3 {

Model::Model(uint32_t tag, const Io::File &modelFile, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale) : Entity(tag, position, rotation, scale), modelFile(modelFile) {}

Model::Model(uint32_t tag, const Io::File &modelFile, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale, const Graphic::Color &color) : Entity(tag, position, rotation, scale), modelFile(modelFile), color(color) {}

void Model::initialize() {
    loadModelFromFile();
}

Util::Array<Math::Vector3D> &Model::getVertices() {
    return vertices;
}

Util::Array<Util::Math::Vector2D> &Model::getEdges() {
    return edges;
}

Util::Array<Math::Vector3D> &Model::getTransformedBuffer() {
    return transformedBuffer;
}

void Model::setTransformedBuffer(const Array<Math::Vector3D> &newVertices) {
    transformedBuffer = newVertices;
}

void Model::calculateTransformedVertices() {
    for (uint32_t i = 0; i < getVertices().length(); i++) {
        auto vertex = vertices[i];

        vertex = vertex.scale(getScale());
        vertex = vertex.rotate(getRotation());
        vertex = vertex + getPosition();

        transformedBuffer[i] = vertex;
    }
}

void Model::draw(Graphics &graphics) {
    graphics.setColor(color);
    graphics.drawModel(transformedBuffer, edges);
}

void Model::onTransformChange() {
    calculateTransformedVertices();
}

void Model::loadModelFromFile() {
    auto fileStream = Io::FileInputStream(modelFile);
    auto stream = Io::BufferedInputStream(fileStream);
    auto vertexList = ArrayList<Math::Vector3D>();
    auto edgeList = ArrayList<Math::Vector2D>();
    ReadState readState = UNKNOWN;

    auto currentLine = stream.readLine();
    while (!currentLine.isEmpty()) {
        if (currentLine.beginsWith("#")) {
            // Skip comments
        } else if (currentLine == "vertices") {
            readState = READ_VERTICES;
        } else if (currentLine == "edges") {
            readState = READ_EDGES;
        } else {
            auto vectorStrings = currentLine.split(" ");
            for (const auto &string: vectorStrings) {
                auto vectorSplit = string.split(",");

                if (readState == READ_VERTICES) {
                    auto vertex = Math::Vector3D(String::parseDouble(vectorSplit[0]), String::parseDouble(vectorSplit[1]), String::parseDouble(vectorSplit[2]));
                    vertexList.add(vertex);
                } else if (readState == READ_EDGES) {
                    auto edge = Math::Vector2D(String::parseDouble(vectorSplit[0]), String::parseDouble(vectorSplit[1]));
                    edgeList.add(edge);
                }
            }
        }

        currentLine = stream.readLine();
    }

    vertices = vertexList.toArray();
    edges = edgeList.toArray();
    transformedBuffer = Array<Math::Vector3D>(vertices.length());

    calculateTransformedVertices();
}

}