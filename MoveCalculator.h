#ifndef MOVE_CALCULATOR_H
#define MOVE_CALCULATOR_H

#include <vector>

#include "Board.h"

class MoveCalculator {
 public:
  MoveCalculator(const Board& board, bool look_for_check = false)
      : board_(board),
        look_for_check_(look_for_check) {
  }

  std::vector<Board> calculateAllMoves();

 private:
  struct KingInCheckException {};

  bool isWhite(char figure) const;
  void calculateMovesForPawn(size_t row, size_t line);
  void calculateMovesForKnight(size_t row, size_t line);
  void calculateMovesForBishop(size_t row, size_t line);
  void calculateMovesForRook(size_t row, size_t line);
  void calculateMovesForQueen(size_t row, size_t line);
  void calculateMovesForKing(size_t row, size_t line);
  void handleMove(size_t old_row, size_t old_line,
                  size_t new_row, size_t new_line);


  Board board_;
  const bool look_for_check_{false};
  std::vector<Board> moves_;
};

#endif  // MOVE_CALCULATOR_H
