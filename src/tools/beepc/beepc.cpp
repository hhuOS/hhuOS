#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

const uint32_t BEEP_MAGIC = 0x42454550;

int main(int argc, char *argv[]) {

    if (argc != 3) {
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

    // Write file header
    uint32_t size = lines.size();
    out.write((char*)&BEEP_MAGIC, sizeof(uint32_t));
    out.write((char*)&size, sizeof(float));

    // Write sound entries
    std::vector<std::string> split;
    for (auto &tmp : lines) {
        boost::split(split, tmp, [](char c){return c == ',';});

        float frequency = std::stof(split[0]);
        uint32_t length = std::stoul(split[1]);

        out.write((char*)&frequency, sizeof(float));
        out.write((char*)&length, sizeof(uint32_t));
    }

    out.close();

    return 0;
}