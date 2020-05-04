#include "MoveCalculator.h"


namespace {

bool isSquareOnBoard(int line, int row) {
  return line >= 0 && line < static_cast<int>(Board::kBoardSize) &&
         row >= 0 && row < static_cast<int>(Board::kBoardSize);
}

bool isKing(const char figure) {
  return figure == 'k' || figure == 'K';
}

bool isFinalRank(size_t row) {
  return row == 0 || row == Board::kBoardSize - 1;
}

bool isWhite(char figure) {
  return figure == 'P' || figure == 'K' || figure == 'N' ||
         figure == 'B' || figure == 'R' || figure == 'Q';
}

bool isStartingRow(char figure, size_t row) {
  return (figure == 'P' && row == 1) ||
         (figure == 'p' && row == Board::kBoardSize - 2);
}

constexpr size_t kKingStartingLine = 4lu;
constexpr size_t kQueenSideRookStartingLine = 0lu;
constexpr size_t kKingSideRookStartingLine = Board::kBoardSize - 1;

}  // unnamed namespace


std::ostream& operator<<(std::ostream& ostr, const Move& move) {
  switch (move.castling) {
    case Castling::K:
    case Castling::k:
      ostr << "0-0";
      break;
    case Castling::Q:
    case Castling::q:
      ostr << "0-0-0";
      break;
    case Castling::LAST:
      ostr << move.old_square.letter << move.old_square.number;
      if (move.capture) {
        ostr << "x";
      } else {
        ostr << "-";
      }
      ostr << move.new_square.letter << move.new_square.number;
      if (move.promotion) {
        ostr << move.promotion;
      }
      break;
  }
  return ostr;
}

void MoveCalculator::calculateAllMovesForFigure(size_t line, size_t row) {
  switch (board_.at(line, row)) {
    case 'p':
    case 'P':
      calculateMovesForPawn(line, row);
      break;
    case 'n':
    case 'N':
      calculateMovesForKnight(line, row);
      break;
    case 'b':
    case 'B':
      calculateMovesForBishop(line, row);
      break;
    case 'r':
    case 'R':
      calculateMovesForRook(line, row);
      break;
    case 'q':
    case 'Q':
      calculateMovesForQueen(line, row);
      break;
    case 'k':
    case 'K':
      calculateMovesForKing(line, row);
      break;
  }
}

std::vector<Move> MoveCalculator::calculateAllMoves() {
  for (size_t line = 0; line < Board::kBoardSize; ++line) {
    for (size_t row = 0; row < Board::kBoardSize; ++row) {
      char square = board_.at(line, row);
      if (board_.at(line, row) && board_.whiteToMove() == isWhite(square)) {
        calculateAllMovesForFigure(line, row);
      }
    }
  }
  return moves_;
}

bool MoveCalculator::isValidMove(size_t old_line, size_t old_row,
                                 size_t new_line, size_t new_row) {
  if (board_.at(new_line, new_row) == 0x0) {
    return true;
  }
  return isWhite(board_.at(old_line, old_row)) != isWhite(board_.at(new_line, new_row));
}

void MoveCalculator::resetCastlings(Board& board, char figure, size_t line, size_t row) const {
  switch (figure) {
    case 'K':
      board.resetCastlings(true);
      break;
    case 'k':
      board.resetCastlings(false);
    case 'R':
      if (row == 0lu && line == 0lu) {
        board.resetCastling(Castling::Q);
      } else if (row == 0lu && line == Board::kBoardSize - 1) {
        board.resetCastling(Castling::K);
      }
      break;
    case 'r':
      if (row == Board::kBoardSize - 1 && line == 0lu) {
        board.resetCastling(Castling::q);
      } else if (row == Board::kBoardSize - 1 &&
                 line == Board::kBoardSize - 1) {
        board.resetCastling(Castling::k);
      }
      break;
    default:
      break;
  }
}

