#ifndef TYPES_H
#define TYPES_H

#include <cassert>
#include <string>


enum class Castling {
  K,
  k,
  Q,
  q,
  LAST
};

struct Square {
  char letter{'\0'};
  char number{'\0'};

  static Square InvalidSquare;
  static bool isValid(const std::string& field) {
    return field.size() == 2u && field[0] >= 'a' && field[0] <= 'h' &&
           field[1] >= '0' && field[1] <= '8';
  }

  Square(size_t line, size_t row) {
    letter = line + 'a';
    number = row + '1';
  }

  Square(const std::string& square) {
    assert(square.length() == 2);
    letter = square[0];
    number = square[1];
  }

  bool operator==(const Square& other) const {
    return letter == other.letter && number == other.number;
  }

  operator bool() const {
    return letter != InvalidSquare.letter ||
           number != InvalidSquare.number;
  }
};

struct Move {
  const Square oldSquare;
  const Square newSquare;
  char castling;
};

#endif  // TYPES_H
