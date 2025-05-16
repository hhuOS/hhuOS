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
 */

#include "ObjectFile.h"

#include <stdint.h>

#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Math.h"
#include "lib/util/collection/Iterator.h"

namespace Util::Game::D3 {

ObjectFile::ObjectFile(const Array<Math::Vector3<double>> &vertices, const Array<Math::Vector3<double>> &vertexNormals, const Array<Math::Vector3<double>> &vertexTextures,
                       const Array<uint32_t> &vertexDrawOrder, const Array<uint32_t> &normalDrawOrder, const Array<uint32_t> &textureDrawOrder) :
        vertices(vertices), vertexNormals(vertexNormals), vertexTextures(vertexTextures), vertexDrawOrder(vertexDrawOrder), normalDrawOrder(normalDrawOrder), textureDrawOrder(textureDrawOrder) {}

ObjectFile* ObjectFile::open(const String &path) {
    auto fileStream = Io::FileInputStream(path);
    auto stream = Io::BufferedInputStream(fileStream);
    bool endOfFile = false;

    auto vertexList = ArrayList<Math::Vector3<double>>();
    auto normalList = ArrayList<Math::Vector3<double>>();
    auto textureList = ArrayList<Math::Vector3<double>>();
    auto vertexDrawOrder = ArrayList<uint32_t>();
    auto normalDrawOrder = ArrayList<uint32_t>();
    auto textureDrawOrder = ArrayList<uint32_t>();

    // Read the file line by line
    auto currentLine = stream.readLine(endOfFile);
    while (!endOfFile) {
        auto lineSplit = currentLine.substring(2).split(" ");

        if (currentLine.beginsWith("v ")) {
            auto vertex = Math::Vector3<double>(String::parseDouble(lineSplit[0]), String::parseDouble(lineSplit[1]), String::parseDouble(lineSplit[2]));
            vertexList.add(vertex);
        } else if (currentLine.beginsWith("f ")) {
            // Fill list with the correct order to draw triangles properly
            for (uint32_t i = 0; i < lineSplit.length(); i++) {
                if(i > 2) {
                    // Vertex normals order
                    if(lineSplit[i].split("/").length() == 3){
                        if(lineSplit[i].split("/")[1] != ""){
                            auto textureIndex0 = String::parseInt(lineSplit[0].split("/")[1]) - 1;
                            auto textureIndex1 = String::parseInt(lineSplit[i - 1].split("/")[1]) - 1;
                            auto textureIndex2 = String::parseInt(lineSplit[i].split("/")[1]) - 1;
                            // Add entries into the texture order
                            textureDrawOrder.add(textureIndex0);
                            textureDrawOrder.add(textureIndex1);
                            textureDrawOrder.add(textureIndex2);
                        }

                        auto normalIndex0 = String::parseInt(lineSplit[0].split("/")[2]) - 1;
                        auto normalIndex1 = String::parseInt(lineSplit[i-1].split("/")[2]) - 1;
                        auto normalIndex2 = String::parseInt(lineSplit[i].split("/")[2]) - 1;
                        normalDrawOrder.add(normalIndex0);
                        normalDrawOrder.add(normalIndex1);
                        normalDrawOrder.add(normalIndex2);
                    }

                    auto firstVertex = String::parseInt(lineSplit[0].split("/")[0]) - 1;
                    auto lastParsedVertex = String::parseInt(lineSplit[i-1].split("/")[0]) - 1;
                    auto currentVertex = String::parseInt(lineSplit[i].split("/")[0]) - 1;

                    vertexDrawOrder.add(firstVertex);
                    vertexDrawOrder.add(lastParsedVertex);
                    vertexDrawOrder.add(currentVertex);
                } else {
                    // Check if the face data has at least 2 entries
                    if (lineSplit[i].split("/").length() >= 2){
                        // Check if the vt entry is empty
                        if(lineSplit[i].split("/")[1] != ""){
                            auto textureIndex = String::parseInt(lineSplit[i].split("/")[1]) - 1;
                            textureDrawOrder.add(textureIndex); // Add entry into the texture Order
                        }
                    }

                    // Vertex normals order
                    if (lineSplit[i].split("/").length() == 3){
                        auto normalIndex = String::parseInt(lineSplit[i].split("/")[2]) - 1;
                        normalDrawOrder.add(normalIndex);
                    }

                    auto vertexIndex = String::parseInt(lineSplit[i].split("/")[0]) - 1;
                    vertexDrawOrder.add(vertexIndex);
                }
            }
        } else if(currentLine.beginsWith("vn ")){
            auto vertex = Math::Vector3<double>(String::parseDouble(lineSplit[0]), String::parseDouble(lineSplit[1]), String::parseDouble(lineSplit[2]));
            normalList.add(vertex);
        } else if(currentLine.beginsWith("vt ")){
            auto vertex = Math::Vector3<double>(String::parseDouble(lineSplit[0]), String::parseDouble(lineSplit[1]), 0.0);
            textureList.add(vertex);
        }

        currentLine = stream.readLine(endOfFile);
    }

    // Normalize model size
    double maxCoordinate = 0;
    for (const auto &vertex : vertexList) {
        auto absX = Math::absolute(vertex.getX());
        auto absY = Math::absolute(vertex.getY());
        auto absZ = Math::absolute(vertex.getZ());

        if (absX > maxCoordinate) maxCoordinate = absX;
        if (absY > maxCoordinate) maxCoordinate = absY;
        if (absZ > maxCoordinate) maxCoordinate = absZ;
    }

    for (uint32_t i = 0; i < vertexList.size(); i++) {
        auto vertex = vertexList.get(i);
        vertexList.set(i, Math::Vector3<double>(vertex.getX() / maxCoordinate, vertex.getY() / maxCoordinate, vertex.getZ() / maxCoordinate));
    }

    return new ObjectFile(vertexList.toArray(), normalList.toArray(), textureList.toArray(), vertexDrawOrder.toArray(), normalDrawOrder.toArray(), textureDrawOrder.toArray());
}

const Array<Math::Vector3<double>>& ObjectFile::getVertices() const {
    return vertices;
}

const Array<Math::Vector3<double>> &ObjectFile::getVertexNormals() const {
    return vertexNormals;
}

const Array<Math::Vector3<double>> &ObjectFile::getVertexTextures() const {
    return vertexTextures;
}

const Array<uint32_t> &ObjectFile::getVertexDrawOrder() const {
    return vertexDrawOrder;
}

const Array<uint32_t> &ObjectFile::getNormalDrawOrder() const {
    return normalDrawOrder;
}

const Array<uint32_t> &ObjectFile::getTextureDrawOrder() const {
    return textureDrawOrder;
}

}