bool MoveCalculator::handlePossibleMove(size_t old_line, size_t old_row,
                                        size_t new_line, size_t new_row) {
  if (isSquareOnBoard(new_line, new_row) == false ||
      isValidMove(old_line, old_row, new_line, new_row) == false) {
    return false;
  }
  Move move(board_, old_line, old_row, new_line, new_row);
  move.capture = board_.at(new_line, new_row) != 0x0;
  char new_square = board_.at(new_line, new_row);
  if (new_square) {
    move.board.resetNumberOfHalfMoves();
  } else {
    move.board.incrementNumberOfHalfMoves();
  }
  move.board.setEnPassantTargetSquare(Square::InvalidSquare);
  move.board.at(new_line, new_row) = move.board.at(old_line, old_row);
  move.board.at(old_line, old_row) = 0x0;
  resetCastlings(move.board, move.board.at(new_line, new_row), old_line, old_row);
  handleMove(move, isKing(new_square));
  return board_.at(new_line, new_row) == 0x0;
}

void MoveCalculator::handleMove(Move& move, bool is_king_capture) {
  if (look_for_king_capture_) {
    if (is_king_capture) {
      throw KingInCheckException();
    }
    return;
  }
  if (is_king_capture) {
    throw InvalidPositionException("Moving side checks the opponent's king");
  }

  move.board.changeSideToMove();
  try {
    MoveCalculator calculator(move.board, true);
    calculator.calculateAllMoves();
    // Exception KingInCheckException was not thrown so move is valid.
    if (move.board.whiteToMove()) {
      move.board.incrementNumberOfMoves();
    }
    updateInsufficientMaterialForMove(move);
    moves_.push_back(move);
  } catch (KingInCheckException& e) {
  }
}

bool MoveCalculator::isCheck() const {
  Board copy = board_;
  copy.changeSideToMove();
  MoveCalculator calculator(copy, true);
  bool is_check = false;
  try {
    calculator.calculateAllMoves();
  } catch (KingInCheckException&) {
    is_check = true;
  }
  return is_check;
}

void MoveCalculator::updateInsufficientMaterialForMove(Move& move) const {
  const Board& board = move.board;
  bool insufficient_material = true;
  bool white_has_bishop = false;
  bool black_has_bishop = false;
  bool white_has_knight = false;
  bool black_has_knight = false;
  for (size_t line = 0; line < Board::kBoardSize; ++line) {
    for (size_t row = 0; row < Board::kBoardSize; ++row) {
      char figure = board.at(line, row);
      if (figure == 0x0) {
        continue;
      }
      switch (figure) {
        case 'Q':
        case 'q':
        case 'R':
        case 'r':
        case 'P':
        case 'p':
          insufficient_material = false;
          break;
        case 'B':
          if (white_has_bishop || white_has_knight) {
            insufficient_material = false;
          } else {
            white_has_bishop = true;
          }
          break;
        case 'b':
          if (black_has_bishop || black_has_knight) {
            insufficient_material = false;
          } else {
            black_has_bishop = true;
          }
          break;
        case 'N':
          if (white_has_bishop) {
            insufficient_material = false;
          } else {
            white_has_knight = true;
          }
          break;
        case 'n':
          if (black_has_bishop) {
            insufficient_material = false;
          } else {
            black_has_knight = true;
          }
          break;
        default:
          BoardAssert(board, figure == 'K' || figure == 'k');
          break;
      }
      if (insufficient_material == false) {
        break;
      }
    }
    if (insufficient_material == false) {
      break;
    }
  }
  move.insufficient_material = insufficient_material;
}

