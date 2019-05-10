/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <kernel/services/TimeService.h>
#include <kernel/Kernel.h>
#include <lib/libc/snprintf.h>
#include <kernel/services/DebugService.h>
#include <apps/Application.h>
#include <kernel/threads/Scheduler.h>
#include "BugDefender.h"
#include "lib/graphic/Colors.h"

#include "lib/libc/sprintf.h"

int BugDefender::score = 0;
int BugDefender::lifes = 3;
bool BugDefender::isGameOver = false;
bool BugDefender::isGameWon = false;
int BugDefender::enemyCount = enemiesPerLine * enemyLines;
Image *BugDefender::background = nullptr;
Image *BugDefender::heartSprite = nullptr;
ThreadPool *BugDefender::beepThreadPool = nullptr;

BugDefender::BugDefender() : Game() {

    if(heartSprite == nullptr) {
        heartSprite = Bmp::load("/initrd/game/res/shield.bmp");
    }

    if(background == nullptr) {
        background = Bmp::load("/initrd/game/res/spaceBackground.bmp");
    }

    score = 0;
    lifes = 3;
    isMenuVisible = true;
    isGameOver = false;
    isGameWon = false;
    enemyCount = enemiesPerLine * enemyLines;
    Fleet::reset();

    beepThreadPool = new ThreadPool(4);

    beepThreadPool->startWorking();

    HHUEngine::setCurrentGame(this);
    HHUEngine::instantiate( new Ship(Vector2(300, 400)) );

    for(int x = 0; x < enemiesPerLine; x++){
        for(int y = 0; y < enemyLines; y++){
            HHUEngine::instantiate( new Enemy(Vector2(100 + x*40, 20 + y*30), y) );
        }
    }
}

BugDefender::~BugDefender() {
    delete beepThreadPool;
}

void BugDefender::update(float delta){

    if(HHUEngine::isKeyPressed(1)){
        isRunning = false;
    }

    if(enemyCount < 1 && !isGameWon)
        isGameWon = true;

    if(lifes == 0 && !isGameOver)
        gameOver();

    if(isMenuVisible){
        if(HHUEngine::isKeyPressed(28))
            isMenuVisible = false;
        return;
    }

    if(isGameWon || isGameOver) {
        return;
    }

    GameObject* g;
    for (uint32_t i = 0; i < gameObjects->size(); i++) {
        g = gameObjects->get(i);
        g->update(delta);
    }


    for (uint32_t i = 0; i < gameObjects->size(); i++) {
        g = gameObjects->get(i);
        checkCollisions(g);
    }

    applyChanges();

    Fleet::swapSpeed();

    // Seed nach jedem update "zufaellig" veraendern
    HHUEngine::setSeed(HHUEngine::rand() * 1234567);
}

void BugDefender::drawInfo(LinearFrameBuffer* g2d){
    g2d->drawLine(20,440,620,440, Color(10,255,10));

    g2d->drawString(sun_font_8x16, 25, 450, "Points: ", Color(10,255,10), Colors::INVISIBLE);

    char scoreString[32];
    sprintf(scoreString, "%d", score);

    g2d->drawString(sun_font_8x16, 90, 450, scoreString, Color(10,255,10), Colors::INVISIBLE);

    if(heartSprite != nullptr) {
        for (int i = 0; i < lifes; i++) {
            heartSprite->draw(static_cast<uint16_t>(540 + 25 * i), 450);
        }
    }
}

void BugDefender::showMenu(LinearFrameBuffer* g2d){

    g2d->placeString(sun_font_12x22, 50, 15, "Bug Defender", Colors::WHITE);

    g2d->placeString(sun_font_8x16, 50, 35, "1st / 2nd Row   -   40 Points", Colors::WHITE);
    g2d->placeString(sun_font_8x16, 50, 45, "3rd / 4th Row   -   60 Points", Colors::WHITE);
    g2d->placeString(sun_font_8x16, 50, 55, "5th / 6th Row   -   80 Punkte", Colors::WHITE);

    g2d->placeString(pearl_font_8x8, 50, 85, "Please press Enter", Colors::WHITE);
}

void BugDefender::showGameOver(LinearFrameBuffer* g2d){

    if((HHUEngine::time() / 1000) % 2 == 0) {
        g2d->placeString(sun_font_12x22, 50, 50, "Game Over!", Colors::WHITE);
    }

    g2d->placeString(std_font_8x8, 50, 85, "Please press Enter", Colors::WHITE);
}

void BugDefender::showGameWon(LinearFrameBuffer* g2d){

    if((HHUEngine::time() / 1000) % 2 == 0) {
        g2d->placeString(sun_font_12x22, 50, 50, "You Win!", Colors::WHITE);
    }

    g2d->placeString(std_font_8x8, 50, 85, "Please press Enter", Colors::WHITE);
}

void BugDefender::draw(LinearFrameBuffer* g2d){

    if(isGameOver){
        showGameOver(g2d);
        g2d->show();

        if(HHUEngine::isKeyPressed(KeyEvent::RETURN)) {
            Game::isRunning = false;
        }

        return;
    }

    if(isGameWon){
        showGameWon(g2d);
        g2d->show();

        if(HHUEngine::isKeyPressed(KeyEvent::RETURN)) {
            Game::isRunning = false;
        }

        return;
    }

    if(isMenuVisible){
        showMenu(g2d);
        g2d->show();
        return;
    }

    if(background != nullptr) {
        background->draw(0, 0);
    }

    drawInfo(g2d);

    GameObject *g;
    for (uint32_t i = 0; i < gameObjects->size(); i++) {
        g = gameObjects->get(i);
        g->draw(g2d);
    }

    g2d->show();
}

void BugDefender::checkCollisions(GameObject* o){

    if(o->collider == 0x0)
        return;

    GameObject *g;
    for (uint32_t i = 0; i < gameObjects->size(); i++) {

        g = gameObjects->get(i);

        if( g == o || g->collider == 0x0 ){
            continue;
        }

        bool isColliding = g->collider->isColliding(*o->collider);

        if( isColliding && !o->colliding){
            o->colliding = true;
            o->onCollisionEnter(*g);
            break;
        } else if ( !isColliding && o->colliding ){
            o->colliding = false;
            o->onCollisionExit();
            break;
        }

    }
}

void BugDefender::addPoints(int points){
    score += points;
}

void BugDefender::takeDamage(int amount){
    lifes -= amount;

    beepThreadPool->addWork([](){HHUEngine::beep(PcSpeaker::C0);});
}

void BugDefender::gameOver(){
    isGameOver = true;
}

void BugDefender::enemyShot(){
    enemyCount--;

    beepThreadPool->addWork([](){HHUEngine::beep(PcSpeaker::C2);});
}
