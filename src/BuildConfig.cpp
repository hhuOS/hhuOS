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

#undef STRINGIFY
#undef XSTRINGIFY