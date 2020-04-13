#include "Board.h"


Board::Board() {
  std::array<Figure, kBoardSize> row;
  row.fill(' ');
  fields_.fill(row);
}

Board::Board(const std::string& fen) {
  FENParser parser(fen);
  if(parser.parse() == false) {
    throw WrongFENExpcetion(fen);
  }
  std::array<Figure, kBoardSize> row;
  row.fill(' ');
  fields_.fill(row);

  std::array<Figure, kBoardSize> row;
  row.fill(Figure::None);
  fields_.fill(row);
  std::string::size_type space_position = fen.find(' ');
  std::string fields = fen.substr(0, space_position);
  std::string partial_fen = fen.substr(space_position);
  getFiguresFromFEN(fields);
  getMiscDataFromFEN(partial_fen);
}

void Board::getFiguresFromFEN(std::string partial_fen) {
  for (size_t i = kBoardSize - 1; i >= 0; --i) {
    std::string one_line;
    if (i > 0) {
      std::string::size_type slash_position = partial_fen.find('/');
      if (slash_position == std::string::npos) {
        throw WrongFENExpcetion(partial_fen);
      }
      one_line = partial_fen.substr(0, slash_position);
      partial_fen = partial_fen.substr(slash_position + 1);
    } else {
      partial_fen = one_line;
    }
    setFiguresForOneLineFromFen(partial_fen, i);
  }
}

void Board::setFiguresForOneLineFromFen(const std::string& one_line, size_t line) {
  size_t current_file = 0;
  for (const char c: one_line) {
    if (current_file >= kBoardSize) {
      throw WrongFENExpcetion(one_line);
    }
    if (c >= '1' && c <= '8') {
      current_file += c - '0';
    } else {
      if (c != 'P' && c != 'p' && c != 'N' && c != 'n' && c != 'B' && c != 'b' &&
          c != 'R' && c != 'r' && c != 'Q' && c != 'q' && c != 'K' && c != 'k') {
        throw WrongFENExpcetion(one_line);
      }
      fields_[current_file][line] = c;
      ++current_file;
    }
  }
}

void Board::setCastlingsFromFen(const std::string& partial_fen) {
  if (partial_fen.empty() == true || partial_fen.size() > 4) {
    throw WrongFENExpcetion(partial_fen);
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
        if (fen.size() != 1) {
          throw WrongFENExpcetion(partial_fen);
        } 
        break;
      default:
        throw WrongFENExpcetion(partial_fen);
    }
  }
}

void Board::getMiscDataFromFEN(partial_fen) {
  const char side_to_move = partial_fen[1];
  switch (side_to_move) {
    case 'w':
      white_to_move_ = true;
      break;
    case 'b':
      side_to_move_ = false;
      break;
    default:
      throw WrongFENExpcetion(partial_fen);
  }

  partial_fen = partial_fen.substr(3);
  std::string::size_type space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENExpcetion(partial_fen);
  }
  std::string castlings = partial_fen.substr(0, space_position);
  setCastlingsFromFen(castlings);
  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 6 || partial_fen[0] != ' ') {  // 6 == length(" - 0 0")
    throw WrongFENExpcetion(partial_fen);
  }
  partial_fen = partial_fen.substr(1);
  space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENExpcetion(partial_fen);
  }

  std::string en_passant_square = partial_fen.substr(0, space_position);
  if (en_passant_square != '-') {
    if (Square::IsValid(en_passant_square) == false) {
      throw WrongFENExpcetion(partial_fen);
    }
    en_passant_target_square_ = Square({en_passant_square[0], en_passant_square[1]});
  }

  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 4 || partial_fen[0] != ' ') {  // 4 == length(" 0 0")
    throw WrongFENExpcetion(partial_fen);
  }
  partial_fen = partial_fen.substr(1);
  space_position = partial_fen.find(' ');
  if (space_position == std::string::npos) {
    throw WrongFENExpcetion(partial_fen);
  }
  std::string halfmove_clock_str = partial_fen.substr(0, space_position);
  if (utils::str_2_uint(halfmove_clock_str, halfmove_clock_) == false) {
    throw WrongFENExpcetion(partial_fen);
  }
  halfmove_clock_ *= 2;
  partial_fen = partial_fen.substr(space_position);
  if (partial_fen.size() < 2 || partial_fen[0] != ' ') {  // 2 == length(" 0")
    throw WrongFENExpcetion(partial_fen);
  }
  std::string fullmove_number_str = partial_fen.substr(1);
  if (utils::str_2_uint(fullmove_number_str, fullmove_number_) == false) {
    throw WrongFENExpcetion(partial_fen);
  }
}
