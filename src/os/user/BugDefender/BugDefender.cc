#include <kernel/services/TimeService.h>
#include <kernel/Kernel.h>
#include <lib/libc/snprintf.h>
#include <kernel/services/DebugService.h>
#include "BugDefender.h"
#include "user/BugDefender/sprites/heart.cc"
#include "lib/Colors.h"

#include "lib/libc/sprintf.h"

int BugDefender::score = 0;
int BugDefender::lifes = 3;
bool BugDefender::isGameOver = false;
bool BugDefender::isGameWon = false;
int BugDefender::enemyCount = enemiesPerLine * enemyLines;

BugDefender::BugDefender() : Game() {

    score = 0;
    lifes = 3;
    isMenuVisible = true;
    isGameOver = false;
    isGameWon = false;
    enemyCount = enemiesPerLine * enemyLines;
    Fleet::reset();

    HHUEngine::setCurrentGame(this);
    HHUEngine::instantiate( new Ship(Vector2(300, 425)) );

    for(int x = 0; x < enemiesPerLine; x++){
        for(int y = 0; y < enemyLines; y++){
            HHUEngine::instantiate( new Enemy(Vector2(100 + x*40, 20 + y*30), y) );
        }
    }


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

    g2d->drawString(sun_font_8x16, 25, 450, "Points: ", Color(10,255,10), Colors::BLACK);

    char scoreString[32];
    sprintf(scoreString, "%d", score);

    g2d->drawString(sun_font_8x16, 90, 450, scoreString, Color(10,255,10), Colors::BLACK);

    for(int i = 0; i < lifes; i++){
        g2d->drawSprite(570 + 15*i, 450, heart.width, heart.height, (int*) &heart.pixel_data[0]);
    }
}

void BugDefender::showMenu(LinearFrameBuffer* g2d){

    Rtc *rtc = Kernel::getService<TimeService>()->getRTC();

    Rtc::date date = rtc->getCurrentDate();

    char timeString[20];

    snprintf(timeString, 20, "%02d.%02d.%04d %02d:%02d:%02d", date.dayOfMonth, date.month, date.year, date.hours, date.minutes, date.seconds);

    g2d->placeString(sun_font_12x22, 50, 5, timeString, Colors::HHU_LIGHT_GRAY);

    g2d->placeString(sun_font_12x22, 50, 15, "Bug Defender", Colors::WHITE);

    g2d->placeString(sun_font_8x16, 50, 35, "1st / 2nd Row   -   40 Points", Colors::WHITE);
    g2d->placeString(sun_font_8x16, 50, 45, "3rd / 4th Row   -   60 Points", Colors::WHITE);
    g2d->placeString(sun_font_8x16, 50, 55, "5th / 6th Row   -   80 Punkte", Colors::WHITE);

    g2d->placeString(pearl_font_8x8, 50, 85, "Please press Enter", Colors::WHITE);

    (Kernel::getService<DebugService>())->printPic();
}

void BugDefender::showGameOver(LinearFrameBuffer* g2d){

    if((HHUEngine::time() / 100) % 2 == 0) {
        g2d->drawString(sun_font_12x22, 266, 229, "Game Over!", Colors::WHITE, Colors::BLACK);
    }
}

void BugDefender::showGameWon(LinearFrameBuffer* g2d){

    if((HHUEngine::time() / 100) % 2 == 0) {
        g2d->drawString(sun_font_12x22, 266, 229, "You win!", Colors::WHITE, Colors::BLACK);
    }
}

void BugDefender::draw(LinearFrameBuffer* g2d){

    if(isGameOver){
        showGameOver(g2d);
        g2d->show();
        return;
    }

    if(isGameWon){
        showGameWon(g2d);
        g2d->show();
        return;
    }

    if(isMenuVisible){
        showMenu(g2d);
        g2d->show();
        return;
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
}

void BugDefender::gameOver(){
    isGameOver = true;
}

void BugDefender::enemyShot(){
    enemyCount--;
}
