#include "kernel/Logger.h"
#include "SysLogeNode.h"

SysLogNode::SysLogNode() : VirtualNode("syslog", FsNode::REGULAR_FILE) {

}

uint64_t SysLogNode::getLength() {
    getLogsFromLogger();

    return cache.length();
}

uint64_t SysLogNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

uint64_t SysLogNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    //getLogsFromLogger();

    uint64_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint64_t) (length - pos);
    }

    memcpy(buf, (char*) cache + pos, numBytes);

    return numBytes;
}

void SysLogNode::getLogsFromLogger() {
    cache = "";
    for (String log : Logger::getLogs()) {
        cache += log;
        cache += "\n";
    }
}