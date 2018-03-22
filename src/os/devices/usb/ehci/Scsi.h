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

#ifndef __Scsi_include__
#define __Scsi_include__

#include <cstdint>

/**
 * @author Filip Krakowski
 */
class Scsi {

public:

    typedef struct {
        uint8_t     deviceType:5;                   // Peripheral Device Type
        uint8_t     deviceQualifier:3;              // Peripheral Device Qualifier

        uint8_t     r1:7;                           // Reserved
        uint8_t     removable:1;                    // Removable Media

        uint8_t     version;                        // Version

        uint8_t     dataFormat:4;                   // Response Data Format
        uint8_t     hierachicalSupport:1;           // Hierarchical Support
        uint8_t     normalAca:1;                    // Normal ACA
        uint8_t     r2:2;                           // Reserved

        uint8_t     additionalLength;               // Additional Length

        uint8_t     protect:1;                      // Protect
        uint8_t     r3:2;                           // Reserved
        uint8_t     thirdPartyCopy:1;               // Third-Party Copy
        uint8_t     targetPortGroupSupport:2;       // Target Port Group Support
        uint8_t     acCoordinator:1;                // Access Controls Coordinator
        uint8_t     sscSupported:1;                 // SSC Supported

        uint8_t     todo1;                          // TODO

        uint8_t     todo2;                          // TODO

        uint8_t     vendorId[8];                    // Vendor Identification

        uint8_t     productId[16];                  // Product Identification

        uint8_t     productRevision[4];             // Product Revision Level

        uint8_t     serialNumber[8];                // Serial Number
    } InquiryData;

    struct Command {

        uint8_t     data[16];                       // Command Data

        static Scsi::Command *read(uint32_t blockHigh, uint32_t blockLow, uint32_t length);
        static Scsi::Command *write(uint32_t blockHigh, uint32_t blockLow, uint32_t length);
        static Scsi::Command *inquiry();
        static Scsi::Command *requestSense();
        static Scsi::Command *testUnitReady();
        static Scsi::Command *readCapacity();
    };


    Scsi();

private:

};

#endif