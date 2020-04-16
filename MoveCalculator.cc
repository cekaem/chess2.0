#include "MoveCalculator.h"

#include <cassert>


std::vector<Board> MoveCalculator::calculateAllMoves() {
  for (size_t i = 0; i < Board::kBoardSize; ++i) {
    for (size_t j = 0; j < Board::kBoardSize; ++j) {
      if (board_[i][j] == 0x0 || board_.whiteToMove() != isWhite(board_[i][j])) {
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

bool MoveCalculator::isWhite(char figure) const {
  return figure == 'P' || figure == 'K' || figure == 'N' ||
         figure == 'B' || figure == 'R' || figure == 'Q';
}


void MoveCalculator::handleMove(size_t old_row, size_t old_line,
                                size_t new_row, size_t new_line) {
  if (look_for_check_) {
    if (board_[new_row][new_line] == 'K' ||
        board_[new_row][new_line] == 'k') {
      throw KingInCheckException();
    }
    return;
  }
  Board board = board_;
  board[new_row][new_line] = board[old_row][old_line];
  board[old_row][old_line] = 0x0;
  board.changeSideToMove();
  bool valid_move = true;
  try {
    MoveCalculator calculator(board, true);
    calculator.calculateAllMoves();
  } catch (KingInCheckException& e) {
    valid_move = false;
  }
  if (valid_move) {
    moves_.push_back(board);
  }
}

void MoveCalculator::calculateMovesForPawn(size_t row, size_t line) {
}

void MoveCalculator::calculateMovesForKnight(size_t row, size_t line) {
}

void MoveCalculator::calculateMovesForBishop(size_t row, size_t line) {
}

void MoveCalculator::calculateMovesForRook(size_t row, size_t line) {
}

void MoveCalculator::calculateMovesForQueen(size_t row, size_t line) {
}
void MoveCalculator::calculateMovesForKing(size_t row, size_t line) {
}
