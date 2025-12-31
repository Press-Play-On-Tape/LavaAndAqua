#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"



uint8_t getJustPressedButtons() {

    a.pollButtons();

    return a.justPressedButtons();

}


uint8_t getPressedButtons() {

    return a.pressedButtons();

}


void loadMap(uint8_t level) {

    uint8_t blockIdx = 0;
    uint8_t portalKeyIdx = 0;
    
    game.resetLevel();

    // Load Map Data ..
    {
        uint24_t levelStart = FX::readIndexedUInt24(Levels::Levels, level);

        for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {
                
            FX::seekDataArray(levelStart, y, 0, Constants::Map_X_Count);            
            // FX::readObject(mapData[Constants::Map_Y_Count - y - 1]);
            FX::readObject(mapData[y]);
            FX::readEnd();

        }


        levelStart = FX::readIndexedUInt24(Levels::Level_Details, level);

        FX::seekData(levelStart);
        game.setWorld_Y_Offset(FX::readPendingUInt8());
        game.getPlayer().setX(FX::readPendingUInt8());
        game.getPlayer().setY(FX::readPendingUInt8());

        FX::readEnd();


        for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {

            for (uint8_t x = 0; x < Constants::Map_X_Count; x++) {
                    
                if (mapData[y][x] == Constants::Tile_Lava) {
                
                    lavaData[y][x] = Constants::Tile_Lava;

                }
                else if (mapData[y][x] == Constants::Tile_Water) {
                
                    lavaData[y][x] = Constants::Tile_Water;

                }
                else if (mapData[y][x] >= Constants::Tile_Counter_00 && mapData[y][x] <= Constants::Tile_Counter_50) {
                
                    lavaData[y][x] = mapData[y][x];
                    mapData[y][x] = 1;

                }
                else if (mapData[y][x] == Constants::Tile_Block) {
                
                    game.getBlock(blockIdx).setX(x);
                    game.getBlock(blockIdx).setY(y);
                    mapData[y][x] = 1;
                    blockIdx++;

                }
                else if (mapData[y][x] == Constants::Tile_Portal) {
                
                    game.getPortal().setX(x);
                    game.getPortal().setY(y);
                    game.getPortal().setOpen(true);
                    mapData[y][x] = 1;

                }
                else if (mapData[y][x] == Constants::Tile_Portal_Inactive) {
                
                    game.getPortal().setX(x);
                    game.getPortal().setY(y);
                    game.getPortal().setOpen(false);
                    mapData[y][x] = 1;

                }
                else if (mapData[y][x] == Constants::Tile_Portal_Key) {
            
                    game.getPortalKey(portalKeyIdx).setX(x);
                    game.getPortalKey(portalKeyIdx).setY(y);
                    mapData[y][x] = 1;
                    portalKeyIdx++;

                }
                else {
                
                    lavaData[y][x] = 0;
                
                }

            }
                
        }

    }

    game.setPortalKeyCount(portalKeyIdx);

}

bool isPortal(uint8_t x, uint8_t y) {

    return game.getPortal().getX() == x && game.getPortal().getY() == y;

}

bool isWalkable(ObjectType objectType, int8_t xOffset, int8_t yOffset) {

    return isWalkable(objectType, 0, 0, xOffset, yOffset);

}

bool isWalkable(ObjectType objectType, uint8_t x, uint8_t y, int8_t xOffset, int8_t yOffset) {

    switch (objectType) {

        case ObjectType::Player:

            switch (mapData[game.getPlayer().getY() + yOffset][game.getPlayer().getX() + xOffset]) {
            
                case 0:
                    return false;

            }

            switch (lavaData[game.getPlayer().getY() + yOffset][game.getPlayer().getX() + xOffset]) {
            
                case Constants::Tile_Basalt:
                    return false;
            
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_50:
                    return false;

            }

            // Push block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (block.getX() == game.getPlayer().getX() + xOffset && block.getY() == game.getPlayer().getY() + yOffset) {
                    // Serial.println("Push");
                
                    if (isWalkable(ObjectType::Block, block.getX(), block.getY(), xOffset, yOffset)) {

                        block.setX(block.getX() + xOffset);
                        block.setY(block.getY() + yOffset);
                        lavaData[block.getY()][block.getX()] = 0;

                        return true;
                    
                    }
                    else {
                        return false;
                    }

                }

            }
            

            return true;

        case ObjectType::Lava:

            switch (mapData[y + yOffset][x + xOffset]) {
            
                case 0:
                    return false;

            }

            // Is lava blocked by a block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (block.getX() == x + xOffset && block.getY() == y + yOffset) {

                    return false;

                }

            }

            return true;

        case ObjectType::Block:

            switch (mapData[y + yOffset][x + xOffset]) {
            
                case 0:
                    return false;

            }

            switch (lavaData[y + yOffset][x + xOffset]) {

                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_50:
                    return false;

            }


            // Push into a second block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (block.getX() == game.getPlayer().getX() + (2 * xOffset) && block.getY() == game.getPlayer().getY() + (2 * yOffset)) {
                    Serial.println("Push second");
                
                    // if (isWalkable(ObjectType::Block, block.getX(), block.getY(), xOffset, yOffset)) {
                    //     block.setX(block.getX() + xOffset);
                    //     block.setY(block.getY() + yOffset);
                    //     return true;
                    
                    // }
                    // else {
                        return false;
                    // }

                }

            }

            return true;

    }

    return true;

}

