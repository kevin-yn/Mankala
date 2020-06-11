//
//  Game.cpp
//  Project3
//
//  Created by Calvin Wang on 5/12/19.
//  Copyright © 2019 Calvin Wang. All rights reserved.
//
#include "Game.h"
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

Game::Game(const Board& b, Player* south, Player* north) :_south(south), _north(north), curr_side(SOUTH) {
    board = new Board(b);
}

Game::~Game() {
    delete board;
}

void Game::display() const {
    cout<<"current Board:"<<endl;
    cout<<"         ";
    for (int i = 1; i <= board->holes(); i++) {
        cout<<"("<<i<<")"<<board->beans(NORTH, i)<<"  ";
    }
    cout<<""<<endl;
    cout<<"(N)"<<board->beans(NORTH, 0)<<"                                           "<<"(S)"<<board->beans(SOUTH, 0)<<endl;
    cout<<"         ";
    for (int i = 1; i <= board->holes(); i++) {
      cout<<"("<<i<<")"<<board->beans(SOUTH, i)<<"  ";
    }
    cout<<""<<endl;
}

void Game::Game::status(bool& over, bool& hasWinner, Side& winner) const {
    if (board->beansInPlay(SOUTH) > 0 && board->beansInPlay(NORTH) > 0) {
        over = false;
        return;
    } else {
        over = true;
        hasWinner = true;
        if (board->beans(SOUTH, 0) + board->beansInPlay(SOUTH) >  board->beans(NORTH, 0) + board->beansInPlay(NORTH)) {
            winner = SOUTH;
            return;
        } else if (board->beans(SOUTH, 0) <  board->beans(NORTH, 0)) {
            winner = NORTH;
            return;
        } else {
            hasWinner = false;
        }
    }
}

bool Game::move() {
    bool over = false;
    bool hasWinner = false;
    Side winner = SOUTH;
    status(over, hasWinner, winner);
    if (over) {
        sweep();
        return false;
    }
    //
    while (true) {
        if (board->beansInPlay(SOUTH) == 0 || board->beansInPlay(NORTH) == 0) {
            sweep();
            return true;
        }
        //display();
        if (curr_side == NORTH) {
            cout<<"NORTH TURN"<<endl;
        } else {
            cout<<"SOUTH TURN"<<endl;
        }
        Player* currPlayer = (curr_side == SOUTH)? _south: _north;
        int hole = currPlayer->chooseMove(*board, curr_side);
        Side endSide = SOUTH;
        int endHole = 10;
        //if it's not iterative, need to wait for enter to proceed
//        if (!currPlayer->isInteractive()) {
//            while (true) {
//                cout << "Press the ENTER key to proceed"<<endl;
//                cin.ignore();
//                if (cin.get() == '\n') {
//                     break;
//                }
//            }
//        }
        board->sow(curr_side, hole, endSide, endHole);
        if (endHole == 0) {
          display();
          continue;
        } else {
            if (endSide == curr_side && (board->beans(endSide, endHole) == 1) && (board->beans(opponent(endSide), endHole))!= 0) {
                board->moveToPot(opponent(curr_side), endHole, curr_side);
                board->moveToPot(curr_side, endHole, curr_side);
            }
            break;
        }
    }
    display();
    curr_side = opponent(curr_side);
    return true;
}

void Game::Game::play() {
    cout<<"START:"<<endl;
    display();
    while (move()) {

    }
    //display();
    bool over = true;
    bool hasWinner = true;
    Side Winner = SOUTH;
    status(over, hasWinner, Winner);
    cout<<"the game has ended, ";
    if (!hasWinner) {
        cout<<"and it's a draw"<<endl;
    } else if (Winner == SOUTH) {
        cout<<_south->name()<<" wins"<<endl;
    } else {
        cout<<_north->name()<<" wins"<<endl;
    }
}

int Game::beans(Side s, int hole) const {
    return board->beans(s, hole);
}

void Game::sweep() {
    if (board->beansInPlay(SOUTH) > 0) {
        for (int i = 1; i <= board->holes(); i++) {
            board->moveToPot(SOUTH, i, SOUTH);
        }
    } else {
        for (int i = 1; i <= board->holes(); i++) {
            board->moveToPot(NORTH, i, NORTH);
        }
    }
}
