#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"


void title_Init() {

    gameState = GameState::Title_Main;
    titleCounter = 0;
    game.resetFrameCount();

}

void title_Update() {

    // game.incFrameCount();

    // uint8_t justPressed = getJustPressedButtons();
    // uint8_t pressed = getPressedButtons();

    // if ((titleCounter == 24) && (justPressed & A_BUTTON)) {

    //     a.initRandomSeed(); 
    //     gameState = GameState::Play_Init;

    // }

    // if (titleCounter < 24) titleCounter++;


    game.incFrameCount();

    uint8_t justPressed = getJustPressedButtons();
    uint8_t aReleased = getJustReleasedButtons(A_BUTTON);
    uint8_t bReleased = getJustReleasedButtons(B_BUTTON);
    uint8_t pressed = getPressedButtons();

    switch (gameState) {

        case GameState::Title_Main:

            if ((game.getFrameCount() > 24) && (justPressed & A_BUTTON)) {

                a.initRandomSeed(); 
                gameState = GameState::Title_Select;
                levelSelect.aCounter = 11;
                titleCounter = 0;

            }
            break;

        case GameState::Title_Select:

            if (bReleased && levelSelect.bCounter < 10) {
                gameState = GameState::Title_Init;
            }

            if (aReleased && levelSelect.aCounter < 10) {

                game.setLevel(levelSelect.getSelectedPuzzle());
                gameState = GameState::Play_Init;
            }

            if (pressed & A_BUTTON) {
                levelSelect.aCounter++;
            }
            else {
                levelSelect.aCounter = 0;
            }

            if (pressed & B_BUTTON) {
                levelSelect.bCounter++;
            }
            else {
                levelSelect.bCounter = 0;
            }

            if (levelSelect.aCounter > 32 && levelSelect.bCounter > 32) {
                gameState = GameState::Title_Clear_Progress;        
            }

            if (justPressed & LEFT_BUTTON && levelSelect.x > 0) {

                levelSelect.x--;

            }

            if (justPressed & RIGHT_BUTTON && levelSelect.x < 4 && game.getPuzzle(levelSelect.getSelectedPuzzle() + 1).getStatus() != PuzzleStatus::Locked) {

                levelSelect.x++;

            }

            if (justPressed & DOWN_BUTTON && levelSelect.y < 7 && game.getPuzzle(levelSelect.getSelectedPuzzle() + 5).getStatus() != PuzzleStatus::Locked) {

                levelSelect.y++;

            }

            if (justPressed & UP_BUTTON && levelSelect.y > 0) {

                levelSelect.y--;

            }

            break;

        case GameState::Title_Clear_Progress:

            if (justPressed & A_BUTTON) {

                cookieReset();
                saveCookie();
                levelSelect.aCounter = 11;
                levelSelect.bCounter = 11;   
                gameState = GameState::Title_Select;

            }

            if (justPressed & B_BUTTON) {

                gameState = GameState::Title_Select;

            }

            break;

    }

    if (titleCounter < 254) {
        titleCounter++;
    }

}

int8_t xPos[] = { -125, -112, -101, -90, -80, -70, -61, -52, -44, -36, -29, -22, -16, -10, -5, 0, 4, 8, 12, 15, 18, 20, 22, 23, 24, 24, 25, 25, 25, };

