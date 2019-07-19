#ifndef HHUOS_ELF_H
#define HHUOS_ELF_H

#include <lib/file/File.h>
#include <lib/elf/ElfConstants.h>

class Elf {

private:

    File *file = nullptr;

    ElfConstants::FileHeader fileHeader{};

    char *sectionNames = nullptr;

    ElfConstants::ProgramHeader *programHeaders = nullptr;

    Util::HashMap<String, ElfConstants::SectionHeader> sectionHeaders;

private:

    explicit Elf(File *file);

    void readFileHeader();

    bool isValid();

    void readSectionNames();

    void readProgramHeaders();

    void readSectionHeaders();

public:

    static Elf* load(const String &path);

    Elf(const Elf &copy) = delete;

    Elf& operator=(const Elf &copy) = delete;

    ~Elf();

    uint32_t getSizeInMemory();

    void loadProgram();

    int (*getEntryPoint())(int, char **) {
        return reinterpret_cast<int (*)(int, char **)>(fileHeader.entry);
    }
};

#endif
