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

#include <stddef.h>

#include "ObjectFile.h"

#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Math.h"

namespace Pulsar {
namespace D3 {

ObjectFile::ObjectFile(const Util::String &path) {
    Util::Io::FileInputStream fileStream(path);
    Util::Io::BufferedInputStream stream(fileStream);

    Util::ArrayList<Util::Math::Vector3<float>> vertexList;
    Util::ArrayList<Util::Math::Vector3<float>> normalList;
    Util::ArrayList<Util::Math::Vector3<float>> textureList;
    Util::ArrayList<size_t> vertexDrawOrderList;
    Util::ArrayList<size_t> normalDrawOrderList;
    Util::ArrayList<size_t> textureDrawOrderList;

    // Read the file line by line
    auto currentLine = stream.readLine();
    while (!currentLine.endOfFile) {
        const auto lineSplit = currentLine.content.substring(2).split(" ");

        if (currentLine.content.beginsWith("v ")) {
            const auto vertex = Util::Math::Vector3<float>(
                Util::String::parseFloat<float>(lineSplit[0]),
                Util::String::parseFloat<float>(lineSplit[1]),
                Util::String::parseFloat<float>(lineSplit[2]));

            vertexList.add(vertex);
        } else if (currentLine.content.beginsWith("f ")) {
            // Fill list with the correct order to draw triangles properly
            for (size_t i = 0; i < lineSplit.length(); i++) {
                if (i > 2) {
                    // Vertex normals order
                    if (lineSplit[i].split("/").length() == 3) {
                        if (lineSplit[i].split("/")[1] != "") {
                            const auto textureIndex0 =
                                Util::String::parseNumber<size_t>(lineSplit[0].split("/")[1]) - 1;
                            const auto textureIndex1 =
                                Util::String::parseNumber<size_t>(lineSplit[i - 1].split("/")[1]) - 1;
                            const auto textureIndex2 =
                                Util::String::parseNumber<size_t>(lineSplit[i].split("/")[1]) - 1;

                            textureDrawOrderList.add(textureIndex0);
                            textureDrawOrderList.add(textureIndex1);
                            textureDrawOrderList.add(textureIndex2);
                        }

                        const auto normalIndex0 =
                            Util::String::parseNumber<size_t>(lineSplit[0].split("/")[2]) - 1;
                        const auto normalIndex1 =
                            Util::String::parseNumber<size_t>(lineSplit[i - 1].split("/")[2]) - 1;
                        const auto normalIndex2 =
                            Util::String::parseNumber<size_t>(lineSplit[i].split("/")[2]) - 1;

                        normalDrawOrderList.add(normalIndex0);
                        normalDrawOrderList.add(normalIndex1);
                        normalDrawOrderList.add(normalIndex2);
                    }

                    const auto firstVertex =
                        Util::String::parseNumber<size_t>(lineSplit[0].split("/")[0]) - 1;
                    const auto lastParsedVertex =
                        Util::String::parseNumber<size_t>(lineSplit[i - 1].split("/")[0]) - 1;
                    const auto currentVertex =
                        Util::String::parseNumber<size_t>(lineSplit[i].split("/")[0]) - 1;

                    vertexDrawOrderList.add(firstVertex);
                    vertexDrawOrderList.add(lastParsedVertex);
                    vertexDrawOrderList.add(currentVertex);
                } else {
                    // Check if the face data has at least 2 entries
                    if (lineSplit[i].split("/").length() >= 2){
                        // Check if the vt entry is empty
                        if(lineSplit[i].split("/")[1] != ""){
                            const auto textureIndex =
                                Util::String::parseNumber<size_t>(lineSplit[i].split("/")[1]) - 1;

                            textureDrawOrderList.add(textureIndex);
                        }
                    }

                    // Vertex normals order
                    if (lineSplit[i].split("/").length() == 3){
                        const auto normalIndex =
                            Util::String::parseNumber<size_t>(lineSplit[i].split("/")[2]) - 1;

                        normalDrawOrderList.add(normalIndex);
                    }

                    const auto vertexIndex =
                        Util::String::parseNumber<size_t>(lineSplit[i].split("/")[0]) - 1;

                    vertexDrawOrderList.add(vertexIndex);
                }
            }
        } else if (currentLine.content.beginsWith("vn ")){
            auto vertex = Util::Math::Vector3<float>(
                Util::String::parseFloat<float>(lineSplit[0]),
                Util::String::parseFloat<float>(lineSplit[1]),
                Util::String::parseFloat<float>(lineSplit[2]));

            normalList.add(vertex);
        } else if (currentLine.content.beginsWith("vt ")){
            auto vertex = Util::Math::Vector3<float>(
                Util::String::parseFloat<float>(lineSplit[0]),
                Util::String::parseFloat<float>(lineSplit[1]),
                0.0);

            textureList.add(vertex);
        }

        currentLine = stream.readLine();
    }

    // Normalize model size
    float maxCoordinate = 0;
    for (const auto &vertex : vertexList) {
        const auto absX = Util::Math::absolute(vertex.getX());
        const auto absY = Util::Math::absolute(vertex.getY());
        const auto absZ = Util::Math::absolute(vertex.getZ());

        if (absX > maxCoordinate) {
            maxCoordinate = absX;
        }
        if (absY > maxCoordinate) {
            maxCoordinate = absY;
        }
        if (absZ > maxCoordinate) {
            maxCoordinate = absZ;
        }
    }

    for (size_t i = 0; i < vertexList.size(); i++) {
        auto vertex = vertexList.get(i);
        vertexList.set(i, Util::Math::Vector3<float>(
            vertex.getX() / maxCoordinate,
            vertex.getY() / maxCoordinate,
            vertex.getZ() / maxCoordinate));
    }

    vertices = vertexList.toArray();
    vertexNormals = normalList.toArray();
    vertexTextures = textureList.toArray();
    vertexDrawOrder = vertexDrawOrderList.toArray();
    normalDrawOrder = normalDrawOrderList.toArray();
    textureDrawOrder = textureDrawOrderList.toArray();
}

}
}