#ifndef __CpuId_include__
#define __CpuId_include__


class CpuId {

public:

    CpuId() = delete;

    CpuId (const CpuId &other) = delete;

    CpuId &operator=(const CpuId &other) = delete;

    virtual ~CpuId() = delete;

    static bool isSupported();

};


#endif
