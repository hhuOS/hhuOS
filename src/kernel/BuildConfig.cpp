/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "BuildConfig.h"

#define XSTRINGIFY(a) STRINGIFY(a)
#define STRINGIFY(a) #a

#ifdef HHUOS_VERSION
const char *BuildConfig::VERSION = XSTRINGIFY(HHUOS_VERSION);
#else
const char *BuildConfig::VERSION = "v0.0.0";
#endif

#ifdef HHUOS_CODENAME
const char *BuildConfig::CODENAME = XSTRINGIFY(HHUOS_CODENAME);
#else
const char *BuildConfig::CODENAME = "Unknown";
#endif

#ifdef HHUOS_GIT_REV
const char *BuildConfig::GIT_REV = XSTRINGIFY(HHUOS_GIT_REV);
#else
const char *BuildConfig::GIT_REV = "unknown";
#endif

#ifdef HHUOS_GIT_BRANCH
const char *BuildConfig::GIT_BRANCH = XSTRINGIFY(HHUOS_GIT_BRANCH);
#else
const char *BuildConfig::GIT_BRANCH = "unknown";
#endif

#ifdef HHUOS_BUILD_DATE
const char *BuildConfig::BUILD_DATE = XSTRINGIFY(HHUOS_BUILD_DATE);
#else
const char *BuildConfig::BUILD_DATE = "0000-00-00 00:00:00";
#endif

#ifdef HHUOS_BUILD_TYPE
const char *BuildConfig::BUILD_TYPE = XSTRINGIFY(HHUOS_BUILD_TYPE);
#else
const char *BuildConfig::BUILD_TYPE = "unknown";
#endif

#undef STRINGIFY
#undef XSTRINGIFY