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

#ifndef _LIB_STR_FORMAT_H
#define _LIB_STR_FORMAT_H 1

#include <cstdarg>

class StrFormat {

public:

    static char * convert(unsigned int number, unsigned int base, unsigned int padding = 0) {

        unsigned int length = 0;

        static char buffer[50];

        char *ptr;

        ptr = &buffer[49];

        *ptr = '\0';

        do {

            *--ptr = hex[number % base];

            number /= base;

            length++;

        } while (number != 0);
        
        while (length < padding) {

            *--ptr = hex[number % base];

            length++;
        }
        
        return(ptr);
    }

private:

    static constexpr const char* hex = "0123456789ABCDEF";
    
};

#endif
