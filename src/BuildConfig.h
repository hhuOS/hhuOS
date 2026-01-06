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

#ifndef HHUOS_BUILDCONFIG_H
#define HHUOS_BUILDCONFIG_H

class BuildConfig {

public:
    /**
     * Get the version number.
     *
     * @return The version number
     */
    static inline const char* getVersion() {
        return VERSION;
    }
    /**
     * Get the version codename.
     *
     * @return The version codename
     */
    static inline const char* getCodename() {
        return CODENAME;
    }

    /**
     * Get the git revision.
     *
     * @return The git revision
     */
    static inline const char* getGitRevision() {
        return GIT_REV;
    }

    /**
     * Get the git branch.
     *
     * @return The git branch
     */
    static inline const char* getGitBranch() {
        return GIT_BRANCH;
    }

    /**
     * Get the build date.
     *
     * @return The build date
     */
    static inline const char* getBuildDate() {
        return BUILD_DATE;
    }

    /**
     * Get the build type.
     *
     * @return The build type
     */
    static inline const char* getBuildType() {
        return BUILD_TYPE;
    }

private:

    static const char *VERSION;
    static const char *CODENAME;
    static const char *GIT_REV;
    static const char *GIT_BRANCH;
    static const char *BUILD_DATE;
    static const char *BUILD_TYPE;
};

#undef STRINGIFY
#undef XSTRINGIFY

#endif
