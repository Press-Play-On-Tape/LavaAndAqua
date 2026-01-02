#include <ArduboyFX.h>  
#include "fxdata/fxdata.h"


void play_Init() { 

    // game.resetFrameCount();
    game.setMoveCount(0);
    gameState = GameState::Play_Shuffle;

    loadMap(game.getLevel());
    menuSelect = 0;
    menuX = 128;
    menuDirection = Direction::None;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void play_Update() { 

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();

    game.incFrameCount();

    if (menuX == 128) {
        
        if (pressed & B_BUTTON && justPressed & DOWN_BUTTON && game.getWorld_Y_Offset() < 3) {
            game.setWorld_Y_Offset(game.getWorld_Y_Offset() + 1);
        }

        else if (pressed & B_BUTTON && justPressed & UP_BUTTON && game.getWorld_Y_Offset() > 0) {
            game.setWorld_Y_Offset(game.getWorld_Y_Offset() - 1);
        }

        else if (justPressed & LEFT_BUTTON && isWalkable(ObjectType::Player, -1, 0)) {
            game.captureMove();
            game.getPlayer().decX();
            updateGreenDoors();
            incLavaAndWater();
        }

        else if (justPressed & RIGHT_BUTTON && isWalkable(ObjectType::Player, 1, 0)) {
            game.captureMove();
            game.getPlayer().incX();
            updateGreenDoors();
            incLavaAndWater();
        }

        else if (justPressed & UP_BUTTON && isWalkable(ObjectType::Player, 0, -1)) {
            game.captureMove();
            game.getPlayer().decY();
            updateGreenDoors();
            incLavaAndWater();
            fix_World_Y_Offset();

        }

        else if (justPressed & DOWN_BUTTON && isWalkable(ObjectType::Player, 0, 1)) {
            game.captureMove();
            game.getPlayer().incY();
            updateGreenDoors();
            incLavaAndWater();
            fix_World_Y_Offset();

        }


    }
    else {

        if (justPressed & UP_BUTTON) {

            if (menuSelect > game.getUndoCount() == 0 ? 2 : 1) {
                menuSelect--;    
            }

        }

        else if (justPressed & DOWN_BUTTON) {

            if (menuSelect < 2) {
                menuSelect++;    
            }

        }

        else if (justPressed & A_BUTTON) {

            switch (menuSelect) {
            
                case 0:
                    game.revertMove();
                    if (game.getUndoCount() == 0) {
                        menuDirection = Direction::Right;
                    }
                    break;
            
                case 1:
                    gameState = GameState::Play_Init;
                    break;
            
                case 2:
                    gameState = GameState::Title_Init;
                    break;
                    
            }

        }

    }

        
    // Menu ..

    if (justPressed & B_BUTTON) {
                
        if (menuX == 128) {
            menuDirection = Direction::Left;
            menuSelect = game.getUndoCount() == 0 ? 1 : 0; 
        }
        else if (menuX == 128 - 32) {
            menuDirection = Direction::Right;
        }
        
    }

    switch (menuDirection) {
    
        case Direction::Left:

            menuX = menuX - 2;

            if (menuX == 128 - 32) {
                menuDirection = Direction::None;
            }

            break;
    
        case Direction::Right:

            menuX = menuX + 2;

            if (menuX == 128) {
                menuDirection = Direction::None;
            }

            break;
        
    }


    // ---------------------------------------------------------------------
    // Have we won?
    // ---------------------------------------------------------------------

    if (game.getPlayer().getX() == game.getPortal().getX() && game.getPlayer().getY() == game.getPortal().getY()) {
    
        // Serial.println("Level Complete");
       
        game.getPuzzle(game.getLevel()).setStatus(PuzzleStatus::Complete);
        game.getPuzzle(game.getLevel()).setNumberOfMoves(game.getMoveCount());
        
        if (game.getLevel() < Constants::Level_Count - 1 && game.getPuzzle(game.getLevel() + 1).getStatus() == PuzzleStatus::Locked) {

            game.getPuzzle(game.getLevel() + 1).setStatus(PuzzleStatus::InProgress);
            game.setLevel(game.getLevel() + 1);
            gameState = GameState::Play_Init;
            levelSelect.increaseGame();

        }
        else {

            gameState = GameState::Title_Select;

        }

        saveCookie();

    }


    // ---------------------------------------------------------------------
    // Have we lost?
    // ---------------------------------------------------------------------

    if (isLava(game.getPlayer().getX() , game.getPlayer().getY())) {
    
        // Serial.println("Dead!");

    }


    // ---------------------------------------------------------------------
    // Portal Key
    // ---------------------------------------------------------------------

    for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {
        
        if (game.getPlayer().getX() == game.getPortalKey(i).getX() && game.getPlayer().getY() == game.getPortalKey(i).getY()) {
        
            game.decPortalKeyCount();
            game.getPortalKey(i).setY(15);
            game.getPortalKey(i).setY(15);

            if (game.getPortalKeyCount() == 0) {
            
                game.getPortal().setOpen(true);
                
            }

        }
    
    }


}


void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    uint8_t currentPlane = a.currentPlane();
    if (a.needsUpdate()) play_Update();

    uint24_t levelIdx = FX::readIndexedUInt24(Images::Level_Images, game.getLevel());
    SpritesU::drawOverwriteFX(0, - game.getWorld_Y_Offset() * 8, levelIdx, currentPlane);

    SpritesU::drawOverwriteFX(120, 0, Images::Mini_HUD, currentPlane);
    SpritesU::drawOverwriteFX(121, 17, Images::Numbers_HUD, ((game.getLevel() + 1) * 3) + currentPlane);
    SpritesU::drawOverwriteFX(121, 50, Images::Numbers_HUD, ((game.getMoveCount() / 10) * 3) + currentPlane);
    SpritesU::drawOverwriteFX(121, 54, Images::Numbers_HUD, ((game.getMoveCount() % 100) * 3) + currentPlane);

    for (uint8_t y = game.getWorld_Y_Offset(); y < Constants::Map_Y_Count; y++) {

        int8_t yPos = ((y - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels;

        if (yPos > 63) continue;

        for (uint8_t x = 0; x < Constants::Map_X_Count; x++) {

            int8_t xPos = (x * 8) - Constants::XOffset_Pixels;

            if (xPos + 8 > menuX) continue;

            if (game.mapData[y][x] >= Constants::Tile_Counter_00 && game.mapData[y][x] <= Constants::Tile_Counter_65) {

                SpritesU::drawOverwriteFX(xPos, yPos, Images::Numbers_5x3_2D_WB, ((game.mapData[y][x] - Constants::Tile_Counter_00) * 3) + currentPlane);

            }
            else if (game.mapData[y][x] == Constants::Tile_Waters_Edge) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Waters_Edge * 3) + currentPlane);

            }
            else if (game.mapData[y][x] == Constants::Tile_Green_Switch) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Green_Switch * 3) + currentPlane);

            }
            else if (game.mapData[y][x] == Constants::Tile_Partial_Wall) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Partial_Wall * 3) + currentPlane);

            }
            else if (game.mapData[y][x] == Constants::Tile_Lava_And_Partial_Wall) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, ((Constants::Image_Lava_And_Partial_Wall + ((game.getFrameCount() % 32) / 8)) * 3) + currentPlane);

            }
            else if (game.mapData[y][x] == Constants::Tile_Water_And_Partial_Wall) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, ((Constants::Image_Water_And_Partial_Wall + ((game.getFrameCount() % 32) / 8)) * 3) + currentPlane);

            }
            else if (game.mapData[y][x] == Constants::Tile_Lava) {

                uint8_t len = 1;

                for (uint8_t i = 1; i < 8; i++) {

                    if (game.mapData[y][x + i] == Constants::Tile_Lava) {
                        len = i + 1;
                    }
                    else {
                       
                        break;
                    }

                }

                uint24_t imgIdx = FX::readIndexedUInt24(Images::Lavas, len - 1);
                SpritesU::drawOverwriteFX(xPos, yPos, imgIdx, (((game.getFrameCount() % 32) / 8) * 3) + currentPlane);

                x = x + len - 1;

            }

            else if (game.mapData[y][x] == Constants::Tile_Water) {

                uint8_t len = 1;

                for (uint8_t i = 1; i < 8; i++) {

                    if (game.mapData[y][x + i] == Constants::Tile_Water) {
                        len = i + 1;
                    }
                    else {
                       
                        break;
                    }

                }

                uint24_t imgIdx = FX::readIndexedUInt24(Images::Waters, len - 1);
                SpritesU::drawOverwriteFX(xPos, yPos, imgIdx, (((game.getFrameCount() % 32) / 8) * 3) + currentPlane);

                x = x + len - 1;

            }

            if (game.mapData[y][x] == Constants::Tile_Basalt) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Basalt * 3) + currentPlane);

            }
        
        }
    
    }


    // Blocks ..

    for (uint8_t i = 0; i < Constants::Block_Count; i++) {

        Block &block = game.getBlock(i);

        if (block.isActive()) {
            SpritesU::drawPlusMaskFX((block.getX() * 8) - Constants::XOffset_Pixels, ((block.getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Block * 3) + currentPlane);
        }
        else {
            break;
        }

    }


    // Portal Keys ..

    for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {

        PortalKey &key = game.getPortalKey(i);

        if (key.isActive()) {
            SpritesU::drawPlusMaskFX((key.getX() * 8) - Constants::XOffset_Pixels, ((key.getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, ((5 + ((game.getFrameCount() % 36) / 12)) * 3) + currentPlane);
        }

    }



    // Green Doors ..

    for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

        GreenDoor &greenDoor = game.getGreenDoor(i);

        if (game.mapData[greenDoor.getY()][greenDoor.getX()] != Constants::Tile_Basalt) {

            if (greenDoor.isActive()) {

                if (greenDoor.isOpen()) {
                    SpritesU::drawPlusMaskFX((greenDoor.getX() * 8) - Constants::XOffset_Pixels, ((greenDoor.getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Green_Open * 3) + currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX((greenDoor.getX() * 8) - Constants::XOffset_Pixels, ((greenDoor.getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Green_Closed * 3) + currentPlane);
                }

            }

        }

    }


    // Portal ..

    if (game.getPortal().isOpen()) {

        SpritesU::drawPlusMaskFX((game.getPortal().getX() * 8) - Constants::XOffset_Pixels, ((game.getPortal().getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, ((Constants::Image_Portal + ((game.getFrameCount() % 40) / 8)) * 3) + currentPlane);
    }
    else {

        SpritesU::drawPlusMaskFX((game.getPortal().getX() * 8) - Constants::XOffset_Pixels, ((game.getPortal().getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Portal_Inactive * 3) + currentPlane);

    }


    // Player ..

    SpritesU::drawPlusMaskFX((game.getPlayer().getX() * 8) - Constants::XOffset_Pixels, ((game.getPlayer().getY() - game.getWorld_Y_Offset()) * 8) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Player * 3) + currentPlane);


    if (menuX < 128) {
        SpritesU::drawOverwriteFX(menuX, 0, Images::Menu, ((menuSelect + (game.getUndoCount() == 0 ? 2 : 0)) * 3) + currentPlane);
    }


}
