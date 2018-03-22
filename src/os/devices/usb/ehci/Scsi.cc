/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Scsi.h"

extern "C" {
    #include "lib/libc/string.h"
}

Scsi::Scsi() {
    
}

Scsi::Command *Scsi::Command::inquiry() {
    Scsi::Command *command = new Scsi::Command;
    memset(command, 0, sizeof(Scsi::Command));

    /* Operation Code */
    command->data[0] = 0x12;

    /* Allocation Length */
    command->data[4] = 36;

    return command;
}

Scsi::Command *Scsi::Command::requestSense() {
    Scsi::Command *command = new Scsi::Command;
    memset(command, 0, sizeof(Scsi::Command));

    /* Operation Code */
    command->data[0] = 0x03;

    /* Allocation Length */
    command->data[4] = 0x12;

    return command;
}

Scsi::Command *Scsi::Command::testUnitReady() {
    Scsi::Command *command = new Scsi::Command;
    memset(command, 0, sizeof(Scsi::Command));

    return command;
}

Scsi::Command *Scsi::Command::read(uint32_t blockHigh, uint32_t blockLow, uint32_t length) {
    Scsi::Command *command = new Scsi::Command;
    memset(command, 0, sizeof(Scsi::Command));

    /* Operation Code */
    command->data[0] = 0x28;

    /* Logical Block Address */
//    command->data[2] = (uint8_t) ( blockHigh >> 24 );
//    command->data[3] = (uint8_t) ( blockHigh >> 16 );
    command->data[2] = (uint8_t) ( blockHigh >> 8  );
    command->data[3] = (uint8_t) ( blockHigh >> 0  );
//    command->data[4] = (uint8_t) ( blockLow  >> 24 );
//    command->data[5] = (uint8_t) ( blockLow  >> 16 );
    command->data[4] = (uint8_t) ( blockLow  >> 8 );
    command->data[5] = (uint8_t) ( blockLow  >> 0 );

    /* Transfer Length */
//    command->data[10] = (uint8_t) ( length >> 24 );
//    command->data[11] = (uint8_t) ( length >> 16 );
    command->data[7] = (uint8_t) ( length >> 8  );
    command->data[8] = (uint8_t) ( length >> 0  );

    return command;
}

Scsi::Command *Scsi::Command::readCapacity() {
    Scsi::Command *command = new Scsi::Command;
    memset(command, 0, sizeof(Scsi::Command));

    /* Operation Code */
    command->data[0] = 0x9E;

    /* Service Action */
    command->data[1] = 0x10;

    /* Allocation Length */
    command->data[13] = 32;

    return command;
}

Scsi::Command *Scsi::Command::write(uint32_t blockHigh, uint32_t blockLow, uint32_t length) {
    Scsi::Command *command = new Scsi::Command;
    memset(command, 0, sizeof(Scsi::Command));

    /* Operation Code */
    command->data[0] = 0x2A;

    /* Logical Block Address */
//    command->data[2] = (uint8_t) ( blockHigh >> 24 );
//    command->data[3] = (uint8_t) ( blockHigh >> 16 );
    command->data[2] = (uint8_t) ( blockHigh >> 8  );
    command->data[3] = (uint8_t) ( blockHigh >> 0  );
//    command->data[6] = (uint8_t) ( blockLow  >> 24 );
//    command->data[7] = (uint8_t) ( blockLow  >> 16 );
    command->data[4] = (uint8_t) ( blockLow  >> 8 );
    command->data[5] = (uint8_t) ( blockLow  >> 0 );

    /* Transfer Length */
//    command->data[10] = (uint8_t) ( length >> 24 );
//    command->data[11] = (uint8_t) ( length >> 16 );
    command->data[7] = (uint8_t) ( length >> 8  );
    command->data[8] = (uint8_t) ( length >> 0  );

    return command;
}
