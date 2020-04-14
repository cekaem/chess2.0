#include "Board.h"

#include "utils/Utils.h"


Board::Board(const std::string& fen) {
  std::array<char, kBoardSize> row;
  row.fill(' ');
  squares_.fill(row);

  std::string::size_type space_position = fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENException(fen);
  }
  std::string fields = fen.substr(0, space_position);
  std::string partial_fen = fen.substr(space_position);
  setFiguresFromFEN(fields);
  setMiscDataFromFEN(partial_fen);
}

Board::Board(const Board& orig, Move move) {
  // TODO
}

void Board::setFiguresFromFEN(std::string partial_fen) {
  for (int i = kBoardSize - 1; i >= 0; --i) {
    std::string one_line;
    if (i > 0) {
      std::string::size_type slash_position = partial_fen.find('/');
      if (slash_position == std::string::npos) {
        throw WrongFENException(partial_fen);
      }
      one_line = partial_fen.substr(0, slash_position);
      partial_fen = partial_fen.substr(slash_position + 1);
    } else {
      one_line = partial_fen;
    }
    setFiguresForOneLineFromFen(one_line, i);
  }
}

void Board::setFiguresForOneLineFromFen(const std::string& one_line, size_t line) {
  size_t current_file = 0;
  for (const char c: one_line) {
    if (current_file >= kBoardSize) {
      throw WrongFENException(one_line);
    }
    if (c >= '1' && c <= '8') {
      current_file += c - '0';
    } else {
      if (c != 'P' && c != 'p' && c != 'N' && c != 'n' && c != 'B' && c != 'b' &&
          c != 'R' && c != 'r' && c != 'Q' && c != 'q' && c != 'K' && c != 'k') {
        throw WrongFENException(one_line);
      }
      squares_[current_file][line] = c;
      ++current_file;
    }
  }
}

void Board::setCastlingsFromFen(const std::string& partial_fen) {
  if (partial_fen.empty() == true || partial_fen.size() > 4) {
    throw WrongFENException(partial_fen);
  }
  for (const char c: partial_fen) {
    switch (c) {
      case 'K':
        castlings_ |= (1 << static_cast<size_t>(Castling::K));
        break;
      case 'k':
        castlings_ |= (1 << static_cast<size_t>(Castling::k));
        break;
      case 'Q':
        castlings_ |= (1 << static_cast<size_t>(Castling::Q));
        break;
      case 'q':
        castlings_ |= (1 << static_cast<size_t>(Castling::q));
        break;
      case '-': 
        if (partial_fen.size() != 1) {
          throw WrongFENException(partial_fen);
        } 
        break;
      default:
        throw WrongFENException(partial_fen);
    }
  }
}

void Board::setMiscDataFromFEN(std::string partial_fen) {
  const char side_to_move = partial_fen[1];
  switch (side_to_move) {
    case 'w':
      white_to_move_ = true;
      break;
    case 'b':
      white_to_move_ = false;
      break;
    default:
      throw WrongFENException(partial_fen);
  }

  partial_fen = partial_fen.substr(3);
  std::string::size_type space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENException(partial_fen);
  }
  std::string castlings = partial_fen.substr(0, space_position);
  setCastlingsFromFen(castlings);
  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 6 || partial_fen[0] != ' ') {  // 6 == length(" - 0 0")
    throw WrongFENException(partial_fen);
  }
  partial_fen = partial_fen.substr(1);
  space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENException(partial_fen);
  }

  std::string en_passant_square = partial_fen.substr(0, space_position);
  if (en_passant_square.size() == 1) {
    if (en_passant_square[0] != '-') {
      throw WrongFENException(partial_fen);
    }
  } else {
    if (Square::isValid(en_passant_square) == false) {
      throw WrongFENException(partial_fen);
    }
    en_passant_target_square_ = {en_passant_square[0], en_passant_square[1]};
  }

  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 4 || partial_fen[0] != ' ') {  // 4 == length(" 0 0")
    throw WrongFENException(partial_fen);
  }
  partial_fen = partial_fen.substr(1);
  space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENException(partial_fen);
  }
  std::string halfmove_clock_str = partial_fen.substr(0, space_position);
  if (utils::str_2_uint(halfmove_clock_str, halfmove_clock_) == false) {
    throw WrongFENException(partial_fen);
  }
  halfmove_clock_ *= 2;
  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 2 || partial_fen[0] != ' ') {  // 2 == length(" 0")
    throw WrongFENException(partial_fen);
  }
  std::string fullmove_number_str = partial_fen.substr(1);
  if (utils::str_2_uint(fullmove_number_str, fullmove_number_) == false) {
    throw WrongFENException(partial_fen);
  }
}

char Board::getSquare(size_t line, size_t row) const {
  return squares_[line][row];
}

char Board::getSquare(const std::string& square) const {
  if (square.size() != 2) {
    throw WrongSquareException(square);
  }
  size_t line = square[0] - 'a';
  size_t row = square[1] - '1';
  if (line > 8 || row > 8) {
    throw WrongSquareException(square);
  }
  return squares_[line][row];
}

bool Board::isPositionValid() const {
  return false;
}

std::vector<Move> Board::calculcateAllMoves() const {
  std::vector<Move> moves;
  return moves;
}
