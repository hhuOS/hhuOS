#include "KeyEvent.h"

KeyEvent::KeyEvent(Key key) : Event(TYPE) {
    this->key = key;
}

KeyEvent::KeyEvent() : Event(TYPE) {

}

char *KeyEvent::getName() {
    return "KeyEvent";
}

Key KeyEvent::getKey() {
    return key;
}

KeyEvent::KeyEvent(const KeyEvent &other) : Event(other){
    this->key = other.key;
}
