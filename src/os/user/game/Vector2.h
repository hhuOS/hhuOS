#ifndef __Vector2_include__
#define __Vector2_include__

class Vector2 {

  private:

    float x;
    float y;

  public:

    Vector2(float x, float y){
      this->x = x;
      this->y = y;
    }

    float getX(){
      return x;
    }

    float getY(){
      return y;
    }

    void set(float x, float y){
      this->x = x;
      this->y = y;
    }

};

#endif
