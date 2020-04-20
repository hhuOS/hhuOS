/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <fstream>

const uint32_t BEEP_MAGIC = 0x42454550;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: ./beepc <source> <destination>\n");
        
        return 1;
    }

    // Open input and output files
    std::ifstream in(argv[1]);
    std::ofstream out(argv[2]);

    // Store all lines inside a vector
    std::string str;
    std::vector<std::string> lines;
    for (std::string line; std::getline(in, str);) {
        lines.push_back(str);
    }

    // Create output directory if necessary
    std::experimental::filesystem::path output(argv[2]);
    
    if(!std::experimental::filesystem::exists(output.parent_path())) {
        std::experimental::filesystem::create_directory(output.parent_path());
    }

    // Write file header
    uint32_t size = lines.size();
    out.write(reinterpret_cast<const char*>(&BEEP_MAGIC), sizeof(uint32_t));
    out.write(reinterpret_cast<const char*>(&size), sizeof(float));

    // Write sound entries
    for (const auto &line : lines) {
        float frequency = std::stof(line.substr(0, line.find(',')));
        uint32_t length = std::stoul(line.substr(line.find(',') + 1));

        out.write(reinterpret_cast<const char*>(&frequency), sizeof(float));
        out.write(reinterpret_cast<const char*>(&length), sizeof(uint32_t));
    }

    out.close();

    return 0;
} 
