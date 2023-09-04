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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/risch114/bachelorarbeit
 */

#include "ObjectFile.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Math.h"

namespace Util::Game::D3 {

ObjectFile::ObjectFile(const Array<Math::Vector3D> &vertices, const Array<Math::Vector2D> &edges) : vertices(vertices), edges(edges) {}

ObjectFile* ObjectFile::open(const String &path) {
    auto fileStream = Io::FileInputStream(path);
    auto stream = Io::BufferedInputStream(fileStream);
    auto vertexList = ArrayList<Math::Vector3D>();
    auto edgeList = ArrayList<Math::Vector2D>();
    bool endOfFile = false;

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
                auto edge = Util::Math::Vector2D(point1, point2);

                if (!edgeList.contains(Math::Vector2D(point1, point2)) && !edgeList.contains(Math::Vector2D(point2, point1))) {
                    edgeList.add(edge);
                }
            }

            // Add connection from last vertex to first vertex
            auto point1 = String::parseInt(lineSplit[lineSplit.length() - 1].split("/")[0]) - 1;
            auto point2 = String::parseInt(lineSplit[0].split("/")[0]) - 1;
            auto edge = Util::Math::Vector2D(point1, point2);

            if (!edgeList.contains(edge)) {
                edgeList.add(edge);
            }
        }

        currentLine = stream.readLine(endOfFile);
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

    return new ObjectFile(vertexList.toArray(), edgeList.toArray());
}

const Array<Math::Vector3D>& ObjectFile::getVertices() const {
    return vertices;
}

const Array<Math::Vector2D>& ObjectFile::getEdges() const {
    return edges;
}

}