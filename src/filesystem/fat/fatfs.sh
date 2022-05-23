#!/bin/bash

# Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
# Institute of Computer Science, Department Operating Systems
# Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
#
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

# Abort if FatFs source code already exists
if [ -d ff/ ]; then
  exit 0;
fi

# Download and unzip FatFs source code
wget -O "/tmp/ff.zip" "http://elm-chan.org/fsw/ff/arc/ff14a.zip"
unzip -o "/tmp/ff.zip" -d "ff"

# Disable real time clock functionality
sed -i "s/#define FF_FS_NORTC		0/#define FF_FS_NORTC		1/g" "ff/source/ffconf.h"
# Set code page to 437 (U.S.)
sed -i "s/#define FF_CODE_PAGE	932/#define FF_CODE_PAGE	437/g" "ff/source/ffconf.h"
# Enable LFN support
sed -i "s/#define FF_USE_LFN		0/#define FF_USE_LFN		1/g" "ff/source/ffconf.h"
# Enable f_mkfs() function
sed -i "s/#define FF_USE_MKFS		0/#define FF_USE_MKFS		1/g" "ff/source/ffconf.h"
# Set the amount of volumes to 10 (maximum)
sed -i "s/#define FF_VOLUMES		1/#define FF_VOLUMES		10/g" "ff/source/ffconf.h"
