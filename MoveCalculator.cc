#include "MoveCalculator.h"

#include <cassert>

#include <iostream>

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
  return (figure == 'P' && row == 2) ||
         (figure == 'p' && row == Board::kBoardSize - 2);
}

constexpr size_t kKingStartingLine = 4lu;
constexpr size_t kQueenSideRookStartingLine = 0lu;
constexpr size_t kKingSideRookStartingLine = Board::kBoardSize - 1;

}  // unnamed namespace


std::vector<Board> MoveCalculator::calculateAllMoves() {
  for (size_t i = 0; i < Board::kBoardSize; ++i) {
    for (size_t j = 0; j < Board::kBoardSize; ++j) {
      char square = board_[i][j];
      if (square == 0x0 || board_.whiteToMove() != isWhite(square)) {
        continue;
      }
      switch (square) {
        case 'p':
        case 'P':
          calculateMovesForPawn(i, j);
          break;
        case 'n':
        case 'N':
          calculateMovesForKnight(i, j);
          break;
        case 'b':
        case 'B':
          calculateMovesForBishop(i, j);
          break;
        case 'r':
        case 'R':
          calculateMovesForRook(i, j);
          break;
        case 'q':
        case 'Q':
          calculateMovesForQueen(i, j);
          break;
        case 'k':
        case 'K':
          calculateMovesForKing(i, j);
          break;
      }
    }
  }
  return moves_;
}

bool MoveCalculator::isValidMove(size_t old_line, size_t old_row,
                                 size_t new_line, size_t new_row) {
  if (board_[new_line][new_row] == 0x0) {
    return true;
  }
  return isWhite(board_[old_line][old_row]) != isWhite(board_[new_line][new_row]);
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
  Board board = board_;
  char new_square = board_[new_line][new_row];
  if (new_square) {
    board.resetNumberOfHalfMoves();
  } else {
    board.incrementNumberOfHalfMoves();
  }
  board.setEnPassantTargetSquare(Square::InvalidSquare);
  board[new_line][new_row] = board[old_line][old_row];
  board[old_line][old_row] = 0x0;
  resetCastlings(board, board[new_line][new_row], old_line, old_row);
  handleMove(board, isKing(new_square));
  return board_[new_line][new_row] == 0x0;
}

void MoveCalculator::handleMove(Board& board, bool is_king_capture) {
  if (look_for_king_capture_) {
    if (is_king_capture) {
      throw KingInCheckException();
    }
    return;
  }
  if (is_king_capture) {
    throw InvalidPositionException("Moving side checks the opponent's king");
  }

  board.changeSideToMove();
  try {
    MoveCalculator calculator(board, true);
    calculator.calculateAllMoves();
    // Exception KingInCheckException was not thrown so move is valid.
    if (board.whiteToMove()) {
      board.incrementNumberOfMoves();
    }
    moves_.push_back(board);
  } catch (KingInCheckException& e) {
  }
}

void MoveCalculator::calculateMovesForPawn(size_t line, size_t row) {
  assert(board_[line][row] == 'p' || board_[line][row] == 'P');
  bool is_white = isWhite(board_[line][row]);
  int forward = is_white ? 1 : -1;
  size_t forward_row = row + forward;
  if (isSquareOnBoard(line, forward_row) == false) {
    throw InvalidPositionException("calculateMovesForPawn: pawn on the first/last row");
  }

  auto helper = [this](size_t line, size_t old_row, size_t new_row, Square en_passant) {
    Board board = board_;
    board[line][old_row] = 0x0;
    board[line][new_row] = board_[line][old_row];
    board.resetNumberOfHalfMoves();
    board.setEnPassantTargetSquare(en_passant);
    if (isFinalRank(new_row)) {
      handlePawnPromotion(board, line, new_row);
    } else {
      handleMove(board, false);
    }
  };

  char examined_square = board_[line][forward_row];
  if (examined_square == 0x0) {
    helper(line, row, forward_row, Square::InvalidSquare);
    if (isStartingRow(board_[line][row], row)) {
      forward_row += forward;
      examined_square = board_[line][forward_row];
      if (examined_square == 0x0) {
        Square en_passant(line, row + forward);
        helper(line, row, forward_row, en_passant);
      }
    }
  }

  handlePossiblePawnsCapture(line, row);
}

