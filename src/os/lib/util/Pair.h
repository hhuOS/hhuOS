#ifndef __Pair_include__
#define __Pair_include__

namespace Util {

    /**
     * @author Filip Krakowski
     */
    template <typename T, typename U>
    class Pair {

    public:

        Pair() = default;

        Pair(T first, U second);

        ~Pair() = default;

        Pair(const Pair &other) = default;

        Pair &operator=(const Pair &other) = default;

        bool operator!=(const Pair &other) const;

        bool operator==(const Pair &other) const;

        explicit operator uint32_t() const;

        T first;

        U second;
    };

    template <typename T, typename U>
    Pair<T,U>::Pair(T first, U second) : first(first), second(second) {}

    template<typename T, typename U>
    bool Pair<T, U>::operator!=(const Pair &other) const {
        return first != other.first || second != other.second;

    }

    template<typename T, typename U>
    bool Pair<T, U>::operator==(const Pair &other) const {
        return first == other.first && second == other.second;
    }

    template<typename T, typename U>
    Pair<T, U>::operator uint32_t() const {

        uint32_t k1 = (uint32_t) first;

        uint32_t k2 = (uint32_t) second;

        return (((k1 + k2) * (k1 + k2 + 1)) / 2) + k2;
    };
}


#endif
