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

bool isFinalRank(size_t row) {
  return row == 0 || row == Board::kBoardSize - 1;
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

void MoveCalculator::handleMove(Board& board, bool is_king_capture) {
  if (look_for_check_) {
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
  bool is_white = isWhite(line, row);
  int forward = is_white ? 1 : -1;
  size_t forward_row = row + forward;
  if (isSquareOnBoard(line, forward_row) == false) {
    throw InvalidPositionException("calculateMovesForPawn: pawn on the first/last row");
  }

  char examined_square = board_[line][forward_row];
  if (examined_square == 0x0) {
    Board board = board_;
    board[line][row] = 0x0;
    board[line][forward_row] = board_[line][row];
    board.resetNumberOfHalfMoves();
    if (isFinalRank(forward_row)) {
      handlePawnPromotion(board, line, forward_row);
    } else {
      handleMove(board, false);
    }
  }

  handlePossiblePawnsCapture(line, row);

  // TODO: en passant
}

void MoveCalculator::handlePossiblePawnsCapture(size_t line, size_t row) {
  assert(board_[line][row] == 'p' || board_[line][row] == 'P');
  auto helper = [this, line, row](int shift) {
    bool is_white = isWhite(line, row);
    size_t forward_row = is_white ? row + 1 : row - 1;
    if (isSquareOnBoard(line + shift, forward_row)) {
      if (board_[line + shift][forward_row] &&
          isWhite(line + shift, forward_row) != is_white) {
        Board board = board_;
        board[line][row] = 0x0;
        board[line + shift][forward_row] = board_[line][row];
        board.resetNumberOfHalfMoves();
        bool is_king_capture = isKing(board_[line + shift][forward_row]);
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
  // TODO: castlings
}