void MoveCalculator::handlePossiblePawnsCapture(size_t line, size_t row) {
  assert(board_[line][row] == 'p' || board_[line][row] == 'P');
  auto helper = [this, line, row](int shift) {
    bool is_white = isWhite(board_[line][row]);
    size_t forward_row = is_white ? row + 1 : row - 1;
    if (isSquareOnBoard(line + shift, forward_row)) {
      Square en_passant_square = board_.getEnPassantTargetSquare();
      bool is_en_passant_capture = Square(line + shift, forward_row) == en_passant_square;
      if (is_en_passant_capture ||
          (board_[line + shift][forward_row] &&
           isWhite(board_[line + shift][forward_row]) != is_white)) {
        Board board = board_;
        board[line][row] = 0x0;
        board[line + shift][forward_row] = board_[line][row];
        board.resetNumberOfHalfMoves();
        board.setEnPassantTargetSquare(Square::InvalidSquare);
        bool is_king_capture = isKing(board_[line + shift][forward_row]);
        if (is_en_passant_capture) {
          assert(board[line + shift][row] == 'P' ||
                 board[line + shift][row] == 'p');
          board[line + shift][row] = 0x0;
        }
        if (!is_king_capture && isFinalRank(forward_row)) {
          handlePawnPromotion(board, line, forward_row);
        } else {
          handleMove(board, is_king_capture);
        }
      }
    }
  };
  helper(1);
  helper(-1);
}

void MoveCalculator::handlePawnPromotion(const Board& board, size_t line, size_t row) {
  assert(board[line][row] == 'p' || board[line][row] == 'P');
  assert(isFinalRank(row));
  auto helper = [this, board, line, row](char figure) {
    Board copy = board;
    copy[line][row] = figure;
    handleMove(copy, false);
  };

  if (board[line][row] == 'P') {
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
  handlePossibleCastlings(line, row);
}

void MoveCalculator::handlePossibleCastlings(size_t line, size_t row) {
  const bool is_white = isWhite(board_[line][row]);
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
    if (board_[rooks_line][row] != rook) {
      return;
    }
    const int shift = king_side ? 1 : -1;
    if (board_[kKingStartingLine + shift][row] || board_[kKingStartingLine + 2 * shift][row]) {
      return;
    }

    auto isMoveValid = [this, row](size_t new_line) -> bool {
      Board copy = board_;
      copy.changeSideToMove();
      const char king = copy[kKingStartingLine][row];
      assert (king == 'K' || king == 'k');
      copy[kKingStartingLine][row] = 0x0;
      copy[new_line][row] = king;
      MoveCalculator calculator(copy, true);
      try {
        calculator.calculateAllMoves();
        // No exception was thrown so move is valid.
        return true;
      } catch (KingInCheckException&) {
      }
      return false;
    };

    if (look_for_king_capture_ == false &&
        isMoveValid(kKingStartingLine) == false) {
      return;
    }

    if (isMoveValid(kKingStartingLine + shift) == false ||
        isMoveValid(kKingStartingLine + 2 * shift) == false) {
      return;
    }

    // All checks are fine, castling is possible
    Board copy = board_;
    copy.incrementNumberOfHalfMoves();
    copy.setEnPassantTargetSquare(Square::InvalidSquare);
    copy.resetCastlings(is_white);
    const char king = copy[kKingStartingLine][row];
    copy[kKingStartingLine][row] = 0x0;
    copy[rooks_line][row] = 0x0;
    copy[kKingStartingLine + 2 * shift][row] = king;
    copy[kKingStartingLine + shift][row] = rook;
    handleMove(copy, false);
  };

  if (is_white) {
    helper(Castling::K, true, true, 0);
    helper(Castling::Q, false, true, 0);
  } else {
    helper(Castling::k, true, false, Board::kBoardSize - 1);
    helper(Castling::q, false, false, Board::kBoardSize - 1);
  }
}
