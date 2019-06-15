#ifndef HHUOS_RESULT_H
#define HHUOS_RESULT_H

#include <cstdint>

namespace Standard::System {

class Result {

public:

    enum Status {
        OK,
        NOT_INITIALIZED
    };

public:

    Result();

    uint32_t getStatus() const;

    void setStatus(Status status);

private:

    Status status;
};

}

#endif
