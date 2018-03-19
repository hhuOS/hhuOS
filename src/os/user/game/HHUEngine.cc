#include <kernel/Kernel.h>
#include <kernel/services/InputService.h>
#include <kernel/services/TimeService.h>
#include "user/game/HHUEngine.h"

Random HHUEngine::random(42, 32768);
Game* HHUEngine::currentGame;

bool HHUEngine::isKeyPressed(int scancode){


  return ((InputService*)Kernel::getService(InputService::SERVICE_NAME))->getKeyboard()->isKeyPressed(scancode);
}

void HHUEngine::invalidateInput(){
  ((InputService*)Kernel::getService(InputService::SERVICE_NAME))->getKeyboard()->resetLast();
}

unsigned long HHUEngine::time(){
  return ((TimeService*)Kernel::getService(TimeService::SERVICE_NAME))->getSystemTime();
}

float HHUEngine::rand(){
  return random.rand(32768) / 32768.0f; // [0 .. 0,999]
}

void HHUEngine::setSeed(unsigned int seed){
  random.setSeed(seed);
}

void HHUEngine::beep(){
  // TODO(krakowski):
  //    Implement beeping
}

void HHUEngine::setCurrentGame(Game* game){
  currentGame = game;
}

bool HHUEngine::isGameSet(){
  return currentGame != 0x0;
}

void HHUEngine::instantiate(GameObject* gameObject){
  currentGame->addGameObject(*gameObject);
}

void HHUEngine::destroy(GameObject* gameObject){
  currentGame->removeGameObject(*gameObject);
}

int HHUEngine::strLen(char* string){
  return strlen(string);
}

bool HHUEngine::strEqual(char* s1, char* s2){
  return strcmp(s1, s2) == 0;
}
