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
    uint8_t greenDoorIdx = 0;

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
                    
                if (mapData[y][x] == Constants::Tile_Border) {
                
                    // mapData[y][x] = Constants::Tile_Border;

                }
                else if (mapData[y][x] == Constants::Tile_Green_Switch) {
                    // mapData[y][x] = Constants::Tile_Lava;

                }
                else if (mapData[y][x] == Constants::Tile_Green_Closed) {

                    game.getGreenDoor(greenDoorIdx).setX(x);
                    game.getGreenDoor(greenDoorIdx).setY(y);
                    mapData[y][x] = 0;
                    greenDoorIdx++;

                }
                else if (mapData[y][x] == Constants::Tile_Waters_Edge) {
                    // mapData[y][x] = Constants::Tile_Lava;

                }
                else if (mapData[y][x] == Constants::Tile_Lava) {
                
                    // mapData[y][x] = Constants::Tile_Lava;

                }
                else if (mapData[y][x] == Constants::Tile_Water) {
                
                    // mapData[y][x] = Constants::Tile_Water;

                }
                else if (mapData[y][x] >= Constants::Tile_Counter_00 && mapData[y][x] <= Constants::Tile_Counter_65) {
                
                    // mapData[y][x] = mapData[y][x];
                    // mapData[y][x] = 1;

                }
                else if (mapData[y][x] == Constants::Tile_Block) {
                
                    game.getBlock(blockIdx).setX(x);
                    game.getBlock(blockIdx).setY(y);
                    mapData[y][x] = 0;
                    blockIdx++;

                }
                else if (mapData[y][x] == Constants::Tile_Portal) {
                
                    game.getPortal().setX(x);
                    game.getPortal().setY(y);
                    game.getPortal().setOpen(true);
                    mapData[y][x] = 0;

                }
                else if (mapData[y][x] == Constants::Tile_Portal_Inactive) {
                
                    game.getPortal().setX(x);
                    game.getPortal().setY(y);
                    game.getPortal().setOpen(false);
                    mapData[y][x] = 0;

                }
                else if (mapData[y][x] == Constants::Tile_Portal_Key) {
            
                    game.getPortalKey(portalKeyIdx).setX(x);
                    game.getPortalKey(portalKeyIdx).setY(y);
                    mapData[y][x] = 0;
                    portalKeyIdx++;

                }
                else {
                
                    mapData[y][x] = 0;
                
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
            
                case Constants::Tile_Border:
                    return false;

            }

            switch (mapData[game.getPlayer().getY() + yOffset][game.getPlayer().getX() + xOffset]) {
            
                case Constants::Tile_Basalt:
                    return false;
            
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                    return false;

            }


            // Push block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (!block.isActive()) break;

                if (block.getX() == game.getPlayer().getX() + xOffset && block.getY() == game.getPlayer().getY() + yOffset) {
                    // Serial.println("Push");
                
                    if (isWalkable(ObjectType::Block, block.getX(), block.getY(), xOffset, yOffset)) {

                        block.setX(block.getX() + xOffset);
                        block.setY(block.getY() + yOffset);

                        if (mapData[block.getY()][block.getX()] == Constants::Tile_Lava) {
                            mapData[block.getY()][block.getX()] = 0;
                        }

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
            
                case Constants::Tile_Border:
                case Constants::Tile_Basalt:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Waters_Edge:
                case Constants::Temp_Lava_And_Partial_Wall:
                case Constants::Temp_Lava:
                    return false;

            
                // case Constants::Tile_Partial_Wall:
                // case Constants::Tile_Lava_And_Partial_Wall:
                //     return false;

            }

            // Is lava blocked by a block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);
                
                if (!block.isActive()) break;

                if (block.getX() == x + xOffset && block.getY() == y + yOffset) {

                    return false;

                }

            }

            // Is lava blocked by a block?

            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                GreenDoor &greenDoor = game.getGreenDoor(i);
                
                if (!greenDoor.isActive()) break;

                if (!greenDoor.isOpen() && greenDoor.getX() == x + xOffset && greenDoor.getY() == y + yOffset) {

                    return false;

                }

            }

            return true;

        case ObjectType::Water:

            switch (mapData[y + yOffset][x + xOffset]) {
            
                case Constants::Tile_Border:
                case Constants::Tile_Basalt:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Waters_Edge:
                case Constants::Temp_Water_And_Partial_Wall:
                case Constants::Temp_Water:
                    return false;

            }

            // Is the water blocked by a block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);
                
                if (!block.isActive()) break;

                if (block.getX() == x + xOffset && block.getY() == y + yOffset) {

                    return false;

                }

            }


            // Is lava blocked by a green door?

            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                GreenDoor &greenDoor = game.getGreenDoor(i);
                
                if (!greenDoor.isActive()) break;

                if (!greenDoor.isOpen() && greenDoor.getX() == x + xOffset && greenDoor.getY() == y + yOffset) {

                    return false;

                }

            }
            
            return true;

        case ObjectType::Block:

            switch (mapData[y + yOffset][x + xOffset]) {
            
                case Constants::Tile_Border:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Partial_Wall:
                case Constants::Tile_Lava_And_Partial_Wall:
                    return false;

            }


            // Push into a second block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (!block.isActive()) break;
                
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

                // Is the block blocked by a green door?

                for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                    GreenDoor &greenDoor = game.getGreenDoor(i);
                    
                    if (!greenDoor.isActive()) break;

                    if (!greenDoor.isOpen() && greenDoor.getX() == x + xOffset && greenDoor.getY() == y + yOffset) {

                        return false;

                    }

                }
                
            }

            return true;

    }

    return true;

}

