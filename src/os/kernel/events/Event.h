#ifndef __Event_include__
#define __Event_include__

#include <cstdint>

class Event {

public:

    explicit Event(uint32_t type) : type(type) {}

    Event(const Event &other);

    virtual ~Event() = default;

    uint32_t    getType() const;

    bool        is(uint32_t type);

    virtual char* getName() = 0;

private:

    uint32_t    type;

};


#endif
