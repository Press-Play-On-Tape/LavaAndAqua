#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println
#define DEBUG_BREAK    asm volatile("break\n");

#define _DEBUG
#define _DEBUG_BID
#define _DEBUG_SKIP_DEAL
#define _DEBUG_BASIC
#define _DEBUG_RAND

#define DEBUG_PLAY_ADDCARD

namespace Constants {

    const int8_t Bounce[] PROGMEM = { -36, -30, -24, -16, -12, -6, 0, 4, 8, 12, 16, 20, 24, 28, 32, 28, 25, 22, 21, 21, 22, 25, 28, 32, 29, 27, 26, 26, 27, 28, 32, 30, 29, 30, 32, 31, 32 };
    
    constexpr uint8_t Map_X_Count = 16;
    constexpr uint8_t Map_Y_Count = 11;


    constexpr uint8_t Tile_Lava = 250;
    constexpr uint8_t Tile_Water = 239;
    constexpr uint8_t Tile_Basalt = 240;
    constexpr uint8_t Tile_Portal = 241;
    constexpr uint8_t Tile_Portal_Inactive = 242;
    constexpr uint8_t Tile_Portal_Key = 243;
    constexpr uint8_t Tile_Block = 251;
    constexpr uint8_t Tile_Counter_00 = 100;
    constexpr uint8_t Tile_Counter_50 = 150;

    constexpr uint8_t Block_Count = 6;
    constexpr uint8_t Portal_Key_Count = 4;

};
