#ifndef MOVE_CALCULATOR_H
#define MOVE_CALCULATOR_H

#include <string>
#include <vector>

#include "Board.h"

class MoveCalculator {
 public:
  struct InvalidPositionException {
    InvalidPositionException(const std::string& m) : message(m) {}
    const std::string message;
  };

  MoveCalculator(const Board& board, bool look_for_check = false)
      : board_(board),
        look_for_check_(look_for_check) {
  }

  std::vector<Board> calculateAllMoves();

 private:
  struct KingInCheckException {};

  bool isWhite(size_t line, size_t row);
  bool isValidMove(size_t old_line, size_t old_row,
                   size_t new_line, size_t new_row);
  void calculateMovesForPawn(size_t line, size_t row);
  void calculateMovesForKnight(size_t line, size_t row);
  void calculateMovesForBishop(size_t line, size_t row);
  void calculateMovesForRook(size_t line, size_t row);
  void calculateMovesForQueen(size_t line, size_t row);
  void calculateMovesForKing(size_t line, size_t row);

  bool handlePossibleMove(size_t old_line, size_t old_row,
                          size_t new_line, size_t new_row);
  void handleMove(Board& board, bool is_king_capture);
  void handlePossiblePawnsCapture(size_t line, size_t row);
  void handlePawnPromotion(const Board& board, size_t line, size_t row);

  Board board_;
  const bool look_for_check_{false};
  std::vector<Board> moves_;
};

#endif  // MOVE_CALCULATOR_H
