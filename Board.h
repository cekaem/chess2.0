#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>

#include "Types.h"


class Board {
 public:
  static constexpr size_t kBoardSize = 8;

  struct InvalidFENException {
    InvalidFENException(const std::string& f) : fen(f) {}
    const std::string fen;
  };

  struct InvalidSquareException {
    InvalidSquareException(const std::string& s) : square(s) {}
    const std::string square;
  };

  struct IndexHelper {
    char& operator[](size_t index) {
      return (*row)[index];
    }

    const char& operator[](size_t index) const {
      return (*row)[index];
    }

    mutable std::array<char, kBoardSize>* row = nullptr;
  } index_helper_;

  Board(const std::string& fen);

  IndexHelper& operator[](size_t index);
  const IndexHelper& operator[](size_t index) const;
  char getSquare(const std::string& square) const;

  bool canCastle(Castling castling) const;
  std::string createFEN() const;

  bool whiteToMove() const {
    return white_to_move_;
  }

  void changeSideToMove() {
    white_to_move_ = !white_to_move_;
  }

  void incrementNumberOfMoves() {
    ++fullmove_number_;
  }

  void incrementNumberOfHalfMoves() {
    ++halfmove_clock_;
  }

  void resetNumberOfHalfMoves() {
    halfmove_clock_ = 0;
  }

  bool operator==(const std::string& fen) const;

  Square getEnPassantTargetSquare() const {
    return en_passant_target_square_;
  }

  void setEnPassantTargetSquare(Square square) {
    en_passant_target_square_ = square;
  }

  void resetCastling(Castling castling) {
    castlings_ &= ~(1 << static_cast<int>(castling));
  }

  void resetCastlings(bool for_white);

 private:
  void setFiguresFromFEN(std::string partial_fen);
  void setFiguresForOneLineFromFEN(const std::string& one_line, size_t line);
  void setMiscDataFromFEN(std::string partial_fen);
  void setCastlingsFromFEN(const std::string& castlings);
  void writeFiguresToFEN(std::stringstream& fen) const;
  void writeMiscDataToFEN(std::stringstream& fen) const;

  std::array<std::array<char, kBoardSize>, kBoardSize> squares_;
  char castlings_ = 0x0;
  Square en_passant_target_square_{Square::InvalidSquare};
  bool white_to_move_{true};
  unsigned halfmove_clock_{0};
  unsigned fullmove_number_{1};
};

std::ostream& operator<<(std::ostream& ostr, const Board& board);

#endif  // BOARD_H
