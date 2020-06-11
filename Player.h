//
//  Player.hpp
//  Project3
//
//  Created by Calvin Wang on 5/12/19.
//  Copyright © 2019 Calvin Wang. All rights reserved.
//

#ifndef Player_h
#define Player_h
#include <string>
#include "Board.h"
#include <vector>
#include <iostream>
using std::endl;
using std::cout;
using std::string;



class Player {
  public:
    Player(std::string name);
    std::string name() const;
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const = 0;
    virtual ~Player();
private:
    string _name;
};

class BadPlayer: public Player {
public:
    BadPlayer(string name);
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const ;
    virtual ~BadPlayer();
};

class HumanPlayer: public Player {
public:
    HumanPlayer(string name);
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~HumanPlayer();
};

class SmartPlayer: public Player {
public:
    SmartPlayer(string name);
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~SmartPlayer();

private:



    class Node {
    public:
      const int positive_infinity = 10000;
      const int negative_infinity = -10000;
      Board* board_;
      bool is_leaf;
      Side side_;
      vector<Node*> children;
      int value;
      int level;
      int move;
      int number;

      Node(Board* board, Side side, int curr_level) {
         board_ = board;
         side_ = side;
         level = curr_level;
         is_leaf = false;
      }

      ~Node() {
          if (is_leaf) {
              delete board_;
              return;
          } else {
              for (Node* n : children) {
                  delete n;
              }
          }
          delete board_;
      }

      bool game_over(const Board* board) const {
          if (board->beansInPlay(SOUTH) == 0 || board->beansInPlay(NORTH) == 0) {
              return true;
          } else if(board->beans(SOUTH, 0) > board->totalBeans()/2 || board->beans(NORTH, 0) > board->totalBeans()/2) {
              return true;
          } else {
              return false;
          }
      }

      bool _hasWinner(Board* b, Side& winner) {
          if (b->beansInPlay(SOUTH) + b->beans(SOUTH, 0) == b->beansInPlay(NORTH) + b->beans(NORTH, 0)) {
              return false;
          } else if (b->beansInPlay(SOUTH) + b->beans(SOUTH, 0) > b->beansInPlay(NORTH) + b->beans(NORTH, 0)) {
              winner = SOUTH;
          } else {
              winner = NORTH;
          }
          return true;
      }

      Board* _move(const Board* board, Side side, int hole, bool& another_turn) {
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

      void set_value_for_leaf(Node* leaf_node) {
          //first case: the game is over
          if (game_over(leaf_node->board_)) {
              Side winner = SOUTH;
              if (!_hasWinner(leaf_node->board_, winner)) {
                  leaf_node->value = 0;
                  return;
              } else if (winner == SOUTH) {
                  leaf_node->value = positive_infinity;
                  return;
              } else {
                  leaf_node->value = negative_infinity;
                  return;
              }
          }
          //second case: the normal case, simply calculate the difference
          leaf_node->value = leaf_node->board_->beans(SOUTH, 0) - leaf_node->board_->beans(NORTH, 0);
      }

      bool _constructor_helper(Node* curr, int maximum_level, int& sec_level_minimum) {

          number++;
          //base case
          if (curr->level > maximum_level || game_over(curr->board_)) {
              set_value_for_leaf(curr);
              curr->is_leaf = true;
              return true;
          }

          //go down further to construct;
          for (int i = 1; i <= curr->board_->holes(); i++) {

              if (curr->board_->beans(curr->side_, i) > 0) {

                  bool another_turn = true;
                  Board* new_board = _move(curr->board_, curr->side_, i, another_turn);
                  Side new_side = opponent(curr->side_);
                  if (!another_turn) {
                      //this is the normal case where change side
                      Node* new_node = new Node(new_board, new_side, curr->level + 1);
                      new_node->move = i;
                      _constructor_helper(new_node, maximum_level, sec_level_minimum);
                      curr->children.push_back(new_node);
                  } else {
                      vector<Board*> boards;
                      recurse_helper(new_board, boards, curr->side_);
                      if (boards.empty()) {
                          //cout<<"YOU BELONG WITH ME !!!!!!"<<endl;
                          return true;
                      }
                      for (Board* b : boards) {
                          Node* new_node = new Node(b, new_side, curr->level + 1);
                          new_node->move = i;
                          _constructor_helper(new_node, maximum_level, sec_level_minimum);
                          curr->children.push_back(new_node);
                      }
                  }
              }
          }
          //after recursion, set the value of this Node

          evaluate(curr);
          return true;
      }
      //memory leak!!!!

      void recurse_helper(Board* curr_board, vector<Board*>& v, Side s) {
          bool another_turn = false;
          for (int i = 1; i <= curr_board->holes(); i++) {
              if (curr_board->beans(s, i) > 0) {
                  Board* new_board = _move(curr_board, s, i, another_turn);

                  if (another_turn) {
                      recurse_helper(new_board, v, s);
                      delete new_board;
                  } else {
                      v.push_back(new_board);
                  }
              }
          }
      }

      void evaluate(Node* node) {
          int initial = 0;
          if (node->side_ == NORTH) {
              initial = positive_infinity;
              for (Node* curr: node->children) {
                  if (curr->value < initial) {
                      initial = curr->value;
                  }
              }
          } else if (node->side_ == SOUTH){
              initial = negative_infinity;
              for (Node* curr: node->children) {
                  if (curr->value > initial) {
                      initial = curr->value;
                  }
              }
          }
          node->value = initial;
      }
  };
};




class LessSmartPlayer: public Player {
public:
    LessSmartPlayer(string name);
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~LessSmartPlayer();
private:
    const int positive_infinity = 10000;
    const int negative_infinity = -10000;
    void chooseMove_helper(Side side, Board* board, int& bestHole, int& value, int level, int maximum_level) const;
    int evaluate(Board* board) const;
    void recurse_helper(Board* curr_board, vector<Board*>& v, Side s) const;
    Board* _move(const Board* board, Side side, int hole, bool& another_turn) const;
    bool game_over(Board* board) const;



};


#endif /* Player_hpp */