void title(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    // if (a.needsUpdate()) title_Update();

    // uint8_t currentPlane = a.currentPlane();


    // SpritesU::drawPlusMaskFX(0, 4, Images::Scoundrel, currentPlane);
        

    if (a.needsUpdate()) title_Update();

    uint8_t currentPlane = a.currentPlane();
    uint8_t yOffset = Constants::levelSelect_Offset[levelSelect.y];

    switch (gameState) {

        case GameState::Title_Main:
            
            SpritesU::drawPlusMaskFX(0, 4, Images::Scoundrel, currentPlane);

            // SpritesU::drawOverwriteFX(0, 0, Images::Background_128x64, (cookie.blackAndWhite?  3 : 0) + currentPlane);

            // if (titleCounter >= 30 && titleCounter < 59) {
            //     SpritesU::drawPlusMaskFX(xPos[titleCounter - 30], 1, Images::Title_Free, currentPlane);
            // }
            // else if (titleCounter >= 59) {
            //     SpritesU::drawPlusMaskFX(26, 1, Images::Title_Free, currentPlane);
            // }

            // if (titleCounter > 30 && titleCounter < 59) {
            //     SpritesU::drawPlusMaskFX(-xPos[titleCounter - 30] + 62, 22, Images::Title_The, currentPlane);
            // }
            // else if (titleCounter >= 59) {
            //     SpritesU::drawPlusMaskFX(36, 22, Images::Title_The, currentPlane);
            // }

            // if (titleCounter >= 30 && titleCounter < 59) {
            //     SpritesU::drawPlusMaskFX(xPos[titleCounter - 30] + 10, 43, Images::Title_Key, currentPlane);
            // }
            // else if (titleCounter >= 59) {
            //     SpritesU::drawPlusMaskFX(36, 43, Images::Title_Key, currentPlane);
            // }

            // if (game.getFrameCount() > 192) {
            //     SpritesU::drawPlusMaskFX(103, 48, Images::Switch, currentPlane);
            // }

            break;

        case GameState::Title_Select:

            // if (titleCounter < 40) {

                // SpritesU::drawOverwriteFX(0, 0, Images::Background_128x64, currentPlane);

                // if (titleCounter >= 10 && titleCounter < 39) {
                //     SpritesU::drawPlusMaskFX(52 - xPos[28 - (titleCounter - 10)], 1, Images::Title_Free, currentPlane);
                // }
                // else {
                //     SpritesU::drawPlusMaskFX(26, 1, Images::Title_Free, currentPlane);
                // }

                // if (titleCounter > 10 && titleCounter < 39) {
                //     SpritesU::drawPlusMaskFX(10 + xPos[28 - (titleCounter - 10)], 22, Images::Title_The, currentPlane);
                // }
                // else {
                //     SpritesU::drawPlusMaskFX(36, 22, Images::Title_The, currentPlane);
                // }

                // if (titleCounter >= 10 && titleCounter < 39) {
                //     SpritesU::drawPlusMaskFX(64 - xPos[28 - (titleCounter - 10)], 43, Images::Title_Key, currentPlane);
                // }
                // else {
                //     SpritesU::drawPlusMaskFX(36, 43, Images::Title_Key, currentPlane);
                // }

            // }
            // else {                

                for (uint8_t y = 0; y < 8; y++) {

                    for (uint8_t x = 0; x < 5; x++) {

                        Puzzle &puzzle = game.getPuzzle((y * 5) + x);

                        if (puzzle.getStatus() == PuzzleStatus::Complete) {

                            SpritesU::drawOverwriteFX(x * 15, (y * 15) - yOffset, Images::Levels_Select, ((((y * 5) + x) + 1) * 3) + currentPlane);

                        }
                        else if (puzzle.getStatus() == PuzzleStatus::InProgress) {

                            SpritesU::drawOverwriteFX(x * 15, (y * 15) - yOffset, Images::Levels_Select, ((((y * 5) + x) + 51) * 3) + currentPlane);

                        }                        
                        else {

                            SpritesU::drawOverwriteFX(x * 15, (y * 15) - yOffset, Images::Levels_Select, (91 * 3) + currentPlane);

                        }

                        if (((y * 5) + x) == levelSelect.getSelectedPuzzle()) {

                            if (game.getFrameCount(48)) {
                                SpritesU::drawPlusMaskFX(x * 15, (y * 15) - yOffset, Images::Levels_Cursor, currentPlane);
                            }

                        }

                    }
                    
                }

                SpritesU::drawOverwriteFX(128 - 53, 0, Images::Levels_HUD, (levelSelect.y * 3) + currentPlane);
                SpritesU::drawOverwriteFX(128 - 53 + 40, 2, Images::Levels_Number, (levelSelect.getSelectedPuzzle() * 3) + currentPlane);
                SpritesU::drawOverwriteFX(128 - 53 + 9, 17, Images::Levels_Status, (static_cast<uint8_t>(game.getPuzzle(levelSelect.getSelectedPuzzle()).getStatus()) * 3) + currentPlane);

                if (game.getPuzzle(levelSelect.getSelectedPuzzle()).getStatus() == PuzzleStatus::Complete) {

                    SpritesU::drawOverwriteFX(128 - 53 + 7, 32, Images::Levels_NumberOfMoves, currentPlane);
                    SpritesU::drawOverwriteFX(113, 41, Images::Levels_NumberOfMoves_Numbers, ((game.getPuzzle(levelSelect.getSelectedPuzzle()).getNumberOfMoves() / 100) * 3) + currentPlane);
                    SpritesU::drawOverwriteFX(118, 41, Images::Levels_NumberOfMoves_Numbers, (((game.getPuzzle(levelSelect.getSelectedPuzzle()).getNumberOfMoves() % 100) / 10) * 3) + currentPlane);
                    SpritesU::drawOverwriteFX(123, 41, Images::Levels_NumberOfMoves_Numbers, (((game.getPuzzle(levelSelect.getSelectedPuzzle()).getNumberOfMoves() % 100) % 10) * 3) + currentPlane);

                }

            // }

            break;

        case GameState::Title_Clear_Progress:
            SpritesU::drawOverwriteFX(36, 25, Images::ClearProgress, currentPlane);
            break;

    }            

}
