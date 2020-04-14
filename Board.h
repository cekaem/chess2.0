#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <vector>

#include "Types.h"


class Board {
 public:
  static constexpr size_t kBoardSize = 8;

  struct WrongFENException {
    WrongFENException(const std::string& f) : fen(f) {}
    const std::string fen;
  };

  struct WrongSquareException {
    WrongSquareException(const std::string& s) : square(s) {}
    const std::string square;
  };

  Board(const std::string& fen);
  Board(const Board& orig, Move move);

  bool isPositionValid() const;
  std::vector<Move> calculcateAllMoves() const;
  inline char getSquare(size_t line, size_t row) const;
  char getSquare(const std::string& square) const;

 private:
  void setFiguresFromFEN(std::string partial_fen);
  void setFiguresForOneLineFromFen(const std::string& one_line, size_t line);
  void setMiscDataFromFEN(std::string partial_fen);
  void setCastlingsFromFen(const std::string& castlings);

  std::array<std::array<char, kBoardSize>, kBoardSize> squares_;
  char castlings_ = 0x0;
  Square en_passant_target_square_;
  bool white_to_move_{true};
  unsigned halfmove_clock_{0};
  unsigned fullmove_number_{0};
};

#endif  // BOARD_H
