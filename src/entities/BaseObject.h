#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"

class BaseObject {

    private:

        uint8_t x = 0;
        uint8_t y = 0;

    public:

        uint8_t getX()                                  { return this->x; }
        uint8_t getY()                                  { return this->y; }

        void setX(uint8_t val)                          { this->x = val; }
        void setY(uint8_t val)                          { this->y = val; }

        bool isActive() {

            return this->x > 0 && this->x < 255;
        
        }

};