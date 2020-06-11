//
//  Board.h
//  Project3
//
//  Created by Calvin Wang on 5/12/19.
//  Copyright © 2019 Calvin Wang. All rights reserved.
//

#ifndef Board_h
#define Board_h
#include "Side.h"
#include <vector>
using std::vector;

class Board {
public:
    Board(int nHoles, int nInitialBeansPerHole);
    Board(const Board& board);
    int holes() const;
    int beans(Side s, int hole) const;
    int beansInPlay(Side s) const;
    int totalBeans() const;
    bool sow(Side s, int hole, Side& endSide, int& endHole);
    bool moveToPot(Side s, int hole, Side potOwner);
    bool setBeans(Side s, int hole, int beans);
    void _print_board() const;





private:
    //helper functions:
    int _beans_helper(vector<int> v) const;
    bool _next_hole(int& hole, bool at_v1) const;
    bool _sow_helper(vector<int>& v1, vector<int>& v2, int hole, int num_beans, int& endHole);
    void _copy(const Board& board);
    //private attributes:
    vector<int> north;
    vector<int> south;
    int num_holes;
    int num_total;
    int initial_num;

    //helper functions for testing:





};

#endif /* Board_h */
