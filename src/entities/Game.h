#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Block.h"
#include "Player.h"
#include "Portal.h"
#include "GreenDoor.h"
#include "PortalKey.h"

struct Game {

    private:

        uint8_t level = 0;
        uint8_t portalKeyCount = 0;
        uint8_t world_Y_Offset = 0;
        uint16_t randomSeed = 0;
        uint16_t frameCount = 0;
        Player player;
        Portal portal;
        GreenDoor greenDoors[Constants::Green_Door_Count];

        Block blocks[Constants::Block_Count];
        PortalKey portalKeys[Constants::Portal_Key_Count];

    public:

        uint16_t getFrameCount()                        { return this->frameCount; }
        uint16_t getFrameCount(uint8_t val)             { return this->frameCount % val < val / 2; }
        uint8_t getWorld_Y_Offset()                     { return this->world_Y_Offset; }
        uint8_t getLevel()                              { return this->level; }
        uint8_t getPortalKeyCount()                     { return this->portalKeyCount; }

        Player &getPlayer()                             { return this->player; }
        Portal &getPortal()                             { return this->portal; }
        Block &getBlock(uint8_t idx)                    { return this->blocks[idx]; }
        PortalKey &getPortalKey(uint8_t idx)            { return this->portalKeys[idx]; }
        GreenDoor &getGreenDoor(uint8_t idx)            { return this->greenDoors[idx]; }

        void setRandomSeed(uint16_t val)                { this->randomSeed = val; }
        void setFrameCount(uint16_t val)                { this->frameCount = val; }
        void setWorld_Y_Offset(uint8_t val)             { this->world_Y_Offset = val; }
        void setLevel(uint8_t val)                      { this->level = val; }
        void setPortalKeyCount(uint8_t val)             { this->portalKeyCount = val; }

        void resetLevel() {
        
            for (uint8_t i = 0; i < Constants::Block_Count; i++) {
            
                this->blocks[i].setX(255);
                this->blocks[i].setY(255);

            }

            for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {

                this->portalKeys[i].setX(255);
                this->portalKeys[i].setY(255);

            }

        }

        void incFrameCount() {

            this->frameCount++;

        }
        void decPortalKeyCount() {

            this->portalKeyCount--;

        }

        void resetFrameCount() {

            this->frameCount = 0;
            
        }

};