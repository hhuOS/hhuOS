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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ClassiCube/src/Core.h"
#undef CC_BUILD_NETWORKING
#include "ClassiCube/src/main_impl.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/Logger.h"
#include "ClassiCube/src/Utils.h"
#include "ClassiCube/src/Errors.h"
#include "ClassiCube/src/Platform.h"
#include "ClassiCube/src/_PlatformBase.h"

#include <util/async/Thread.h>
#include <util/time/Timestamp.h>
#include <util/base/String.h>
#include <util/collection/Array.h>
#include <util/io/file/File.h>
#include <util/time/Date.h>

cc_uint8 Platform_Flags = PLAT_FLAG_SINGLE_PROCESS | PLAT_FLAG_APP_EXIT;

const char* Platform_AppNameSuffix = " hhuOS";
cc_bool Platform_ReadonlyFilesystem = false;

const cc_result ReturnCode_FileNotFound = ENOENT;
const cc_result ReturnCode_DirectoryExists = EEXIST;
const cc_result ReturnCode_PathNotFound = 99999; // Same as MSDOS
const cc_result ReturnCode_FileShareViolation = 1000000000; // Same as MSDOS

/*##################################################################################################################*
*--------------------------------------------------Main entrypoint--------------------------------------------------*
*###################################################################################################################*/

int32_t main(const int32_t argc, char **argv) {
	SetupProgram(argc, argv);

	while (true) {
		RunProgram(argc, argv);
	}
}

/*##################################################################################################################*
*---------------------------------------------------Logging/Time----------------------------------------------------*
*###################################################################################################################*/

void Platform_Log(const char*, const int) {}

TimeMS DateTime_CurrentUTC() {
	return Util::Time::Date().getUnixTime();
}

void DateTime_CurrentLocal(cc_datetime *t) {
	const auto date = Util::Time::Date();
	t->year = date.getYear();
	t->month = date.getMonth();
	t->day = date.getDayOfMonth();
	t->hour = date.getHours();
	t->minute = date.getMinutes();
	t->second = date.getSeconds();
}

/*##################################################################################################################*
*----------------------------------------------------Crash handling-------------------------------------------------*
*###################################################################################################################*/

void CrashHandler_Install() {}

void Process_Abort2(const cc_result result, const char *raw_msg) {
	Logger_DoAbort(result, raw_msg, nullptr);
}

/*##################################################################################################################*
*-----------------------------------------------------Stopwatch-----------------------------------------------------*
*###################################################################################################################*/

cc_uint64 Stopwatch_Measure() {
	return Util::Time::Timestamp::getSystemTime().toMicroseconds();
}

cc_uint64 Stopwatch_ElapsedMicroseconds(const cc_uint64 beg, const cc_uint64 end) {
	if (end < beg) {
		return 0;
	}

	return end - beg;
}

/*##################################################################################################################*
*--------------------------------------------------Directory/File---------------------------------------------------*
*###################################################################################################################*/

void Platform_EncodePath(cc_filepath *dst, const cc_string *src) {
	String_EncodeUtf8(dst->buffer, src);
}

void Platform_DecodePath(cc_string *dst, const cc_filepath *path) {
	String_AppendUtf8(dst, path->buffer, String_Length(path->buffer));
}

void Directory_GetCachePath(cc_string*) {}

cc_result Directory_Create2(const cc_filepath *path) {
	const auto file = Util::Io::File(path->buffer);
	if (file.exists()) {
		return ReturnCode_DirectoryExists;
	}

	return file.create(Util::Io::File::DIRECTORY) ? 0 : -1;
}

int File_Exists(const cc_filepath *path) {
	const auto file = Util::Io::File(path->buffer);
	return file.exists() ? 1 : 0;
}

cc_result Directory_Enum(const cc_string *path, void *obj, const Directory_EnumCallback callback) {
	const auto file = Util::Io::File(path->buffer);
	if (!file.exists() || !file.isDirectory()) {
		return ReturnCode_FileNotFound;
	}

	const auto basePath = file.getCanonicalPath();
	for (auto &child : file.getChildren()) {
		const auto isDir = child.isDirectory() ? 1 : 0;
		callback(reinterpret_cast<const cc_string*>(static_cast<const char*>(child.getName())), obj, isDir);
	}

	return 0;
}

cc_result File_Open(cc_file *file, const cc_filepath *path) {
	*file = reinterpret_cast<cc_file>(fopen(path->buffer, "r"));
	return *file == 0 ? ENOENT : 0;
}
cc_result File_Create(cc_file *file, const cc_filepath *path) {
	*file = reinterpret_cast<cc_file>(fopen(path->buffer, "w"));
	return *file == 0 ? ENOENT : 0;
}
cc_result File_OpenOrCreate(cc_file *file, const cc_filepath *path) {
	*file = reinterpret_cast<cc_file>(fopen(path->buffer, "rw"));
	return *file == 0 ? ENOENT : 0;
}

cc_result File_Read(const cc_file file, void *data, const cc_uint32 count, cc_uint32 *bytesRead) {
	*bytesRead = fread(data, 1, count, reinterpret_cast<FILE*>(file));
	if (*bytesRead == 0 && ferror(reinterpret_cast<FILE*>(file))) {
		return -1;
	}

	return 0;
}

