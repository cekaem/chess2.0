#ifndef TYPES_H
#define TYPES_H

#include <string>

/*
enum class Figure {
  None,
  Pawn,
  Knight,
  Bishop,
  Rook,
  Queen,
  King
};
*/

enum class Castling {
  K,
  k,
  Q,
  q,
  LAST
};

struct Square {
  const char letter{'\0'};
  const char number{'\0'};

  static bool isValid(const std::string& field) {
    return field.size() == 2u && field[0] >= 'a' && field[0] <= 'h' &&
           field[1] >= '0' && field[1] <= '8';
  }
};

struct Move {
  const Square oldSquare;
  const Square newSquare;
  // TODO: implement all required fields (castling, en-passant, ...)
};

#endif  // TYPES_H
