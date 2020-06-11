//
//  Player.cpp
//  Project3
//
//  Created by Calvin Wang on 5/12/19.
//  Copyright © 2019 Calvin Wang. All rights reserved.
//

#include "Player.h"
#include <iostream>
using std::string;
using std::cin;
//Player class functions::

Player::Player(std::string name) {
    _name = name;
}
std::string Player::name() const {
    return _name;
}
bool Player::isInteractive() const {
    return false;
}
Player::~Player() {
    //?
}


SmartPlayer::SmartPlayer(string name): Player(name)  {
}
bool SmartPlayer::isInteractive() const {
    return false;
}
int SmartPlayer::chooseMove(const Board& b, Side s) const {
    Board* board = new Board(b);
    Node* root_ = new Node(board, s, 0);
    root_->number = 1;
    int nega = -1410065408;
    int posi = 1410065408;
    if (s == SOUTH) {
        root_->_constructor_helper(root_, 4, nega);
    } else if (s == NORTH) {
        root_->_constructor_helper(root_, 4, posi);
    }
    //start to choose move
    if (root_->is_leaf) {
        cout<<"SmartPlayer chooseMove asked to choose when the game is over";
        for (int i = 1; i <= b.holes(); i++) {
            if (b.beans(s, i) > 0) {
                cout<<"smart player plays  "<<i<<endl;
                delete root_;
                return i;
            }
        }
    }
  //choose the good move

    for (Node* curr : root_->children) {
        if (root_->value == curr->value) {
            cout<<"smart player plays  "<<curr->move<<endl;

            int tmp = curr->move;
            delete root_;
            return tmp;
        }
    }
    cout<<"smart player wrong wrong wrong wrong"<<endl;
    return -1;
}
SmartPlayer::~SmartPlayer() {

}



BadPlayer::BadPlayer(string name): Player(name) {}
bool BadPlayer::isInteractive() const {
    return false;
}
int BadPlayer::BadPlayer::chooseMove(const Board& b, Side s) const {
    int holes = b.holes();
    for (int i = 1; i <= holes; i++) {
        if (b.beans(s, i) > 0) {
            return i;
        }
    }
    return -1;
}
BadPlayer::~BadPlayer() {

}


//HumanPlayer class functions
HumanPlayer::HumanPlayer(string name): Player(name)  {}
bool HumanPlayer::isInteractive() const {
    return true;
}
int HumanPlayer::chooseMove(const Board& b, Side s) const {
    int move;
    std::cout<<"Please make a move from 1 to "<<b.holes()<<":"<<std::endl;
    cin >> move;
    while(b.beans(s, move) == 0 || b.beans(s, move) == -1) {
        std::cout<<"This move is invalid. Please make a different move from 1 to "<<b.holes()<<std::endl;
        cin >> move;
    }
    return move;
}
HumanPlayer::~HumanPlayer() {}


LessSmartPlayer::LessSmartPlayer(string name): Player(name)  {
}
bool LessSmartPlayer::isInteractive() const {
    return false;
}
int LessSmartPlayer::chooseMove(const Board& b, Side s) const {
    int bestHole = 0;
    int value = 0;
    Board* new_board = new Board(b);
    chooseMove_helper(s, new_board, bestHole, value, 0, 4);
    return bestHole;
}
LessSmartPlayer::~LessSmartPlayer() {

}

