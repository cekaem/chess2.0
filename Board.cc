#include "Board.h"

#include "utils/Utils.h"

Square Square::InvalidSquare = {0, 0};

std::ostream& operator<<(std::ostream& ostr, const Board& move) {
  ostr << move.createFEN();
  return ostr;
}

Board::Board(const std::string& fen) {
  std::array<char, kBoardSize> row;
  row.fill(0x0);
  squares_.fill(row);

  std::string::size_type space_position = fen.find(' ');
  if (space_position == std::string::npos) {
    throw InvalidFENException(fen);
  }
  std::string fields = fen.substr(0, space_position);
  std::string partial_fen = fen.substr(space_position);
  setFiguresFromFEN(fields);
  setMiscDataFromFEN(partial_fen);
}

void Board::setFiguresFromFEN(std::string partial_fen) {
  for (int i = kBoardSize - 1; i >= 0; --i) {
    std::string one_line;
    if (i > 0) {
      std::string::size_type slash_position = partial_fen.find('/');
      if (slash_position == std::string::npos) {
        throw InvalidFENException(partial_fen);
      }
      one_line = partial_fen.substr(0, slash_position);
      partial_fen = partial_fen.substr(slash_position + 1);
    } else {
      one_line = partial_fen;
    }
    setFiguresForOneLineFromFEN(one_line, i);
  }
}

void Board::setFiguresForOneLineFromFEN(const std::string& one_line, size_t line) {
  size_t current_file = 0;
  for (const char c: one_line) {
    if (current_file >= kBoardSize) {
      throw InvalidFENException(one_line);
    }
    if (c >= '1' && c <= '8') {
      current_file += c - '0';
    } else {
      if (c != 'P' && c != 'p' && c != 'N' && c != 'n' && c != 'B' && c != 'b' &&
          c != 'R' && c != 'r' && c != 'Q' && c != 'q' && c != 'K' && c != 'k') {
        throw InvalidFENException(one_line);
      }
      squares_[current_file][line] = c;
      ++current_file;
    }
  }
}

void Board::setCastlingsFromFEN(const std::string& partial_fen) {
  if (partial_fen.empty() == true || partial_fen.size() > 4) {
    throw InvalidFENException(partial_fen);
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
          throw InvalidFENException(partial_fen);
        } 
        break;
      default:
        throw InvalidFENException(partial_fen);
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
      throw InvalidFENException(partial_fen);
  }

  partial_fen = partial_fen.substr(3);
  std::string::size_type space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw InvalidFENException(partial_fen);
  }
  std::string castlings = partial_fen.substr(0, space_position);
  setCastlingsFromFEN(castlings);
  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 6 || partial_fen[0] != ' ') {  // 6 == length(" - 0 0")
    throw InvalidFENException(partial_fen);
  }
  partial_fen = partial_fen.substr(1);
  space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw InvalidFENException(partial_fen);
  }

  std::string en_passant_square = partial_fen.substr(0, space_position);
  if (en_passant_square.size() == 1) {
    if (en_passant_square[0] != '-') {
      throw InvalidFENException(partial_fen);
    }
  } else {
    if (Square::isValid(en_passant_square) == false) {
      throw InvalidFENException(partial_fen);
    }
    en_passant_target_square_ = en_passant_square;
  }

  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 4 || partial_fen[0] != ' ') {  // 4 == length(" 0 0")
    throw InvalidFENException(partial_fen);
  }
  partial_fen = partial_fen.substr(1);
  space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw InvalidFENException(partial_fen);
  }
  std::string halfmove_clock_str = partial_fen.substr(0, space_position);
  if (utils::str_2_uint(halfmove_clock_str, halfmove_clock_) == false) {
    throw InvalidFENException(partial_fen);
  }
  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 2 || partial_fen[0] != ' ') {  // 2 == length(" 0")
    throw InvalidFENException(partial_fen);
  }
  std::string fullmove_number_str = partial_fen.substr(1);
  if (utils::str_2_uint(fullmove_number_str, fullmove_number_) == false) {
    throw InvalidFENException(partial_fen);
  }
}

std::string Board::createFEN() const {
  std::stringstream fen;
  writeFiguresToFEN(fen);
  writeMiscDataToFEN(fen);
  return fen.str();
}

void Board::writeFiguresToFEN(std::stringstream& fen) const {
  for (int row = kBoardSize - 1; row >= 0; --row) {
    int empty_lines = 0;
    for (size_t line = 0; line < kBoardSize; ++line) {
      if (squares_[line][row] == 0x0) {
        ++empty_lines;
      } else {
        if (empty_lines > 0) {
          fen << empty_lines;
          empty_lines = 0;
        }
        fen << squares_[line][row];
      }
    }
    if (empty_lines > 0) {
      fen << empty_lines;
    }
    if (row > 0) {
      fen << "/";
    }
  }
}

void Board::writeMiscDataToFEN(std::stringstream& fen) const {
  if (white_to_move_) {
    fen << " w ";
  } else {
    fen << " b ";
  }
  if (castlings_ == 0) {
    fen << "-";
  } else {
    if (canCastle(Castling::K)) {
      fen << "K";
    }
    if (canCastle(Castling::Q)) {
      fen << "Q";
    }
    if (canCastle(Castling::k)) {
      fen << "k";
    }
    if (canCastle(Castling::q)) {
      fen << "q";
    }
  }
  if (en_passant_target_square_ == Square::InvalidSquare) {
    fen << " - ";
  } else {
    fen << " " << en_passant_target_square_.letter << en_passant_target_square_.number << " ";
  }
  fen << halfmove_clock_ << " " << fullmove_number_;
}

bool Board::operator==(const std::string& fen) const {
  Board second(fen);
  return squares_ == second.squares_ &&
         en_passant_target_square_ == second.en_passant_target_square_ &&
         halfmove_clock_ == second.halfmove_clock_ &&
         fullmove_number_ == second.fullmove_number_ &&
         castlings_ == second.castlings_ &&
         white_to_move_ == second.white_to_move_;
}

Board::IndexHelper& Board::operator[](size_t index) {
  index_helper_.row = &squares_[index];
  return index_helper_;
}

const Board::IndexHelper& Board::operator[](size_t index) const {
  index_helper_.row = const_cast<std::array<char, kBoardSize>*>(
      &squares_[index]);
  return index_helper_;
}

char Board::getSquare(const std::string& square) const {
  if (square.size() != 2) {
    throw InvalidSquareException(square);
  }
  size_t line = square[0] - 'a';
  size_t row = square[1] - '1';
  if (line >= 8 || row >= 8) {
    throw InvalidSquareException(square);
  }
  return squares_[line][row];
}

void Board::resetCastlings(bool for_white) {
  if (for_white) {
    resetCastling(Castling::Q);
    resetCastling(Castling::K);
  } else {
    resetCastling(Castling::q);
    resetCastling(Castling::k);
  }
}

bool Board::canCastle(Castling castling) const {
  return (castlings_ & (1 << static_cast<size_t>(castling))) != 0 ;
}