bool isLava(uint8_t x, uint8_t y) {

    return mapData[y][x] != 0;
    
}
bool debugMe = false;
void incLavaAndWater() {

    // Water first ..

    for (uint16_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint16_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            if (mapData[y][x] == Constants::Tile_Water || mapData[y][x] == Constants::Tile_Water_And_Partial_Wall) {

                if (mapData[y][x - 1] == Constants::Tile_Lava)      { mapData[y][x - 1] = Constants::Tile_Basalt; }
                if (mapData[y][x + 1] == Constants::Tile_Lava)      { mapData[y][x + 1] = Constants::Tile_Basalt; }
                if (mapData[y - 1][x] == Constants::Tile_Lava)      { mapData[y - 1][x] = Constants::Tile_Basalt; }
                if (mapData[y + 1][x] == Constants::Tile_Lava)      { mapData[y + 1][x] = Constants::Tile_Basalt; }

                if (mapData[y][x - 1] != Constants::Tile_Water && mapData[y][x - 1] != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, -1, 0) && !isPortal(x - 1, y)) { 
                    if (mapData[y][x - 1] == Constants::Tile_Partial_Wall) {
                        mapData[y][x - 1] = Constants::Temp_Water_And_Partial_Wall;
                    }
                    else {
                        mapData[y][x - 1] = Constants::Temp_Water;
                    }
                }

                if (mapData[y][x + 1] != Constants::Tile_Water && mapData[y][x + 1] != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 1, 0) && !isPortal(x + 1, y)) { 
                    if (mapData[y][x + 1] == Constants::Tile_Partial_Wall) {
                        mapData[y][x + 1] = Constants::Temp_Water_And_Partial_Wall;
                    }
                    else {
                        mapData[y][x + 1] = Constants::Temp_Water;
                    }
                }

                if (mapData[y - 1][x] != Constants::Tile_Water && mapData[y - 1][x] != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 0, -1) && !isPortal(x, y - 1)) { 
                    if (mapData[y - 1][x] == Constants::Tile_Partial_Wall) {
                        mapData[y - 1][x] = Constants::Temp_Water_And_Partial_Wall;
                    }
                    else {
                        mapData[y - 1][x] = Constants::Temp_Water;
                    }
                }

                if (mapData[y + 1][x] != Constants::Tile_Water && mapData[y + 1][x] != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 0, 1) && !isPortal(x, y + 1)) { 
                    if (mapData[y + 1][x] == Constants::Tile_Partial_Wall) {
                        mapData[y + 1][x] = Constants::Temp_Water_And_Partial_Wall;
                    }
                    else {
                        mapData[y + 1][x] = Constants::Temp_Water;
                    }
                }

            }

        }
    
    }

    for (uint16_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (mapData[y][x] == Constants::Temp_Water)                     { mapData[y][x] = Constants::Tile_Water;}
            if (mapData[y][x] == Constants::Temp_Water_And_Partial_Wall)    { mapData[y][x] = Constants::Tile_Water_And_Partial_Wall;}

        }
    
    }

    for (uint16_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint16_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            if (mapData[y][x] == Constants::Tile_Lava || mapData[y][x] == Constants::Tile_Lava_And_Partial_Wall) {

                if (mapData[y][x - 1] == Constants::Tile_Water)      { mapData[y][x - 1] = Constants::Tile_Basalt; }
                if (mapData[y][x + 1] == Constants::Tile_Water)      { mapData[y][x + 1] = Constants::Tile_Basalt; }
                if (mapData[y - 1][x] == Constants::Tile_Water)      { mapData[y - 1][x] = Constants::Tile_Basalt; }
                if (mapData[y + 1][x] == Constants::Tile_Water)      { mapData[y + 1][x] = Constants::Tile_Basalt; }


                if (mapData[y][x - 1] != Constants::Tile_Lava && mapData[y][x - 1] != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, -1, 0) && !isPortal(x - 1, y)) { 
                    if (mapData[y][x - 1] == Constants::Tile_Partial_Wall) {
                        mapData[y][x - 1] = Constants::Temp_Lava_And_Partial_Wall;
                    }
                    else {
                        mapData[y][x - 1] = Constants::Temp_Lava;
                    }
                }
                
                if (mapData[y][x + 1] != Constants::Tile_Lava && mapData[y][x + 1] != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 1, 0) && !isPortal(x + 1, y)) { 
                    if (mapData[y][x + 1] == Constants::Tile_Partial_Wall) {
                        mapData[y][x + 1] = Constants::Temp_Lava_And_Partial_Wall;
                    }
                    else {
                        mapData[y][x + 1] = Constants::Temp_Lava;
                    }
                }

                if (mapData[y - 1][x] != Constants::Tile_Lava && mapData[y - 1][x] != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 0, -1) && !isPortal(x, y - 1)) { 
                    if (mapData[y - 1][x] == Constants::Tile_Partial_Wall) {
                        mapData[y - 1][x] = Constants::Temp_Lava_And_Partial_Wall;
                    }
                    else {
                        mapData[y - 1][x] = Constants::Temp_Lava;
                    }
                }

                if (mapData[y + 1][x] != Constants::Tile_Lava && mapData[y + 1][x] != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 0, 1) && !isPortal(x, y + 1)) { 
                    if (mapData[y + 1][x] == Constants::Tile_Partial_Wall) {
                        mapData[y + 1][x] = Constants::Temp_Lava_And_Partial_Wall;
                    }
                    else {
                        mapData[y + 1][x] = Constants::Temp_Lava;
                    }
                }

            }

        }
    
    }

    for (uint16_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (mapData[y][x] == Constants::Temp_Lava)                      { mapData[y][x] = Constants::Tile_Lava;}
            if (mapData[y][x] == Constants::Temp_Lava_And_Partial_Wall)     { mapData[y][x] = Constants::Tile_Lava_And_Partial_Wall;}
       
            if (mapData[y][x] >= Constants::Tile_Counter_00 && mapData[y][x] <= Constants::Tile_Counter_65) {
            
                mapData[y][x]--;

                if (mapData[y][x] == Constants::Tile_Counter_00) {

                    mapData[y][x] = 0;

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

void updateGreenDoors() {

    for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {
        
        GreenDoor &greenDoor = game.getGreenDoor(i);
        greenDoor.setOpen(false);

        if (mapData[game.getPlayer().getY()][game.getPlayer().getX()] == Constants::Tile_Green_Switch) {
            greenDoor.setOpen(true);
        }
        else {

            for (uint8_t j = 0; j < Constants::Block_Count; j++) {
                
                Block &block = game.getBlock(j);
                if (!block.isActive()) break;

                if (mapData[block.getY()][block.getX()] == Constants::Tile_Green_Switch) {
    
                    greenDoor.setOpen(true);

                }
            
            }

        }

    }
    
}