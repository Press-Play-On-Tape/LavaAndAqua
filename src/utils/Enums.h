#pragma once



enum class ObjectType : uint8_t { 
    Player,
    Lava, 
    Block,
    Water, 
};

// inline CursorPosition &operator++(CursorPosition &c) {
//     c = static_cast<CursorPosition>( static_cast<uint8_t>(c) + 1 );
//     return c;
// }

// inline CursorPosition operator++(CursorPosition &c, int) {
//     CursorPosition result = c;
//     ++c;
//     return result;
// }

// inline CursorPosition &operator--(CursorPosition &c) {
//     c = static_cast<CursorPosition>( static_cast<uint8_t>(c) - 1 );
//     return c;
// }

// inline CursorPosition operator--(CursorPosition &c, int) {
//     CursorPosition result = c;
//     --c;
//     return result;
// }

enum class Suit : uint8_t { 
    Spades, 
    Clubs, 
    Diamonds, 
    Hearts, 
    No_Trumps = 254, 
    None = 255, 
};

inline Suit &operator++(Suit &c) {
    c = static_cast<Suit>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline Suit operator++(Suit &c, int) {
    Suit result = c;
    ++c;
    return result;
}


inline Suit &operator--(Suit &c) {
    c = static_cast<Suit>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline Suit operator--(Suit &c, int) {
    Suit result = c;
    --c;
    return result;
}

enum class Rank : uint8_t { 
    None = 0, 
    Two = 2, 
    Three, 
    Four, 
    Five, 
    Six, 
    Seven, 
    Eight, 
    Nine, 
    Ten, 
    Jack, 
    Queen, 
    King, 
    Ace, 
};

inline Rank &operator++(Rank &c) {
    c = static_cast<Rank>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline Rank operator++(Rank &c, int) {
    Rank result = c;
    ++c;
    return result;
}

inline Rank &operator--(Rank &c) {
    c = static_cast<Rank>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline Rank operator--(Rank &c, int) {
    Rank result = c;
    --c;
    return result;
}


enum class GameState : uint8_t {

    SplashScreen_Start,
        SplashScreen_00 = SplashScreen_Start,
        SplashScreen_01,
        SplashScreen_02,
        SplashScreen_03,
    SplashScreen_End,

    Title_Init,
    Title_Start,
        Title_OptPlay = Title_Start,
    Title_End,

    Play_Init,
    Play_Start,
        Play_Shuffle = Play_Start,
        Play_Deal_00, 
        Play_Deal_01, 
        Play_Deal_02, 
        Play_Deal_03, 
        Play, 
        Equip,
        RestoreHealth,
        BurnCard,
        ChooseFight,
        YouWin,
        YouLose,
        Score,
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
