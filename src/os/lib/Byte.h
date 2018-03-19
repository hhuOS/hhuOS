#ifndef __BYTE_H__
#define __BYTE_H__

typedef unsigned char byte;

/**
 * Byte.
 * This Byte class contents some bit shift methodes and get, set, toggle and
 * delete methode. 
 */
class Byte {
private:
    byte _byte;

public:
    Byte() {}

    Byte(byte b) {
        set(b);
    }

    /**
     * copy constructor
     */
    Byte(const Byte & copy) {
        set(copy._byte);
    }

    /**
     * Assignment (Byte) b = (Byte) src;
     */
    Byte & operator=(const Byte & src) {
        set(src._byte);
        return *this;
    }

    /**
     * Assignment (Byte) b = (char) src;
     */
    Byte & operator=(const char & src) {
        set(src);
        return *this;
    }

    /**
     * Assignment (Byte) b = (unsigned char) src;
     */
    Byte & operator=(const byte & src) {
        set(src);
        return *this;
    }

    virtual ~Byte() {}
    void operator delete(void*, unsigned int) {}

    byte get() {
        return _byte;
    }

    /**
     * set all 8 bits
     */
    Byte & set(const byte & b) {
        _byte = b;
        return *this;
    }

    /**
     * set all 8 bits
     */
    Byte & set(const Byte & b) {
        _byte = b._byte;
        return *this;
    }

    /**
     * set bit.
     * set it to 1 or val
     * @param bit value 0..7 MSB
     * @param val default = true (1)
     */
    Byte & sBit(const int & bit, bool val = true) {
        if (val) {
            _byte |= 1 << bit;
        } else {
            _byte &= ~(1 << bit);
        }
        return *this;
    }

    /**
     * delete bit.
     * set it to 0
     * @param bit value 0..7 MSB
     */
    Byte & dBit(const int & bit) {
        _byte &= ~(1 << bit);
        return *this;
    }

    /**
     * toggle bit.
     * @param bit value 0..7 MSB
     */
    Byte & tBit(const int & bit) {
        _byte ^= 1 << bit;
        return *this;
    }

    /**
     * get bit.
     * @param bit value 0..7 MSB
     */
    bool bit(const int & bitNr) {
        return ((_byte >> bitNr) & 0x01);
    }
    
    /**
     * get a bit like an array.
     */
    bool operator[](const int & bitNr) {
        return bit(bitNr);
    }

};

#endif