void LessSmartPlayer::chooseMove_helper(Side side, Board* board, int& bestHole, int& value, int level, int maximum_level) const {
    //base case: 1. level deep enough. 2. the game is already game_over
    if (level > maximum_level || game_over(board)) {
        value = evaluate(board);
        // i dont think this matter
        bestHole = -1;
        return;
    }

    //here, the game is not over, there must be at least one move to make;
    if (side == SOUTH) {
        //since we are looking for the maximum value, we set it to be very small first;
        value = negative_infinity - 1;
    } else if (side == NORTH) {
        value = positive_infinity + 1;
    }

    //go through all the possible moves
    for (int i = 1; i <= board->holes(); i++) {
        if (board->beans(side, i) == 0) {
            continue;
        }
        bool another_turn = false;
        Board* new_board = _move(board, side, i, another_turn);
        if (!another_turn) {
            int new_value = 0;
            int new_bestHole = 0;
            chooseMove_helper(opponent(side), new_board, new_bestHole, new_value, level + 1, maximum_level);
            if (new_value > value && side == SOUTH) {
                value = new_value;
                bestHole = i;
            } else if (new_value < value && side == NORTH) {
                value = new_value;
                bestHole = i;
            }
        } else {
            //because the same side makes another move, i need the recursive helper here
            vector<Board*> boards;
            recurse_helper(board, boards, side);
            for (Board* b : boards) {
              int new_value = 0;
              int new_bestHole = 0;
              chooseMove_helper(opponent(side), b, new_bestHole, new_value, level + 1, maximum_level);
              if (new_value > value && side == SOUTH) {
                  value = new_value;
                  bestHole = i;
              } else if (new_value < value && side == NORTH) {
                  value = new_value;
                  bestHole = i;
              }
            }
        }
    }




    //delete board;
    return;
}

int LessSmartPlayer::evaluate(Board* board)const {
    if (board->beans(SOUTH, 0) > board->totalBeans()/2) {
        return positive_infinity;
    }
    if (board->beans(NORTH, 0) > board->totalBeans()/2) {
        return negative_infinity;
    }
    if (board->beansInPlay(SOUTH) == 0 || board->beansInPlay(NORTH) == 0) {
        if (board->beansInPlay(SOUTH) + board->beans(SOUTH, 0) == board->beansInPlay(NORTH) + board->beans(NORTH, 0)) {
            return 0;
        } else if (board->beansInPlay(SOUTH) + board->beans(SOUTH, 0) > board->beansInPlay(NORTH) + board->beans(NORTH, 0)) {
            return positive_infinity;
        } else {
            return negative_infinity;
        }
    }
    return board->beans(SOUTH, 0) - board->beans(NORTH, 0);
}

void LessSmartPlayer::recurse_helper(Board* curr_board, vector<Board*>& v, Side s)const {
    bool another_turn = false;
    for (int i = 1; i <= curr_board->holes(); i++) {
        if (curr_board->beans(s, i) > 0) {
            Board* new_board = _move(curr_board, s, i, another_turn);

            if (another_turn) {
                recurse_helper(new_board, v, s);
                //delete new_board;
            } else {
                v.push_back(new_board);
            }
        }
    }
    //delete curr_board;
}

Board* LessSmartPlayer::_move(const Board* board, Side side, int hole, bool& another_turn)const {
    Board* new_board = new Board(*board);
    Side endSide = SOUTH;
    int endHole = 10;
    new_board->sow(side, hole, endSide, endHole);
    if (endHole == 0) {
        if (game_over(new_board)) {
            another_turn = false;
        } else {
            another_turn = true;
        }
    } else {
        another_turn = false;
        if (endSide == side && new_board->beans(endSide, endHole) == 1
          && new_board->beans(opponent(endSide), endHole) != 0) {
              new_board->moveToPot(side, endHole, side);
              new_board->moveToPot(opponent(side), endHole, side);
        }
    }
    return new_board;
}



bool LessSmartPlayer::game_over(Board* board)const {
    if (board->beansInPlay(SOUTH) == 0 || board->beansInPlay(NORTH) == 0) {
        return true;
    } else if (board->beans(SOUTH, 0) > board->totalBeans()/2 || board->beans(NORTH, 0) > board->totalBeans()/2) {
        return true;
    }
    return false;
}

// bool LessSmartPlayer::hasWinner(Side winner, Board* board) {
//     if (board->beansInPlay(SOUTH) + board->beans(SOUTH, 0) == board->beansInPlay(NORTH) + board->beans(NORTH, 0)) {
//         return false;
//     }
//     if (board->beansInPlay(SOUTH) + board->beans(SOUTH, 0) > board->beansInPlay(NORTH) + board->beans(NORTH, 0)) {
//         winner = SOUTH;
//     } else {
//         winner = NORTH;
//     }
//     return true;
// }
