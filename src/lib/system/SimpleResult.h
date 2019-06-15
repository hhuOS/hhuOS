#ifndef HHUOS_SIMPLERESULT_H
#define HHUOS_SIMPLERESULT_H

#include "Result.h"

namespace Standard::System {

template<typename T>
class SimpleResult : public Result {

public:

    SimpleResult() = default;

    void setValue(T value);

    T getValue();

private:

    T value;

};

template<typename T>
void SimpleResult<T>::setValue(T value) {
    this->value = value;
}

template<typename T>
T SimpleResult<T>::getValue() {
    return value;
}

}

#endif
