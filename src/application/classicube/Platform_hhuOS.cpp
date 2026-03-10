#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ClassiCube/src/Core.h"
#undef CC_BUILD_NETWORKING
#include "ClassiCube/src/_PlatformBase.h"
#include "ClassiCube/src/Utils.h"
#include "ClassiCube/src/Errors.h"
#include "ClassiCube/src/main_impl.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/Logger.h"
#include "ClassiCube/src/Platform.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/time/Date.h"

cc_uint8 Platform_Flags = PLAT_FLAG_SINGLE_PROCESS | PLAT_FLAG_APP_EXIT;

const char* Platform_AppNameSuffix = " hhuOS";
cc_bool Platform_ReadonlyFilesystem = false;

const cc_result ReturnCode_FileNotFound = ENOENT;
const cc_result ReturnCode_DirectoryExists = EEXIST;
const cc_result ReturnCode_PathNotFound = 99999; // Same as MSDOS
const cc_result ReturnCode_FileShareViolation = 1000000000; // Same as MSDOS
const cc_result ReturnCode_SocketInProgess  = -10002; // Same as MSDOS
const cc_result ReturnCode_SocketWouldBlock = -10002; // Same as MSDOS
const cc_result ReturnCode_SocketDropped    = -10002; // Same as MSDOS

/*########################################################################################################################*
*-----------------------------------------------------Main entrypoint-----------------------------------------------------*
*#########################################################################################################################*/

int32_t main(const int32_t, char **) {
	constexpr int32_t argc = 2;
	char* argv[] = {const_cast<char*>("classicube"), const_cast<char*>("--singleplayer")};

	SetupProgram(argc, argv);
	while (true) {
		RunProgram(argc, argv);
	}
}

/*########################################################################################################################*
*------------------------------------------------------Logging/Time-------------------------------------------------------*
*#########################################################################################################################*/

void Platform_Log(const char *, const int) {}

TimeMS DateTime_CurrentUTC() {
	return Util::Time::Date().getUnixTime();
}

void DateTime_CurrentLocal(cc_datetime* t) {
	const auto date = Util::Time::Date();
	t->year = date.getYear();
	t->month = date.getMonth();
	t->day = date.getDayOfMonth();
	t->hour = date.getHours();
	t->minute = date.getMinutes();
	t->second = date.getSeconds();
}

/*########################################################################################################################*
*-------------------------------------------------------Crash handling----------------------------------------------------*
*#########################################################################################################################*/

void CrashHandler_Install() {}

void Process_Abort2(const cc_result result, const char *raw_msg) {
	Logger_DoAbort(result, raw_msg, nullptr);
}

/*########################################################################################################################*
*--------------------------------------------------------Stopwatch--------------------------------------------------------*
*#########################################################################################################################*/

cc_uint64 Stopwatch_Measure() {
	return Util::Time::Timestamp::getSystemTime().toMicroseconds();
}

cc_uint64 Stopwatch_ElapsedMicroseconds(const cc_uint64 beg, const cc_uint64 end) {
	if (end < beg) {
		return 0;
	}

	return end - beg;
}

/*########################################################################################################################*
*-----------------------------------------------------Directory/File------------------------------------------------------*
*#########################################################################################################################*/

void Platform_EncodePath(cc_filepath *dst, const cc_string *src) {
	String_EncodeUtf8(dst->buffer, src);
}

void Platform_DecodePath(cc_string* dst, const cc_filepath* path) {
	const char* str = path->buffer;
	String_AppendUtf8(dst, str, String_Length(str));
}

void Directory_GetCachePath([[maybe_unused]] cc_string *path) {}

cc_result Directory_Create(const cc_filepath *path) {
	auto file = Util::Io::File(path->buffer);
	if (file.exists()) {
		return ReturnCode_DirectoryExists;
	}

	return file.create(Util::Io::File::DIRECTORY) ? 0 : -1;
}

cc_result Directory_Create2(const cc_filepath* path) {
	auto file = Util::Io::File(path->buffer);
	if (file.exists()) {
		return ReturnCode_DirectoryExists;
	}

	return file.create(Util::Io::File::DIRECTORY) ? 0 : -1;
}

int File_Exists(const cc_filepath *path) {
	auto file = Util::Io::File(path->buffer);
	return file.exists() ? 1 : 0;
}

