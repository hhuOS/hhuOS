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
