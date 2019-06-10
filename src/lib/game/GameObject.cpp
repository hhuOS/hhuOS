#include "GameObject.h"

GameObject::GameObject(Vector2 position, char* tag) : position(position), tag(tag) {

}

GameObject::~GameObject(){
    if (collider) {
        delete collider;
    }
}

void GameObject::onCollisionEnter(GameObject &other) {

}

void GameObject::onCollisionExit() {

}

Vector2 GameObject::getPosition() {
    return position;
}

char* GameObject::getTag() {
    return tag;
}

void GameObject::move(float x, float y) {
    this->position.set(position.getX() + x, position.getY() + y);

    if (collider) {
        collider->setPosition(position.getX() + x, position.getY() + y);
    }
}

void GameObject::setPosition(float x, float y) {
    this->position.set(x, y);
}

void GameObject::setPosition(Vector2 position) {
    this->position = position;
}