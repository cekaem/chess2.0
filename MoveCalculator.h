#ifndef MOVE_CALCULATOR_H
#define MOVE_CALCULATOR_H

#include <ostream>
#include <string>
#include <vector>

#include "Board.h"

struct Move {
  Move(const Board& b,
       size_t old_line,
       size_t old_row,
       size_t new_line,
       size_t new_row)
    : board(b),
      old_square{old_line, old_row},
      new_square{new_line, new_row} {
  }

  Board board;
  const Square old_square;
  const Square new_square;
  Castling castling{Castling::LAST};
  bool capture{false};
  char promotion{0x0};
  bool check{false};
  bool mate{false};
  bool stalemate{false};
  bool insufficient_material{false};
  int evaluation{0};  // this field is filled by Engine
  int moves_to_mate{0};  // this field is filled by Engine
};

std::ostream& operator<<(std::ostream& ostr, const Move& move);

class MoveCalculator {
 public:
  struct InvalidPositionException {
    InvalidPositionException(const std::string& m) : message(m) {}
    const std::string message;
  };

  MoveCalculator(const Board& board, bool look_for_king_capture = false)
      : board_(board),
        look_for_king_capture_(look_for_king_capture) {
  }

  std::vector<Move> calculateAllMoves();

 private:
  struct KingInCheckException {};

  bool isValidMove(size_t old_line, size_t old_row,
                   size_t new_line, size_t new_row);
  void calculateAllMovesInternal();
  void calculateAllMovesForFigure(size_t line, size_t row);
  void calculateMovesForPawn(size_t line, size_t row);
  void calculateMovesForKnight(size_t line, size_t row);
  void calculateMovesForBishop(size_t line, size_t row);
  void calculateMovesForRook(size_t line, size_t row);
  void calculateMovesForQueen(size_t line, size_t row);
  void calculateMovesForKing(size_t line, size_t row);

  bool handlePossibleMove(size_t old_line, size_t old_row,
                          size_t new_line, size_t new_row);
  void handleMove(Move& board, bool is_king_capture);
  void handlePossiblePawnsCapture(size_t line, size_t row);
  void handlePawnPromotion(const Move& move, size_t line, size_t row);
  void handlePossibleCastlings(size_t line, size_t row);
  void resetCastlings(Board& board, char figure, size_t line, size_t row) const;
  void updateCheckAndCheckMateForAllMoves();
  void updateCheckAndCheckMateForMove(Move& move) const;
  void updateInsufficientMaterialForMove(Move& move) const;

  const Board& board_;
  const bool look_for_king_capture_{false};
  std::vector<Move> moves_;
};

#endif  // MOVE_CALCULATOR_H
