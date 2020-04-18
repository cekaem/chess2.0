#include "MoveCalculator.h"

#include <cassert>


namespace {

bool isSquareOnBoard(int line, int row) {
  return line >= 0 && line < static_cast<int>(Board::kBoardSize) &&
         row >= 0 && row < static_cast<int>(Board::kBoardSize);
}

bool isKing(const char figure) {
  return figure == 'k' || figure == 'K';
}

}  // unnamed namespace


std::vector<Board> MoveCalculator::calculateAllMoves() {
  for (size_t i = 0; i < Board::kBoardSize; ++i) {
    for (size_t j = 0; j < Board::kBoardSize; ++j) {
      if (board_[i][j] == 0x0 || board_.whiteToMove() != isWhite(i, j)) {
        continue;
      }
      switch (board_[i][j]) {
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

bool MoveCalculator::isWhite(size_t line, size_t row) {
  char figure = board_[line][row];
  return figure == 'P' || figure == 'K' || figure == 'N' ||
         figure == 'B' || figure == 'R' || figure == 'Q';
}

bool MoveCalculator::isValidMove(size_t old_line, size_t old_row,
                                 size_t new_line, size_t new_row) {
  if (board_[new_line][new_row] == 0x0) {
    return true;
  }
  return isWhite(old_line, old_row) != isWhite(new_line, new_row);
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
  board[new_line][new_row] = board[old_line][old_row];
  board[old_line][old_row] = 0x0;
  handleMove(board, isKing(new_square));
  return board_[new_line][new_row] == 0x0;
}

void MoveCalculator::handleMove(Board& board, bool is_check) {
  if (look_for_check_) {
    if (is_check) {
      throw KingInCheckException();
    }
    return;
  }
  if (is_check) {
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
  bool is_white = isWhite(line, row);
  int forward = is_white ? 1 : -1;
  size_t forward_row = row + forward;
  if (isSquareOnBoard(line, forward_row) == false) {
    throw InvalidPositionException("calculateMovesForPawn: pawn on the first/last row");
  }

  char examined_square = board_[line][forward_row];
  if (examined_square == 0x0) {
    if (forward_row == 0 || forward_row == Board::kBoardSize - 1) {
      // TODO: promotion
    } else {
      Board board = board_;
      board[line][row] = 0x0;
      board[line][forward_row] = board_[line][row];
      board.resetNumberOfHalfMoves();
      handleMove(board, false);
    }
  }

  if (isSquareOnBoard(line - 1, forward_row)) {
    examined_square = board_[line - 1][forward_row];
    if (examined_square && isWhite(line - 1, forward_row) != is_white) {
      if (forward_row == 0 || forward_row == Board::kBoardSize - 1) {
        // TODO: promotion
      } else {
        Board board = board_;
        board[line][row] = 0x0;
        board[line - 1][forward_row] = board_[line][row];
        board.resetNumberOfHalfMoves();
        handleMove(board, isKing(board_[line - 1][forward_row]));
      }
    }
  }

  if (isSquareOnBoard(line + 1, forward_row)) {
    examined_square = board_[line + 1][forward_row];
    if (examined_square && isWhite(line + 1, forward_row) != is_white) {
      if (forward_row == 0 || forward_row == Board::kBoardSize - 1) {
        // TODO: promotion
      } else {
        Board board = board_;
        board[line][row] = 0x0;
        board[line + 1][forward_row] = board_[line][row];
        board.resetNumberOfHalfMoves();
        handleMove(board, isKing(board_[line + 1][forward_row]));
      }
    }
  }

  // TODO: en passant
}

void MoveCalculator::calculateMovesForKnight(size_t line, size_t row) {
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
}

void MoveCalculator::calculateMovesForQueen(size_t line, size_t row) {
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
  // TODO: castlings
}
