#ifndef __HashNode_include__
#define __HashNode_include__

namespace Util {

    template <typename K, typename V>
    class HashNode {
    public:

        HashNode(const K &key, const V &value);

        HashNode(const HashNode<K, V> &other) = delete;

        HashNode<K, V> &operator=(const HashNode<K, V> &other) = delete;

        ~HashNode() = default;

        K getKey() const;

        V getValue() const;

        void setValue(const V &value);

        HashNode *getNext() const;

        void setNext(HashNode *next);

    private:

        K key;

        V value;

        HashNode *next;

    };

    template <class K, class V>
    K HashNode<K,V>::getKey() const {
        return key;
    }

    template <class K, class V>
    V HashNode<K,V>::getValue() const {
        return value;
    }

    template <class K, class V>
    void HashNode<K,V>::setValue(const V &value) {
        this->value = value;
    }

    template <class K, class V>
    HashNode<K,V> *HashNode<K,V>::getNext() const {
        return next;
    }

    template <class K, class V>
    void HashNode<K,V>::setNext(HashNode *next) {
        this->next = next;
    }

    template <class K, class V>
    HashNode<K,V>::HashNode(const K &key, const V &value) : key(key), value(value) {
        next = nullptr;
    }

}


#endif
