#ifndef MOVE_CALCULATOR_H
#define MOVE_CALCULATOR_H

#include <vector>

#include "Board.h"

class MoveCalculator {
= public:
  std::vector<Board> calculateAllMoves(const Board& board);
};

#endif  // MOVE_CALCULATOR_H
