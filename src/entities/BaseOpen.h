#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"

class BaseOpen {

    private:

        bool open = false;

    public:

        bool isOpen()                                  { return this->open; }
    
        void setOpen(bool val)                         { this->open = val; }
   
};