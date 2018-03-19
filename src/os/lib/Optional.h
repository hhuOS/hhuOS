#ifndef __Optional_include__
#define __Optional_include__

/**
 * @author Filip Krakowski
 */
template <typename T>
class Optional {

public:

    Optional() = default;

    explicit Optional(const T &element);

    bool isNull();

    T value();

private:

    T element;

    bool isInitialized = false;

};

template <class T>
Optional<T>::Optional(const T &element) {
    this->element = element;
    this->isInitialized = true;
}

template <class T>
bool Optional<T>::isNull() {
    return !isInitialized;
}

template <class T>
T Optional<T>::value() {
    return element;
}


#endif
