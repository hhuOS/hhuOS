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

#include "lib/libc/printf.h"
#include "lib/libc/sprintf.h"
#include "kernel/Kernel.h"
#include "lib/OutputStream.h"
#include "devices/graphics/text/CgaText.h"
#include "devices/graphics/text/TextDriver.h"

#include "lib/file/File.h"

extern "C" {
    #include "lib/libc/string.h"
}

#define PRINTF_BUFFER       32

Spinlock *printLock;

OutputStream *stdout = nullptr;

CgaText &getCgaText() {

    static CgaText tmp;

    return tmp;
}

Spinlock &getPrintLock() {

    static Spinlock tmp;

    return tmp;
}

void init() {

    CgaText &cgaText = getCgaText();

    cgaText.clear();

    stdout = &cgaText;

    printLock = &getPrintLock();
}

void printf(const char *format, ...) {

    if (stdout == nullptr) {
        init();
    }


    if(!printLock->tryLock()) {
        return;
    }

    const char* traverse = format;
    const char* string;

    char buffer[PRINTF_BUFFER];
    char specifier[SPECIFIER_LENGTH + PADDING_LENGTH + 1];

    va_list args;
    va_start(args, format);

    // Traverse through the format string
    while (*traverse != '\0') {

        // Print out all non-specifier characters
        while( *traverse != '%' ) {

            if (*traverse == '\0') {
                va_end(args);
                printLock->release();
                return;
            }

            *stdout << *traverse;
            stdout->flush();

            traverse++;
        }

        // Copy specifier to buffer
        memcpy(specifier, traverse, SPECIFIER_LENGTH + PADDING_LENGTH);

        traverse++;

        // Check if padding exists
        if (*traverse == '0') {
            traverse += 2;
            specifier[SPECIFIER_LENGTH + PADDING_LENGTH] = '\0';
        } else {
            specifier[SPECIFIER_LENGTH] = '\0';
        }

        traverse++;

        // Print strings without copying/converting them unnecessarily
        if (specifier[1] == 's') {
            string = va_arg(args, char *);
            *stdout << (char *) string;
            stdout->flush();
            continue;
        }

        // Convert numbers to strings and store them inside the buffer
        sprintf(buffer, specifier, args);

        // Flush buffer
        *stdout << buffer;
        stdout->flush();
    }

    va_end(args);
    printLock->release();
}

void printfUpdateStdout() {
	stdout = File::open("/dev/stdout", "w");
}

