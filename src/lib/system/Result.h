#ifndef HHUOS_RESULT_H
#define HHUOS_RESULT_H

#include <cstdint>

namespace Standard::System {

class Result {

public:

    Result();

    uint32_t getStatus() const;

    uint32_t getValue() const;

    void setStatus(uint32_t status);

    void setValue(uint32_t value);

private:

    uint32_t status;
    uint32_t value;
};

}

#endif