void MoveCalculator::calculateMovesForPawn(size_t line, size_t row) {
  BoardAssert(board_, board_.at(line, row) == 'p' || board_.at(line, row) == 'P');
  handlePossiblePawnsCapture(line, row);
  if (look_for_king_capture_) {
    // Pawn move cannot beat king.
    return;
  }
  bool is_white = isWhite(board_.at(line, row));
  int forward = is_white ? 1 : -1;
  size_t forward_row = row + forward;
  if (isSquareOnBoard(line, forward_row) == false) {
    throw InvalidPositionException("calculateMovesForPawn: pawn on the first/last row");
  }

  auto helper = [this](size_t line, size_t old_row, size_t new_row, Square en_passant) {
    Move move(board_, line, old_row, line, new_row);
    move.board.at(line, old_row) = 0x0;
    move.board.at(line, new_row) = board_.at(line, old_row);
    move.board.resetNumberOfHalfMoves();
    move.board.setEnPassantTargetSquare(en_passant);
    if (isFinalRank(new_row)) {
      handlePawnPromotion(move, line, new_row);
    } else {
      handleMove(move, false);
    }
  };

  char examined_square = board_.at(line, forward_row);
  if (examined_square == 0x0) {
    helper(line, row, forward_row, Square::InvalidSquare);
    if (isStartingRow(board_.at(line, row), row)) {
      forward_row += forward;
      examined_square = board_.at(line, forward_row);
      if (examined_square == 0x0) {
        Square en_passant(line, row + forward);
        helper(line, row, forward_row, en_passant);
      }
    }
  }
}

void MoveCalculator::handlePossiblePawnsCapture(size_t line, size_t row) {
  BoardAssert(board_, board_.at(line, row) == 'p' || board_.at(line, row) == 'P');
  auto helper = [this, line, row](int shift) {
    bool is_white = isWhite(board_.at(line, row));
    size_t forward_row = is_white ? row + 1 : row - 1;
    if (isSquareOnBoard(line + shift, forward_row)) {
      Square en_passant_square = board_.getEnPassantTargetSquare();
      bool is_en_passant_capture = Square(line + shift, forward_row) == en_passant_square;
      if (is_en_passant_capture ||
          (board_.at(line + shift, forward_row) &&
           isWhite(board_.at(line + shift, forward_row)) != is_white)) {
        Move move(board_, line, row, line + shift, forward_row);
        move.capture = true;
        move.board.at(line, row) = 0x0;
        move.board.at(line + shift, forward_row) = board_.at(line, row);
        move.board.resetNumberOfHalfMoves();
        move.board.setEnPassantTargetSquare(Square::InvalidSquare);
        bool is_king_capture = isKing(board_.at(line + shift, forward_row));
        if (is_en_passant_capture) {
          BoardAssert(board_, move.board.at(line + shift, row) == 'P' ||
                              move.board.at(line + shift, row) == 'p');
          move.board.at(line + shift, row) = 0x0;
        }
        if (!is_king_capture && isFinalRank(forward_row)) {
          handlePawnPromotion(move, line + shift, forward_row);
        } else {
          handleMove(move, is_king_capture);
        }
      }
    }
  };
  helper(1);
  helper(-1);
}

void MoveCalculator::handlePawnPromotion(const Move& move, size_t line, size_t row) {
  assert(move.board.at(line, row) == 'p' || move.board.at(line, row) == 'P');
  assert(isFinalRank(row));
  auto helper = [this, &move, line, row](char figure) {
    Move copy = move;
    copy.board.at(line, row) = figure;
    copy.promotion = figure;
    handleMove(copy, false);
  };

  if (move.board.at(line, row) == 'P') {
    helper('Q');
    helper('R');
    helper('B');
    helper('N');
  } else {
    helper('q');
    helper('r');
    helper('b');
    helper('n');
  }
}

void MoveCalculator::calculateMovesForKnight(size_t line, size_t row) {
  handlePossibleMove(line, row, line + 1, row + 2);
  handlePossibleMove(line, row, line - 1, row + 2);
  handlePossibleMove(line, row, line + 2, row + 1);
  handlePossibleMove(line, row, line + 2, row - 1);
  handlePossibleMove(line, row, line + 1, row - 2);
  handlePossibleMove(line, row, line - 1, row - 2);
  handlePossibleMove(line, row, line - 2, row + 1);
  handlePossibleMove(line, row, line - 2, row - 1);
}

void MoveCalculator::calculateMovesForBishop(size_t line, size_t row) {
  for (int offset = 1;
       handlePossibleMove(line, row, line + offset, row + offset);
       ++offset);
  for (int offset = 1;
       handlePossibleMove(line, row, line + offset, row - offset);
       ++offset);
  for (int offset = 1;
       handlePossibleMove(line, row, line - offset, row + offset);
       ++offset);
  for (int offset = 1;
       handlePossibleMove(line, row, line - offset, row - offset);
       ++offset);
}

