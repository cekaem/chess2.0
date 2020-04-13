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

  Board();
  Board(const std::string& fen);

  Board makeMove(const Move& move) const;
  const std::string& createFEN() const;
  bool isPositionValid() const;
  std::vector<Move> calculcateAllMoves() const;

 private:
  void getFiguresFromFEN(std::string partial_fen);
  void setFiguresForOneLineFromFen(const std::string& one_line, size_t line);
  void getMiscDataFromFEN(std::string partial_fen);
  void setCastlingsFromFen(const std::string& castlings);

  std::array<std::array<char, kBoardSize>, kBoardSize> fields_;
  char castlings_ = 0x0;
  Square en_passant_target_square_;
  bool white_to_move_{true};
  short halfmove_clock_{0};
  short fullmove_number_{0};
};

#endif  // BOARD_H
