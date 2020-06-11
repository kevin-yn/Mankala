//
//  Game.hpp
//  Project3
//
//  Created by Calvin Wang on 5/12/19.
//  Copyright © 2019 Calvin Wang. All rights reserved.
//

#ifndef Game_h
#define Game_h
#include "Player.h"

class Game {
public:
    //Board needs a custom copy constructor
    Game(const Board& b, Player* south, Player* north);
    void display() const;
    void status(bool& over, bool& hasWinner, Side& winner) const;
    bool move();
    void play();
    int beans(Side s, int hole) const;
    ~Game();

private:
    void sweep();
    Board* board;
    Player* _south;
    Player* _north;
    Side curr_side;
};

#endif /* Game_hpp */