void MoveCalculator::calculateMovesForRook(size_t line, size_t row) {
  for (int offset = 1;
       handlePossibleMove(line, row, line + offset, row);
       ++offset);
  for (int offset = 1;
       handlePossibleMove(line, row, line - offset, row);
       ++offset);
  for (int offset = 1;
       handlePossibleMove(line, row, line, row + offset);
       ++offset);
  for (int offset = 1;
       handlePossibleMove(line, row, line, row - offset);
       ++offset);
}

void MoveCalculator::calculateMovesForQueen(size_t line, size_t row) {
  calculateMovesForRook(line, row);
  calculateMovesForBishop(line, row);
}

void MoveCalculator::calculateMovesForKing(size_t line, size_t row) {
  handlePossibleMove(line, row, line + 1, row + 1);
  handlePossibleMove(line, row, line + 1, row);
  handlePossibleMove(line, row, line + 1, row - 1);
  handlePossibleMove(line, row, line, row - 1);
  handlePossibleMove(line, row, line - 1, row - 1);
  handlePossibleMove(line, row, line - 1, row);
  handlePossibleMove(line, row, line - 1, row + 1);
  handlePossibleMove(line, row, line, row + 1);
  if (look_for_king_capture_ == false) {
    handlePossibleCastlings(line, row);
  }
}

void MoveCalculator::handlePossibleCastlings(size_t line, size_t row) {
  const bool is_white = isWhite(board_.at(line, row));
  const size_t king_starting_row = is_white ? 0 : Board::kBoardSize - 1;

  if (line != kKingStartingLine || row != king_starting_row) {
    return;
  }

  auto helper = [this](Castling castling, bool king_side, bool is_white, size_t row) {
    if (board_.canCastle(castling) == false) {
      return;
    }
    const char rook = is_white ? 'R' : 'r';
    const size_t rooks_line = king_side ? kKingSideRookStartingLine : kQueenSideRookStartingLine;
    if (board_.at(rooks_line, row) != rook) {
      return;
    }
    const int shift = king_side ? 1 : -1;
    if (board_.at(kKingStartingLine + shift, row) || board_.at(kKingStartingLine + 2 * shift, row)) {
      return;
    }

    auto isMoveValid = [this, row](size_t new_line) -> bool {
      Board copy = board_;
      copy.changeSideToMove();
      copy.setEnPassantTargetSquare(Square::InvalidSquare);
      const char king = copy.at(kKingStartingLine, row);
      assert (king == 'K' || king == 'k');
      copy.at(kKingStartingLine, row) = 0x0;
      copy.at(new_line, row) = king;
      MoveCalculator calculator(copy, true);
      try {
        calculator.calculateAllMoves();
        // No exception was thrown so move is valid.
        return true;
      } catch (KingInCheckException&) {
      }
      return false;
    };

    if (isMoveValid(kKingStartingLine) == false ||
        isMoveValid(kKingStartingLine + shift) == false ||
        isMoveValid(kKingStartingLine + 2 * shift) == false) {
      return;
    }

    // All checks are fine, castling is possible
    Move move(board_, kKingStartingLine, row, kKingStartingLine + 2 * shift, row);
    move.board.incrementNumberOfHalfMoves();
    move.board.setEnPassantTargetSquare(Square::InvalidSquare);
    move.board.resetCastlings(is_white);
    const char king = move.board.at(kKingStartingLine, row);
    move.board.at(kKingStartingLine, row) = 0x0;
    move.board.at(rooks_line, row) = 0x0;
    move.board.at(kKingStartingLine + 2 * shift, row) = king;
    move.board.at(kKingStartingLine + shift, row) = rook;
    move.castling = castling;
    handleMove(move, false);
  };

  if (is_white) {
    helper(Castling::K, true, true, 0);
    helper(Castling::Q, false, true, 0);
  } else {
    helper(Castling::k, true, false, Board::kBoardSize - 1);
    helper(Castling::q, false, false, Board::kBoardSize - 1);
  }
}
