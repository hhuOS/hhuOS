#ifndef __Pair_include__
#define __Pair_include__

template <typename T, typename U>
class Pair {

public:

    Pair(T &first, U &second);

    T &first;

    U &second;

    bool operator==(Pair &other);
};

template <class T, class U>
Pair<T,U>::Pair(T &first, U &second) : first(first), second(second) {}

template <class T, class U>
bool Pair<T,U>::operator==(Pair &other) {
    return first == other.first && second == other.second;
};

#endif
