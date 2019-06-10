#include "Game.h"

Game::Game() {
    removeList = new Util::ArrayList<GameObject*>();
    addList = new Util::ArrayList<GameObject*>();
    gameObjects = new Util::ArrayList<GameObject*>();
    isRunning = true;
};

Game::~Game(){
    delete removeList;
    delete addList;
    delete gameObjects;
}

void Game::addGameObject(GameObject &gameObject) {
    addList->add(&gameObject);
}

void Game::removeGameObject(GameObject &gameObject) {
    removeList->add(&gameObject);
}

void Game::applyChanges() {

    GameObject *go;
    for (uint32_t i = 0; i < addList->size(); i++) {
        go = addList->get(i);
        gameObjects->add(go);
    }

    for (uint32_t i = 0; i < removeList->size(); i++) {
        go = removeList->get(i);
        gameObjects->remove(go);
    }

    removeList->clear();
    addList->clear();
}