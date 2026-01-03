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

uint8_t getJustReleasedButtons(uint8_t button) {

    return a.justReleasedButtons(button);

}

// void loadMap(uint8_t level) {

//     uint8_t blockIdx = 0;
//     uint8_t portalKeyIdx = 0;
//     uint8_t greenDoorIdx = 0;

//     game.resetLevel();

//     // Load Map Data ..
//     {
//         uint24_t levelStart = FX::readIndexedUInt24(Levels::Levels, level);

//         for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {
                
//             FX::seekDataArray(levelStart, y, 0, Constants::Map_X_Count);            
//             FX::readObject(game.getMapData[y]);
//             FX::readEnd();

//         }


//         levelStart = FX::readIndexedUInt24(Levels::Level_Details, level);

//         FX::seekData(levelStart);
//         game.setWorld_Y_Offset(FX::readPendingUInt8());
//         game.getPlayer().setX(FX::readPendingUInt8());
//         game.getPlayer().setY(FX::readPendingUInt8());

//         FX::readEnd();


//         for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {

//             for (uint8_t x = 0; x < Constants::Map_X_Count; x++) {
                    
//                 if (game.getMapData(x, y) == Constants::Tile_Green_Closed) {

//                     game.getGreenDoor(greenDoorIdx).setX(x);
//                     game.getGreenDoor(greenDoorIdx).setY(y);
//                     game.setMapData(x, y, 0);
//                     greenDoorIdx++;

//                 }
//                 else if (game.getMapData(x, y) == Constants::Tile_Block) {
                
//                     game.getBlock(blockIdx).setX(x);
//                     game.getBlock(blockIdx).setY(y);
//                     game.setMapData(x, y, 0);
//                     blockIdx++;

//                 }
//                 else if (game.getMapData(x, y) == Constants::Tile_Portal) {
                
//                     game.getPortal().setX(x);
//                     game.getPortal().setY(y);
//                     game.getPortal().setOpen(true);
//                     game.setMapData(x, y, 0);

//                 }
//                 else if (game.getMapData(x, y) == Constants::Tile_Portal_Inactive) {
                
//                     game.getPortal().setX(x);
//                     game.getPortal().setY(y);
//                     game.getPortal().setOpen(false);
//                     game.setMapData(x, y, 0);

//                 }
//                 else if (game.getMapData(x, y) == Constants::Tile_Portal_Key) {
            
//                     game.getPortalKey(portalKeyIdx).setX(x);
//                     game.getPortalKey(portalKeyIdx).setY(y);
//                     game.setMapData(x, y, 0);
//                     portalKeyIdx++;

//                 }
//                 // else {
                
//                 //     game.setMapData(x, y, 0);
                
//                 // }

//             }
                
//         }

//     }

//     game.setPortalKeyCount(portalKeyIdx);

// }

bool isPortal(uint8_t x, uint8_t y) {

    return game.getPortal().getX() == x && game.getPortal().getY() == y;

}

bool isWalkable(ObjectType objectType, int8_t xOffset, int8_t yOffset) {

    return isWalkable(objectType, 0, 0, xOffset, yOffset);

}

