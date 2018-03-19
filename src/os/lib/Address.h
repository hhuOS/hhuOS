#ifndef __Address_include__
#define __Address_include__


#include <cstdint>

class Address {

public:

    Address();

    Address(uint32_t address);

    ~Address() = default;

    Address(const Address &other) = default;

    Address &operator=(const Address &other) = default;

    bool operator!=(const Address &other) const;

    bool operator==(const Address &other) const;

    explicit operator uint32_t() const;

private:

    uint32_t address;

};


#endif
