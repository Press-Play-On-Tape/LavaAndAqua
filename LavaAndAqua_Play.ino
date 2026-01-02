#include <ArduboyFX.h>  
#include "fxdata/fxdata.h"


uint8_t mapData[11][16];

void play_Init() { 

    game.resetFrameCount();
    gameState = GameState::Play_Shuffle;

    // #ifdef DEBUG_RAND
    a.initRandomSeed();
    uint16_t r = random(8000);
    // r = 5362;
    // DEBUG_PRINT("Rand ");
    // DEBUG_PRINTLN(r);
    randomSeed(r);
    game.setRandomSeed(r);
    // #endif

    game.setLevel(23);
    loadMap(game.getLevel());

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void play_Update() { 

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();

    game.incFrameCount();

    if (pressed & B_BUTTON && justPressed & DOWN_BUTTON && game.getWorld_Y_Offset() < 3) {
       game.setWorld_Y_Offset(game.getWorld_Y_Offset() + 1);
    }

    else if (pressed & B_BUTTON && justPressed & UP_BUTTON && game.getWorld_Y_Offset() > 0) {
       game.setWorld_Y_Offset(game.getWorld_Y_Offset() - 1);
    }

    else if (justPressed & LEFT_BUTTON && isWalkable(ObjectType::Player, -1, 0)) {
        game.getPlayer().setX(game.getPlayer().getX() - 1);
        updateGreenDoors();
        incLavaAndWater();
    }

    else if (justPressed & RIGHT_BUTTON && isWalkable(ObjectType::Player, 1, 0)) {
        game.getPlayer().setX(game.getPlayer().getX() + 1);
        updateGreenDoors();
        incLavaAndWater();
    }

    else if (justPressed & UP_BUTTON && isWalkable(ObjectType::Player, 0, -1)) {
        game.getPlayer().setY(game.getPlayer().getY() - 1);
        updateGreenDoors();
        incLavaAndWater();
        fix_World_Y_Offset();

    }

    else if (justPressed & DOWN_BUTTON && isWalkable(ObjectType::Player, 0, 1)) {
        game.getPlayer().setY(game.getPlayer().getY() + 1);
        updateGreenDoors();
        incLavaAndWater();
        fix_World_Y_Offset();

    }


    // ---------------------------------------------------------------------
    // Have we won?
    // ---------------------------------------------------------------------

    if (game.getPlayer().getX() == game.getPortal().getX() && game.getPlayer().getY() == game.getPortal().getY()) {
    
        Serial.println("Level Complete");

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
        
            Serial.println("Portal Key");
            game.decPortalKeyCount();
            game.getPortalKey(i).setX(255);
            game.getPortalKey(i).setY(255);

            if (game.getPortalKeyCount() == 0) {
            
                game.getPortal().setOpen(true);
                
            }

        }
    
    }


}