bool isWalkable(ObjectType objectType, uint8_t x, uint8_t y, int8_t xOffset, int8_t yOffset) {

    switch (objectType) {

        case ObjectType::Player:

            switch (game.getMapData(game.getPlayer().getX() + xOffset, game.getPlayer().getY() + yOffset)) {
            
                case Constants::Tile_Border:
                    return false;

            }

            switch (game.getMapData(game.getPlayer().getX() + xOffset, game.getPlayer().getY() + yOffset)) {
            
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

                        if (game.getMapData(block.getX(), block.getY()) == Constants::Tile_Lava) {
                            game.setMapData(block.getX(), block.getY(), 0);
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

            switch (game.getMapData(x + xOffset, y + yOffset)) {
            
                case Constants::Tile_Border:
                case Constants::Tile_Basalt:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Waters_Edge:
                case Constants::Temp_Lava_And_Partial_Wall:
                case Constants::Temp_Lava:
                    return false;

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

            switch (game.getMapData(x + xOffset, y + yOffset)) {
            
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

            switch (game.getMapData(x + xOffset, y + yOffset)) {
            
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
                    // Serial.println("Push second");
                
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

    return game.getMapData(x, y) != 0;
    
}

void incLavaAndWater() {

    // Water first ..

    for (uint16_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint16_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            if (game.getMapData(x, y) == Constants::Tile_Water || game.getMapData(x, y) == Constants::Tile_Water_And_Partial_Wall) {

                if (game.getMapData(x - 1, y) == Constants::Tile_Lava)      { game.setMapData(x - 1, y, Constants::Tile_Basalt); }
                if (game.getMapData(x + 1, y) == Constants::Tile_Lava)      { game.setMapData(x + 1, y, Constants::Tile_Basalt); }
                if (game.getMapData(x, y - 1) == Constants::Tile_Lava)      { game.setMapData(x, y - 1, Constants::Tile_Basalt); }
                if (game.getMapData(x, y + 1) == Constants::Tile_Lava)      { game.setMapData(x, y + 1, Constants::Tile_Basalt); }

                if (game.getMapData(x - 1, y) != Constants::Tile_Water && game.getMapData(x - 1, y) != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, -1, 0) && !isPortal(x - 1, y)) { 
                    if (game.getMapData(x - 1, y) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x - 1, y, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x - 1, y, Constants::Temp_Water);
                    }
                }

                if (game.getMapData(x + 1, y) != Constants::Tile_Water && game.getMapData(x + 1, y) != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 1, 0) && !isPortal(x + 1, y)) { 
                    if (game.getMapData(x + 1, y) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x + 1, y, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x + 1, y, Constants::Temp_Water);
                    }
                }

                if (game.getMapData(x, y - 1) != Constants::Tile_Water && game.getMapData(x, y - 1) != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 0, -1) && !isPortal(x, y - 1)) { 
                    if (game.getMapData(x, y - 1) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y - 1, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y - 1, Constants::Temp_Water);
                    }
                }

                if (game.getMapData(x, y + 1) != Constants::Tile_Water && game.getMapData(x, y + 1) != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 0, 1) && !isPortal(x, y + 1)) { 
                    if (game.getMapData(x, y + 1) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y + 1, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y + 1, Constants::Temp_Water);
                    }
                }

            }

        }
    
    }

    for (uint16_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (game.getMapData(x, y) == Constants::Temp_Water)                     { game.setMapData(x, y, Constants::Tile_Water); }
            if (game.getMapData(x, y) == Constants::Temp_Water_And_Partial_Wall)    { game.setMapData(x, y, Constants::Tile_Water_And_Partial_Wall); }

        }
    
    }

    for (uint16_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint16_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            if (game.getMapData(x, y) == Constants::Tile_Lava || game.getMapData(x, y) == Constants::Tile_Lava_And_Partial_Wall) {

                if (game.getMapData(x - 1, y) == Constants::Tile_Water)      { game.setMapData(x - 1, y, Constants::Tile_Basalt); }
                if (game.getMapData(x + 1, y) == Constants::Tile_Water)      { game.setMapData(x + 1, y, Constants::Tile_Basalt); }
                if (game.getMapData(x, y - 1) == Constants::Tile_Water)      { game.setMapData(x, y - 1, Constants::Tile_Basalt); }
                if (game.getMapData(x, y + 1) == Constants::Tile_Water)      { game.setMapData(x, y + 1, Constants::Tile_Basalt); }

                if (game.getMapData(x - 1, y) != Constants::Tile_Lava && game.getMapData(x - 1, y) != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, -1, 0) && !isPortal(x - 1, y)) { 
                    if (game.getMapData(x - 1, y) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x - 1, y, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x - 1, y, Constants::Temp_Lava);
                    }
                }
                
                if (game.getMapData(x + 1, y) != Constants::Tile_Lava && game.getMapData(x + 1, y) != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 1, 0) && !isPortal(x + 1, y)) { 
                    if (game.getMapData(x + 1, y) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x + 1, y, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x + 1, y, Constants::Temp_Lava);
                    }
                }

                if (game.getMapData(x, y - 1) != Constants::Tile_Lava && game.getMapData(x, y - 1) != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 0, -1) && !isPortal(x, y - 1)) { 
                    if (game.getMapData(x, y - 1) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y - 1, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y - 1, Constants::Temp_Lava);
                    }
                }

                if (game.getMapData(x, y + 1) != Constants::Tile_Lava && game.getMapData(x, y + 1) != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 0, 1) && !isPortal(x, y + 1)) { 
                    if (game.getMapData(x, y + 1) == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y + 1, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y + 1, Constants::Temp_Lava);
                    }
                }

            }

        }
    
    }

    for (uint16_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (game.getMapData(x, y) == Constants::Temp_Lava)                      { game.setMapData(x, y, Constants::Tile_Lava); }
            if (game.getMapData(x, y) == Constants::Temp_Lava_And_Partial_Wall)     { game.setMapData(x, y, Constants::Tile_Lava_And_Partial_Wall); }
       
            if (game.getMapData(x, y) >= Constants::Tile_Counter_00 && game.getMapData(x, y) <= Constants::Tile_Counter_65) {
            
                game.setMapData(x, y, game.getMapData(x, y) - 1);

                if (game.getMapData(x, y) == Constants::Tile_Counter_00) {

                    game.setMapData(x, y, 0);

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

        if (game.getMapData(game.getPlayer().getX(), game.getPlayer().getY()) == Constants::Tile_Green_Switch) {
            greenDoor.setOpen(true);
        }
        else {

            for (uint8_t j = 0; j < Constants::Block_Count; j++) {
                
                Block &block = game.getBlock(j);
                if (!block.isActive()) break;

                if (game.getMapData(block.getX(), block.getY()) == Constants::Tile_Green_Switch) {
    
                    greenDoor.setOpen(true);

                }
            
            }

        }

    }
    
}

void saveCookie() {

    FX::saveGameState(cookie);

}

void cookieReset() {

    for (uint8_t i = 0; i < 40; i++) {

        // if (i == 0) {
        if (i < 22) { //SJH
            game.getPuzzle(i).setStatus(PuzzleStatus::InProgress);
            game.getPuzzle(i).setNumberOfMoves(0);
        }
        else {
            game.getPuzzle(i).setStatus(PuzzleStatus::Locked);
            game.getPuzzle(i).setNumberOfMoves(0);
        }

    }
    
    levelSelect.x = 0;
    levelSelect.y = 0;

}