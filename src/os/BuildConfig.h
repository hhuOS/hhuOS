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

#ifndef HHUOS_BUILDCONFIG_H
#define HHUOS_BUILDCONFIG_H

#define XSTRINGIFY(a) STRINGIFY(a)
#define STRINGIFY(a) #a

class BuildConfig {

public:

#ifdef GITCOMMIT
    static constexpr const char* GIT_REV = XSTRINGIFY(GITCOMMIT);
#else
    static constexpr const char* GIT_REV = "N/A";
#endif

#ifdef GITTAG
    static constexpr const char* VERSION = XSTRINGIFY(GITTAG);
#else
    static constexpr const char* VERSION = "0.0";
#endif

#ifdef BUILDDATE
    static constexpr const char* BUILD_DATE = XSTRINGIFY(BUILDDATE);
#else
    static constexpr const char* BUILD_DATE = "0000-00-00 00:00:00";
#endif

};

#undef STRINGIFY
#undef XSTRINGIFY

#endif
