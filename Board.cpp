//
//  Board.cpp
//  Project3
//
//  Created by Calvin Wang on 5/12/19.
//  Copyright © 2019 Calvin Wang. All rights reserved.
//

#include <stdio.h>
#include "Board.h"
#include <iostream>
using std::endl;
using std::cout;


Board::Board(int nHoles, int nInitialBeansPerHole) {
    //Construct a Board with the indicated number of holes per side (not counting the pot) and initial number of beans per hole. If nHoles is not positive, act as if it were 1; if nInitialBeansPerHole is negative, act as if it were 0.
    initial_num = (nInitialBeansPerHole >= 0) ? nInitialBeansPerHole: 0;
    num_holes = (nHoles > 0) ? nHoles : 1;
    num_total = 2 * num_holes * initial_num;
    north.push_back(0);
    south.push_back(0);
    for (int i = 0; i < num_holes; i++) {
        north.push_back(initial_num);
        south.push_back(initial_num);
    }
}

Board::Board(const Board& board) {
    _copy(board);
}

int Board::holes() const {
    return num_holes;
}
int Board::beans(Side s, int hole) const {
    if (hole > num_holes || hole < 0) {return -1;}
    if (s == NORTH) {
        //cout<<north.size()<<"  "<<hole<<endl;
        return north[hole];
    } else {
        // here im reversing the order of index in South
        if (hole == 0) {
            return south[0];
        }
        return south[num_holes + 1 - hole];
    }
}
//this helper function return the number of beans in play on one Side
//It DOES NOT INCLUDE POT
int Board::_beans_helper(vector<int> v) const {
    int total = 0;
    for (unsigned long i = 1; i < v.size(); i++) {
       total += v[i];
    }
    return total;
}
int Board::beansInPlay(Side s) const {
    if (s == SOUTH) {return _beans_helper(south);}
    return _beans_helper(north);

}
int Board::totalBeans() const {
    return num_total;
}
//tasks for sow
//1. just sow
//2. change endSide
//3. change end
bool Board::sow(Side s, int hole, Side& endSide, int& endHole) {
    //cout<<holes()<<" "<<hole<<endl;
    if (hole > num_holes) {return false;}
    int num_beans = beans(s, hole);
    if (num_beans == 0) {return false;}
    // if it's south, reverse the index of the hole
    if (s == SOUTH) {
        hole = num_holes + 1 - hole;
    }
    //first change the hole to be zero
    bool if_same_side = true;
    if (s == SOUTH) {
        south[hole] = 0;
        if_same_side = _sow_helper(south, north, hole, num_beans, endHole);
    } else {
        north[hole] = 0;
        if_same_side = _sow_helper(north, south, hole, num_beans, endHole);
    }
    //when chaning endHole!, also need to check if it's south. Should reverse the index
    if (!if_same_side) {
        if (s == SOUTH) {
            endSide = NORTH;
        } else {
            endSide = SOUTH;
        }
    } else {
        endSide = s;
    }
    if (endSide == SOUTH && endHole != 0) {
        endHole = num_holes + 1 - endHole;
    }
    return true;
}
//starting_hole is the NOT the first hole to start to sow in
// it is the same as hole in sow.
//return true if it stays on the same Side, therefore just return at_v1!
bool Board::_sow_helper(vector<int>& v1, vector<int>& v2, int hole, int num_beans, int& endHole) {
    bool at_v1 = true;
    for (int i = 0; i < num_beans; i++) {
        if (!_next_hole(hole, at_v1)) {
            //if change side, then reverse true;
            //at_v1 = (at_v1)? false: true;
            if (at_v1) {
               at_v1 = false;
            } else {
               at_v1 = true;
            }
        }
        if (at_v1) {
            v1[hole]++;
        } else {
            v2[hole]++;
        }
    }
    endHole = hole;
    return at_v1;
}

//return true if stays on the same side
bool Board::_next_hole(int& hole, bool at_v1) const {
    if (hole == 0 || (hole == 1 && !at_v1)) {
        hole = num_holes;
        return false;
    } else {
        hole--;
        return true;
    }
}

bool Board::moveToPot(Side s, int hole, Side potOwner) {
    if (hole == 0 || hole > num_holes) {
        return false;
    }
    int num = 0;
    if (s == SOUTH) {
        num = south[num_holes + 1 - hole];
        south[num_holes + 1 - hole] = 0;
    } else {
        num = north[hole];
        north[hole] = 0;
    }
    if (potOwner == SOUTH) {
        south[0] += num;
    } else {
        north[0] += num;
    }
    return true;
}
bool Board::setBeans(Side s, int hole, int bean) {
    if (hole > num_holes || bean < 0) {return false;}
    num_total = num_total - beans(s, hole) + bean;
    if (s == SOUTH) {
        if (hole == 0) {
            south[0] = bean;
            return true;
        }
        south[num_holes + 1 - hole] = bean;
    } else {
        north[hole] = bean;
    }
    return true;
}

void Board::_print_board() const {
    cout<<" print board called"<<endl;
    for (int i = 0; i <= num_holes; i++) {
        cout<<"("<<i<<")"<<north[i]<<"  ";
    }
    cout<<""<<endl;
    cout<<"      ";
    for (int i = num_holes; i > 0; i--) {
        cout<<"("<<num_holes + 1 - i<<")"<<south[i]<<"  ";
    }
    cout<<"(0)"<<south[0]<<"  ";
    cout<<""<<endl;
}

void Board::_copy(const Board& board) {
    this->num_holes = board.num_holes;
    this->num_total = board.num_total;
    this->initial_num = board.initial_num;
    vector<int> v1;
    for (int i : board.north) {
       v1.push_back(i);
    }
    this->north = v1;
    vector<int> v2;
    for (int i : board.south) {
        v2.push_back(i);
    }
    this->south = v2;
}