cc_result File_Write(const cc_file file, const void *data, const cc_uint32 count, cc_uint32 *bytesWrote) {
	*bytesWrote = fwrite(data, 1, count, reinterpret_cast<FILE*>(file));
	if (*bytesWrote == 0 && ferror(reinterpret_cast<FILE*>(file))) {
		return -1;
	}

	return 0;
}

cc_result File_Close(const cc_file file) {
	return fclose(reinterpret_cast<FILE*>(file));
}

cc_result File_Seek(const cc_file file, const int offset, const int seekType) {
	static constexpr cc_uint8 modes[3] = { SEEK_SET, SEEK_CUR, SEEK_END };
	return fseek(reinterpret_cast<FILE*>(file), offset, modes[seekType]);
}

cc_result File_Position(const cc_file file, cc_uint32 *pos) {
	return fgetpos(reinterpret_cast<FILE*>(file), reinterpret_cast<fpos_t*>(pos));
}

cc_result File_Length(const cc_file file, cc_uint32 *len) {
	const long pos = ftell(reinterpret_cast<FILE*>(file));

	fseek (reinterpret_cast<FILE*>(file), 0, SEEK_END);
	*len = ftell(reinterpret_cast<FILE*>(file));
	fseek (reinterpret_cast<FILE*>(file), pos, SEEK_SET);

	return 0;
}

/*##################################################################################################################*
*-----------------------------------------------------Threading-----------------------------------------------------*
*###################################################################################################################*/

void Thread_Sleep(const cc_uint32 milliseconds) {
	const auto time = Util::Time::Timestamp::ofMilliseconds(milliseconds);
	Util::Async::Thread::sleep(time);
}

void Thread_Run(void **handle, Thread_StartFunc, int, const char*) {
	*handle = nullptr;
}

void Thread_Detach(void*) {}

void Thread_Join(void*) {}

void* Mutex_Create(const char*) {
	return nullptr;
}

void Mutex_Free(void*) {}

void Mutex_Lock(void*) {}

void Mutex_Unlock(void*) {}

void* Waitable_Create(const char*) {
	return nullptr;
}

void Waitable_Free(void*) {}

void Waitable_Signal(void*) {}

void Waitable_Wait(void*) {}

void Waitable_WaitFor(void*, cc_uint32) {}

/*##################################################################################################################*
*-----------------------------------------------------Font/Text-----------------------------------------------------*
*###################################################################################################################*/

void Platform_LoadSysFonts() {}

/*##################################################################################################################*
*------------------------------------------------------Socket-------------------------------------------------------*
*###################################################################################################################*/

cc_bool SockAddr_ToString(const cc_sockaddr*, cc_string*) {
	return false;
}

cc_bool ParseIPv4(const cc_string*, int, cc_sockaddr*) {
	return false;
}

cc_bool ParseIPv6(const char*, int, cc_sockaddr*) {
	return false;
}

cc_result ParseHost(const char*, int, cc_sockaddr*, int*) {
	return ERR_NOT_SUPPORTED;
}

/*##################################################################################################################*
*--------------------------------------------------Process/Module---------------------------------------------------*
*###################################################################################################################*/

cc_bool Process_OpenSupported = false;

cc_result Process_StartGame2(const cc_string *args, const int numArgs) {
	return SetGameArgs(args, numArgs);
}

void Process_Exit(const cc_result code) {
	exit(static_cast<int>(code));
}

cc_result Process_StartOpen(const cc_string*) {
	return ERR_NOT_SUPPORTED;
}

/*##################################################################################################################*
*-----------------------------------------------------Platform------------------------------------------------------*
*###################################################################################################################*/

void Platform_Free() {}

cc_bool Platform_DescribeError(const cc_result res, cc_string *dst) {
	const char *error = strerror(static_cast<int>(res));
	String_AppendUtf8(dst, error, String_Length(error));

	return true;
}

void Platform_Init() {}

/*##################################################################################################################*
*----------------------------------------------------Encryption-----------------------------------------------------*
*###################################################################################################################*/

cc_result Platform_Encrypt(const void*, int, cc_string*) {
	return ERR_NOT_SUPPORTED;
}

cc_result Platform_Decrypt(const void*, int, cc_string*) {
	return ERR_NOT_SUPPORTED;
}

cc_result Platform_GetEntropy(void*, int) {
	return ERR_NOT_SUPPORTED;
}

/*##################################################################################################################*
*--------------------------------------------------Configuration----------------------------------------------------*
*###################################################################################################################*/

int Platform_GetCommandLineArgs(int argc, STRING_REF char **argv, cc_string *args) {
	if (gameHasArgs) {
		return GetGameArgs(args);
	}

	// Skip first argument (program name)
	argc--;
	argv++;

	// Copy arguments
	const int count = argc < GAME_MAX_CMDARGS ? argc : GAME_MAX_CMDARGS;
	for (int i = 0; i < count; i++) {
		args[i] = String_FromReadonly(argv[i]);
	}

	return count;
}

cc_result Platform_SetDefaultCurrentDirectory(int, char**) {
	return Util::Io::File::changeDirectory("/user/classicube/") ? 0 : ReturnCode_FileNotFound;
}