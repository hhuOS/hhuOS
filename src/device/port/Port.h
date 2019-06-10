#ifndef HHUOS_SIMPLEPORT_H
#define HHUOS_SIMPLEPORT_H

#include <cstdint>
#include "lib/string/String.h"

class Port {

public:

    Port() = default;

    Port(const Port &copy) = delete;

    ~Port() = default;

    /**
     * Send data via the port.
     *
     * @param data Pointer to the data
     * @param len The amount of bytes to be sent
     */
    virtual void sendData(char *data, uint32_t len);

    /**
     * Read data from the port.
     * This function is blocking and will return only if len bytes have been received!
     *
     * @param data Buffer to which the received data will be written.
     * @param len The amount of bytes to read
     */
    virtual void readData(char *data, uint32_t len);

    /**
     * Read a single character from the port.
     * This function is blocking and will return only if one byte has been received!
     *
     * @return The read character
     */
    virtual char readChar() = 0;

    /**
     * Send a single character via the port.
     *
     * @param c The character to send
     */
    virtual void sendChar(char c) = 0;

    /**
     * Get the port's name (e.g. 'COM1', 'LPT2', ...).
     */
    virtual String getName() = 0;

};

#endif