bool isLava(uint8_t x, uint8_t y) {

    return lavaData[y][x] != 0;
    
}

void incLava() {

    // Water first ..

    for (uint16_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint16_t x = 1; x < Constants::Map_X_Count - 1; x++) {

// if (lavaData[y][x] == Constants::Tile_Water && x ==7 & y == 2)      {

// if (lavaData[y][x] == Constants::Tile_Water && lavaData[y - 1][x] == Constants::Tile_Lava)      {
// Serial.println("a");
// }
// if (lavaData[y][x] == Constants::Tile_Water && lavaData[y - 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0, -1) && !isPortal(x, y - 1))      {
// Serial.println("b");
// }
// DEBUG_BREAK
// }
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y][x - 1] == Constants::Tile_Lava)      { lavaData[y][x - 1] = Constants::Tile_Basalt; }
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y][x + 1] == Constants::Tile_Lava)      { lavaData[y][x + 1] = Constants::Tile_Basalt; }
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y - 1][x] == Constants::Tile_Lava)      { lavaData[y - 1][x] = Constants::Tile_Basalt; }
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y + 1][x] == Constants::Tile_Lava)      { lavaData[y + 1][x] = Constants::Tile_Basalt; }

            // if (lavaData[y][x] == Constants::Tile_Water && lavaData[y][x - 1] == 0 && isWalkable(ObjectType::Lava, x, y, -1, 0) && !isPortal(x - 1, y))      { lavaData[y][x - 1] = 101;}
            // if (lavaData[y][x] == Constants::Tile_Water && lavaData[y][x + 1] == 0 && isWalkable(ObjectType::Lava, x, y,  1, 0) && !isPortal(x + 1, y))      { lavaData[y][x + 1] = 101;}

            // if (lavaData[y][x] == Constants::Tile_Water && lavaData[y - 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0, -1) && !isPortal(x, y - 1))      { lavaData[y - 1][x] = 101;}
            // if (lavaData[y][x] == Constants::Tile_Water && lavaData[y + 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0,  1) && !isPortal(x, y + 1))      { lavaData[y + 1][x] = 101;}
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y][x - 1] == 0 && isWalkable(ObjectType::Lava, x, y, -1, 0))      { lavaData[y][x - 1] = 99;}
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y][x + 1] == 0 && isWalkable(ObjectType::Lava, x, y,  1, 0))      { lavaData[y][x + 1] = 99;}

            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y - 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0, -1))      { lavaData[y - 1][x] = 99;}
            if (lavaData[y][x] == Constants::Tile_Water && lavaData[y + 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0,  1))      { lavaData[y + 1][x] = 99;}

        }
    
    }

    for (uint16_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (lavaData[y][x] == 99) { lavaData[y][x] = Constants::Tile_Water;}
       
        }
    
    }


    for (uint16_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint16_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y][x - 1] == Constants::Tile_Water)      { lavaData[y][x - 1] = Constants::Tile_Basalt; }
            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y][x + 1] == Constants::Tile_Water)      { lavaData[y][x + 1] = Constants::Tile_Basalt; }
            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y - 1][x] == Constants::Tile_Water)      { lavaData[y - 1][x] = Constants::Tile_Basalt; }
            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y + 1][x] == Constants::Tile_Water)      { lavaData[y + 1][x] = Constants::Tile_Basalt; }

            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y][x - 1] == 0 && isWalkable(ObjectType::Lava, x, y, -1, 0) && !isPortal(x - 1, y))      { lavaData[y][x - 1] = 98;}
            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y][x + 1] == 0 && isWalkable(ObjectType::Lava, x, y,  1, 0) && !isPortal(x + 1, y))      { lavaData[y][x + 1] = 98;}

            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y - 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0, -1) && !isPortal(x, y - 1))      { lavaData[y - 1][x] = 98;}
            if (lavaData[y][x] == Constants::Tile_Lava && lavaData[y + 1][x] == 0 && isWalkable(ObjectType::Lava, x, y, 0,  1) && !isPortal(x, y + 1))      { lavaData[y + 1][x] = 98;}

        }
    
    }

    for (uint16_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (lavaData[y][x] == 98) { lavaData[y][x] = Constants::Tile_Lava;}
       
            if (lavaData[y][x] >= Constants::Tile_Counter_00 && lavaData[y][x] <= Constants::Tile_Counter_50) {
            
                lavaData[y][x]--;

                if (lavaData[y][x] == Constants::Tile_Counter_00) {

                    lavaData[y][x] = 0;

                }

            }

        }
    
    }

}

void fix_World_Y_Offset() {

    switch (game.getPlayer().getY()) {
    
        case 0 ... 3:
            game.setWorld_Y_Offset(0);
            break;

        case 4:
            game.setWorld_Y_Offset(1);
            break;

        case 5:
            game.setWorld_Y_Offset(2);
            break;

        case 6 ... 9:
            game.setWorld_Y_Offset(3);
            break;


    }
    
}