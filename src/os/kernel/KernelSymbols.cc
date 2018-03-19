#include "KernelSymbols.h"

extern "C" {
    #include "lib/libc/string.h"
}

HashMap<String, uint32_t> KernelSymbols::symbolTable(1129);
HashMap<uint32_t, String> KernelSymbols::debugTable(1129);
File *KernelSymbols::symbolFile = nullptr;
File *KernelSymbols::debugFile = nullptr;
bool KernelSymbols::initialized = false;

uint32_t pow(uint32_t base, uint32_t exponent) {

    if (exponent == 0) {
        return 1;
    }

    if (base == 0) {
        return 0;
    }

    uint32_t ret = base;

    for (uint32_t i = 1; i < exponent; i++) {
        ret *= base;
    }

    return ret;
}

uint32_t hextoint(const char *hexString) {
    uint32_t len = strlen(hexString);
    uint32_t ret = 0;

    char current;
    uint32_t num = 0;
    for (uint32_t i = len; i > 0; i--) {
        current = hexString[i - 1];

        if (current > 47 && current < 58) {
            num = current - '0';
        } else if (current > 64 && current < 71) {
            num = current - '7';
        } else if (current > 96 && current < 103) {
            num = current - 'W';
        } else {
            break;
        }

        ret += num * pow(16, len - i);

    }

    return ret;
}

uint32_t KernelSymbols::get(const String &name) {

    Optional<uint32_t> optional = symbolTable.get(name);

    if (optional.isNull()) {
        return 0x0;
    }

    return optional.value();
}

String KernelSymbols::get(uint32_t eip) {

    if (!initialized) {
        return "";
    }

    Optional<String> optional;

    while (optional.isNull()) {
        optional = debugTable.get(eip);
        eip--;
    }

    return optional.value();
}

void KernelSymbols::parseSymbolFile() {

    uint64_t symbolLength = symbolFile->getInfo()->length;
    uint64_t debugLength = debugFile->getInfo()->length;

    char *symbolBuffer = new char[symbolLength + 1];
    char *debugBuffer = new char[debugLength + 1];

    symbolFile->readBytes(symbolBuffer, symbolLength);
    debugFile->readBytes(debugBuffer, debugLength);

    symbolBuffer[symbolLength] = '\0';
    debugBuffer[debugLength] = '\0';

    Util::Array<String> symbolList = String(symbolBuffer).split('\n');
    Util::Array<String> debugList = String(debugBuffer).split('\n');

    for (const String &symbolLine : symbolList) {

        Util::Array<String> token = symbolLine.split(" ", 3);

        symbolTable.put(token[2], hextoint((char*) token[0]));
    }

    for (const String &debugLine : debugList) {

        Util::Array<String> token = debugLine.split(" ", 3);

        debugTable.put(hextoint((char*) token[0]), token[2]);
    }

//    char *current = symbolBuffer;
//
//    while (*current != '\0') {
//
//        char *endOfLine = strchr(current, '\n');
//
//        if (endOfLine) {
//            *endOfLine = '\0';
//        }
//
//        uint32_t address = hextoint(strtok(current, " "));
//        strtok(0x0, " ");
//        char *symbol = strtok(0x0, " ");
//
//        symbolTable.put(symbol, address);
//
//        current = endOfLine + 1;
//    }
//
//    current = debugBuffer;
//
//    while (*current != '\0') {
//
//        char *endOfLine = strchr(current, '\n');
//
//        if (endOfLine) {
//            *endOfLine = '\0';
//        }
//
//        uint32_t address = hextoint(strtok(current, " "));
//        strtok(0x0, " ");
//        char *symbol = strtok(0x0, " ");
//
//        debugTable.put(address, symbol);
//
//        current = endOfLine + 1;
//    }
}

void KernelSymbols::initialize() {

#if KERNEL_LOAD_SYMBOLS
    symbolFile = File::open(SYMBOL_FILE_PATH, "r");

    debugFile = File::open(DEBUG_FILE_PATH, "r");

    if (symbolFile == nullptr || debugFile == nullptr) {
        return;
    }

    parseSymbolFile();

    initialized = true;
#endif
}

bool KernelSymbols::isInitialized() {
    return initialized;
}