cc_result Directory_Enum(const cc_string *path, void *obj, const Directory_EnumCallback callback) {
	auto file = Util::Io::File(path->buffer);
	if (!file.exists() || !file.isDirectory()) {
		return ReturnCode_FileNotFound;
	}

	const auto basePath = file.getCanonicalPath();
	for (auto &child : file.getChildren()) {
		callback(reinterpret_cast<const cc_string*>(static_cast<const char*>(child.getName())), obj, child.isDirectory() ? 1 : 0);
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

/*########################################################################################################################*
*--------------------------------------------------------Threading--------------------------------------------------------*
*#########################################################################################################################*/

void Thread_Sleep(const cc_uint32 milliseconds) {
	const auto time = Util::Time::Timestamp::ofMilliseconds(milliseconds);
	Util::Async::Thread::sleep(time);
}

void Thread_Run([[maybe_unused]] void **handle, [[maybe_unused]] Thread_StartFunc func, [[maybe_unused]] int stackSize, [[maybe_unused]] const char *name) {
	*handle = nullptr;
}

void Thread_Detach([[maybe_unused]] void *handle) {}

void Thread_Join([[maybe_unused]] void *handle) {}

void* Mutex_Create([[maybe_unused]] const char *name) {
	return nullptr;
}

void Mutex_Free([[maybe_unused]] void *handle) {}

void Mutex_Lock([[maybe_unused]] void *handle) {}

void Mutex_Unlock([[maybe_unused]] void *handle) {}

void* Waitable_Create([[maybe_unused]] const char *name) {
	return nullptr;
}

void Waitable_Free([[maybe_unused]] void *handle) {}

void Waitable_Signal([[maybe_unused]] void *handle) {}

void Waitable_Wait([[maybe_unused]] void *handle) {}

void Waitable_WaitFor([[maybe_unused]] void *handle, [[maybe_unused]] cc_uint32 milliseconds) {}

/*########################################################################################################################*
*--------------------------------------------------------Font/Text--------------------------------------------------------*
*#########################################################################################################################*/

void Platform_LoadSysFonts() {}

/*########################################################################################################################*
*---------------------------------------------------------Socket----------------------------------------------------------*
*#########################################################################################################################*/

cc_bool SockAddr_ToString([[maybe_unused]] const cc_sockaddr* addr, [[maybe_unused]] cc_string* dst) {
	return false;
}

cc_bool ParseIPv4([[maybe_unused]] const cc_string* ip, [[maybe_unused]] int port, [[maybe_unused]] cc_sockaddr* dst) {
	return false;
}

cc_bool ParseIPv6([[maybe_unused]] const char* ip, [[maybe_unused]] int port, [[maybe_unused]] cc_sockaddr* dst) {
	return false;
}

cc_result ParseHost([[maybe_unused]] const char* host, [[maybe_unused]] int port, [[maybe_unused]] cc_sockaddr* addrs, [[maybe_unused]] int* numValidAddrs) {
	return ERR_NOT_SUPPORTED;
}

/*########################################################################################################################*
*-----------------------------------------------------Process/Module------------------------------------------------------*
*#########################################################################################################################*/

cc_bool Process_OpenSupported = false;

cc_result Process_StartGame2(const cc_string *args, const int numArgs) {
	return SetGameArgs(args, numArgs);
}

void Process_Exit(const cc_result code) {
	exit(static_cast<int>(code));
}

cc_result Process_StartOpen([[maybe_unused]] const cc_string *args) {
	return ERR_NOT_SUPPORTED;
}

/*########################################################################################################################*
*--------------------------------------------------------Platform---------------------------------------------------------*
*#########################################################################################################################*/

void Platform_Free() {}

cc_bool Platform_DescribeError(const cc_result res, cc_string *dst) {
	const char *error = strerror(static_cast<int>(res));
	String_AppendUtf8(dst, error, String_Length(error));

	return true;
}

void Platform_Init() {}

/*########################################################################################################################*
*-------------------------------------------------------Encryption--------------------------------------------------------*
*#########################################################################################################################*/

cc_result Platform_Encrypt([[maybe_unused]] const void *data, [[maybe_unused]] int len, [[maybe_unused]] cc_string *dst) {
	return ERR_NOT_SUPPORTED;
}

cc_result Platform_Decrypt([[maybe_unused]] const void *data, [[maybe_unused]] int len, [[maybe_unused]] cc_string *dst) {
	return ERR_NOT_SUPPORTED;
}

cc_result Platform_GetEntropy([[maybe_unused]] void *data, [[maybe_unused]] int len) {
	return ERR_NOT_SUPPORTED;
}

/*########################################################################################################################*
*-----------------------------------------------------Configuration-------------------------------------------------------*
*#########################################################################################################################*/

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

cc_result Platform_SetDefaultCurrentDirectory([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
	return Util::Io::File::changeDirectory("/user/classicube/") ? 0 : ReturnCode_FileNotFound;
}