void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    uint8_t currentPlane = a.currentPlane();
    if (a.needsUpdate()) play_Update();

    uint24_t levelIdx = FX::readIndexedUInt24(Images::Levels, game.getLevel());
    SpritesU::drawOverwriteFX(0, -game.getWorld_Y_Offset() * 8, levelIdx, currentPlane);


    for (uint16_t y = game.getWorld_Y_Offset(); y < Constants::Map_Y_Count; y++) {

        if ((y - game.getWorld_Y_Offset()) * 8 > 63) continue;

        for (uint16_t x = 0; x < Constants::Map_X_Count; x++) {

            if (mapData[y][x] >= Constants::Tile_Counter_00 && mapData[y][x] <= Constants::Tile_Counter_65) {

                SpritesU::drawOverwriteFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Numbers_5x3_2D_WB, ((mapData[y][x] - Constants::Tile_Counter_00) * 3) + currentPlane);

            }
            else if (mapData[y][x] == Constants::Tile_Waters_Edge) {

                SpritesU::drawPlusMaskFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Waters_Edge * 3) + currentPlane);

            }
            else if (mapData[y][x] == Constants::Tile_Green_Switch) {

                SpritesU::drawPlusMaskFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Green_Switch * 3) + currentPlane);

            }
            else if (mapData[y][x] == Constants::Tile_Partial_Wall) {

                SpritesU::drawPlusMaskFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Partial_Wall * 3) + currentPlane);

            }
            else if (mapData[y][x] == Constants::Tile_Lava_And_Partial_Wall) {

                SpritesU::drawPlusMaskFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Tiles, ((Constants::Image_Lava_And_Partial_Wall + ((game.getFrameCount() % 32) / 8)) * 3) + currentPlane);

            }
            else if (mapData[y][x] == Constants::Tile_Water_And_Partial_Wall) {

                SpritesU::drawPlusMaskFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Tiles, ((Constants::Image_Water_And_Partial_Wall + ((game.getFrameCount() % 32) / 8)) * 3) + currentPlane);

            }
            else if (mapData[y][x] == Constants::Tile_Lava) {

                uint8_t len = 1;

                for (uint8_t i = 1; i < 8; i++) {

                    if (mapData[y][x + i] == Constants::Tile_Lava) {
                        len = i + 1;
                    }
                    else {
                       
                        break;
                    }

                }

                uint24_t imgIdx = FX::readIndexedUInt24(Images::Lavas, len - 1);
                SpritesU::drawOverwriteFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, imgIdx, (((game.getFrameCount() % 32) / 8) * 3) + currentPlane);

                x = x + len - 1;

            }

            else if (mapData[y][x] == Constants::Tile_Water) {

                uint8_t len = 1;

                for (uint8_t i = 1; i < 8; i++) {

                    if (mapData[y][x + i] == Constants::Tile_Water) {
                        len = i + 1;
                    }
                    else {
                       
                        break;
                    }

                }

                uint24_t imgIdx = FX::readIndexedUInt24(Images::Waters, len - 1);
                SpritesU::drawOverwriteFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, imgIdx, (((game.getFrameCount() % 32) / 8) * 3) + currentPlane);

                x = x + len - 1;

            }

            if (mapData[y][x] == Constants::Tile_Basalt) {

                SpritesU::drawPlusMaskFX(x * 8, (y - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Basalt * 3) + currentPlane);

            }
        
        }
    
    }


    // Blocks ..

    for (uint8_t i = 0; i < Constants::Block_Count; i++) {

        Block &block = game.getBlock(i);

        if (block.isActive()) {
            SpritesU::drawPlusMaskFX(block.getX() * 8, (block.getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Block * 3) + currentPlane);
        }
        else {
            break;
        }

    }


    // Portal Keys ..

    for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {

        PortalKey &key = game.getPortalKey(i);

        if (key.isActive()) {
            SpritesU::drawPlusMaskFX(key.getX() * 8, (key.getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, ((5 + ((game.getFrameCount() % 36) / 12)) * 3) + currentPlane);
        }

    }



    // Green Doors ..

    for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

        GreenDoor &greenDoor = game.getGreenDoor(i);

        if (mapData[greenDoor.getY()][greenDoor.getX()] != Constants::Tile_Basalt) {

            if (greenDoor.isActive()) {

                if (greenDoor.isOpen()) {
                    SpritesU::drawPlusMaskFX(greenDoor.getX() * 8, (greenDoor.getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Green_Open * 3) + currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(greenDoor.getX() * 8, (greenDoor.getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Green_Closed * 3) + currentPlane);
                }

            }

        }

    }


    // Portal ..

    if (game.getPortal().isOpen()) {
        // Serial.print("> ");
        // Serial.print((game.getFrameCount() % 32) / 8);
        // Serial.println("");
        SpritesU::drawPlusMaskFX(game.getPortal().getX() * 8, (game.getPortal().getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, ((Constants::Image_Portal + ((game.getFrameCount() % 40) / 8)) * 3) + currentPlane);
    }
    else {
    // Serial.println("sdas");
        SpritesU::drawPlusMaskFX(game.getPortal().getX() * 8, (game.getPortal().getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Portal_Inactive * 3) + currentPlane);
    }


    // Player ..

    SpritesU::drawPlusMaskFX(game.getPlayer().getX() * 8, (game.getPlayer().getY() - game.getWorld_Y_Offset()) * 8, Images::Tiles, (Constants::Image_Player * 3) + currentPlane);


}
