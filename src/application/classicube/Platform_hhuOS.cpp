#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "application/classicube/ClassiCube/src/Core.h"
#include "application/classicube/ClassiCube/src/_PlatformBase.h"
#include "application/classicube/ClassiCube/src/Utils.h"
#include "application/classicube/ClassiCube/src/Errors.h"
#include "lib/util/async/Thread.h"
#include "lib/util/base/System.h"
#include "lib/util/time/Timestamp.h"
#include "application/classicube/ClassiCube/src/Constants.h"
#include "application/classicube/ClassiCube/src/Logger.h"
#include "application/classicube/ClassiCube/src/Platform.h"
#include "application/classicube/ClassiCube/src/String.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/time/Date.h"

#undef CC_BUILD_NETWORKING

const char* Platform_AppNameSuffix = " hhuOS";
cc_bool Platform_SingleProcess = true;
cc_bool Platform_ReadonlyFilesystem = false;

const cc_result ReturnCode_FileNotFound = ENOENT;
const cc_result ReturnCode_DirectoryExists = EEXIST;
const cc_result ReturnCode_FileShareViolation = 1000000000; // Same as MSDOS
const cc_result ReturnCode_SocketInProgess  = -10002; // Same as MSDOS
const cc_result ReturnCode_SocketWouldBlock = -10002; // Same as MSDOS
const cc_result ReturnCode_SocketDropped    = -10002; // Same as MSDOS

/*########################################################################################################################*
*---------------------------------------------------------Memory----------------------------------------------------------*
*#########################################################################################################################*/

void* Mem_Set(void *dst, const cc_uint8 value, const unsigned numBytes) {
	return memset(dst, value, numBytes);
}

void* Mem_Copy(void *dst, const void *src, const unsigned numBytes) {
	return memcpy(dst, src, numBytes);
}

void* Mem_Move(void* dst, const void *src, const unsigned numBytes) {
	return memmove(dst, src, numBytes);
}

void* Mem_TryAlloc(const cc_uint32 numElems, const cc_uint32 elemsSize) {
	const cc_uint32 size = CalcMemSize(numElems, elemsSize);
	return size ? malloc(size) : nullptr;
}

void* Mem_TryAllocCleared(const cc_uint32 numElems, const cc_uint32 elemsSize) {
	return calloc(numElems, elemsSize);
}

void* Mem_TryRealloc(void *mem, const cc_uint32 numElems, const cc_uint32 elemsSize) {
	const cc_uint32 size = CalcMemSize(numElems, elemsSize);
	return size ? realloc(mem, size) : nullptr;
}

void Mem_Free(void *mem) {
	free(mem);
}

/*########################################################################################################################*
*------------------------------------------------------Logging/Time-------------------------------------------------------*
*#########################################################################################################################*/

void Platform_Log(const char *msg, const int len) {
	for (int i = 0; i < len; i++) {
		Util::System::out << msg[i];
	}

	Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
}

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

void Directory_GetCachePath([[maybe_unused]] cc_string *path) {}

cc_result Directory_Create(const cc_filepath *path) {
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
	for (const auto &childName : file.getChildren()) {
		auto child = Util::Io::File(basePath + "/" + childName);
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

cc_result Socket_ParseAddress([[maybe_unused]] const cc_string *address, [[maybe_unused]] int port, [[maybe_unused]] cc_sockaddr *addrs, [[maybe_unused]] int *numValidAddrs) {
	return ERR_NOT_SUPPORTED;
}

cc_result Socket_Create([[maybe_unused]] cc_socket *s, [[maybe_unused]] cc_sockaddr *addr, [[maybe_unused]] cc_bool nonblocking) {
	return ERR_NOT_SUPPORTED;
}

cc_result Socket_Connect([[maybe_unused]] cc_socket s, [[maybe_unused]] cc_sockaddr *addr) {
	return ERR_NOT_SUPPORTED;
}

cc_result Socket_Read([[maybe_unused]] cc_socket s, [[maybe_unused]] cc_uint8 *data, [[maybe_unused]] cc_uint32 count, [[maybe_unused]] cc_uint32 *modified) {
	return ERR_NOT_SUPPORTED;
}

cc_result Socket_Write([[maybe_unused]] cc_socket s, [[maybe_unused]] const cc_uint8 *data, [[maybe_unused]] cc_uint32 count, [[maybe_unused]] cc_uint32 *modified) {
	return ERR_NOT_SUPPORTED;
}

void Socket_Close([[maybe_unused]] cc_socket s) {}

cc_result Socket_CheckReadable([[maybe_unused]] cc_socket s, [[maybe_unused]] cc_bool *readable) {
	return ERR_NOT_SUPPORTED;
}

cc_result Socket_CheckWritable([[maybe_unused]] cc_socket s, [[maybe_unused]] cc_bool *writable) {
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
*--------------------------------------------------------Updater----------------------------------------------------------*
*#########################################################################################################################*/

cc_bool Updater_Supported = false;

cc_bool Updater_Clean() {
	return true;
}

const UpdaterInfo Updater_Info = { "&eCompile latest source code to update", 0, {} };

cc_result Updater_Start([[maybe_unused]] const char **action) {
	return ERR_NOT_SUPPORTED;
}

cc_result Updater_GetBuildTime([[maybe_unused]] cc_uint64 *timestamp) {
	return ERR_NOT_SUPPORTED;
}

cc_result Updater_MarkExecutable() {
	return ERR_NOT_SUPPORTED;
}

cc_result Updater_SetNewBuildTime([[maybe_unused]] cc_uint64 timestamp) {
	return ERR_NOT_SUPPORTED;
}


/*########################################################################################################################*
*-------------------------------------------------------Dynamic lib-------------------------------------------------------*
*#########################################################################################################################*/

const cc_string DynamicLib_Ext = String_FromConst(".dll");

void* DynamicLib_Load2([[maybe_unused]] const cc_string *path) {
	return nullptr;
}

void* DynamicLib_Get2([[maybe_unused]] void *lib, [[maybe_unused]] const char *name) {
	return nullptr;
}

cc_bool DynamicLib_DescribeError([[maybe_unused]] cc_string *dst) {
	return false;
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
