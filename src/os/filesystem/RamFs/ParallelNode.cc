#include "ParallelNode.h"

String ParallelNode::generateName(Parallel::LptPort port) {
    switch(port) {
        case Parallel::LPT1 :
            return "parallel1";
        case Parallel::LPT2 :
            return "parallel2";
        case Parallel::LPT3 :
            return "parallel3";
        default:
            return "parallel";
    }
}

ParallelNode::ParallelNode(Parallel *parallel) : VirtualNode(generateName(parallel->getPortNumber()), FsNode::BLOCK_FILE), parallel(parallel) {

}

uint64_t ParallelNode::getLength() {
    return 0;
}

uint64_t ParallelNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    parallel->readData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}

uint64_t ParallelNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    parallel->sendData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}
