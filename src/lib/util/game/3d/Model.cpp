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
#include "lib/util/math/Math.h"

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
    bool endOfFile = false;

    if (modelFile.getName().endsWith(".3do")) {
        ReadState readState = UNKNOWN;

        auto currentLine = stream.readLine(endOfFile);
        while (!endOfFile) {
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

            currentLine = stream.readLine(endOfFile);
        }
    } else {
        auto currentLine = stream.readLine(endOfFile);
        while (!endOfFile) {
            auto lineSplit = currentLine.substring(2).split(" ");

            if (currentLine.beginsWith("v ")) {
                auto vertex = Math::Vector3D(String::parseDouble(lineSplit[0]), String::parseDouble(lineSplit[1]), String::parseDouble(lineSplit[2]));
                vertexList.add(vertex);
            } else if (currentLine.beginsWith("f ")) {
                for (uint32_t i = 0; i < lineSplit.length() - 1; i++) {
                    auto point1 = String::parseInt(lineSplit[i].split("/")[0]) - 1;
                    auto point2 = String::parseInt(lineSplit[i + 1].split("/")[0]) - 1;
                    edgeList.add(Util::Math::Vector2D(point1, point2));
                }

                // Add connection from last vertex to first vertex
                auto point1 = String::parseInt(lineSplit[lineSplit.length() - 1].split("/")[0]) - 1;
                auto point2 = String::parseInt(lineSplit[0].split("/")[0]) - 1;
                edgeList.add(Util::Math::Vector2D(point1, point2));
            }

            currentLine = stream.readLine(endOfFile);
        }
    }

    // Normalize model size
    double maxCoordinate = 0;
    for (const auto &vertex : vertexList) {
        if (Math::absolute(vertex.getX()) > maxCoordinate) maxCoordinate = vertex.getX();
        if (Math::absolute(vertex.getY()) > maxCoordinate) maxCoordinate = vertex.getY();
        if (Math::absolute(vertex.getZ()) > maxCoordinate) maxCoordinate = vertex.getZ();
    }

    for (uint32_t i = 0; i < vertexList.size(); i++) {
        auto vertex = vertexList.get(i);
        vertexList.set(i, Math::Vector3D(vertex.getX() / maxCoordinate, vertex.getY() / maxCoordinate, vertex.getZ() / maxCoordinate));
    }

    vertices = vertexList.toArray();
    edges = edgeList.toArray();
    transformedBuffer = Array<Math::Vector3D>(vertices.length());

    calculateTransformedVertices();
}

}