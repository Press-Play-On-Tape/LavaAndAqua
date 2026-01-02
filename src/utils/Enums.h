#pragma once


struct LevelSelect {

    uint8_t selectedPuzzle = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t aCounter = 0;
    uint8_t bCounter = 0;

    uint8_t getSelectedPuzzle() {

        return (this->y * 4) + this->x;
        
    }

    void increaseGame() {

        this->x++;
        if (this->x == 5) {
            this->x = 0;
            this->y++;
        }

    }

};

enum PuzzleStatus : uint8_t { 
    Locked,
    Complete,
    InProgress,
};

enum class ObjectType : uint8_t { 
    Player,
    Lava, 
    Block,
    Water, 
};

enum class GameState : uint8_t {

    SplashScreen_Start,
        SplashScreen_00 = SplashScreen_Start,
        SplashScreen_01,
        SplashScreen_02,
        SplashScreen_03,
    SplashScreen_End,

    Title_Init,
    Title_Start,
        Title_Main = Title_Start,
        Title_Select,
        Title_Clear_Progress,
    Title_End,

    Play_Init,
    Play_Start,
        Play_Shuffle = Play_Start,
        Play_Deal_00, 
        RestartGame,
    Play_End,
    
};

inline GameState &operator++(GameState &c) {
    c = static_cast<GameState>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline GameState operator++(GameState &c, int) {
    GameState result = c;
    ++c;
    return result;
}

inline GameState &operator--(GameState &c) {
    c = static_cast<GameState>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline GameState operator--(GameState &c, int) {
    GameState result = c;
    --c;
    return result;
}

enum Direction : uint8_t { 
    Down,
    Up
};


enum PuffMode : uint8_t { 
    EquipWeapon,
    Run,
    Heart,
    Burn,
    Fight_BareHand,
    Fight_Weapon,
};
