/* Component tests for class MoveCalculator */

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Board.h"
#include "MoveCalculator.h"
#include "utils/Test.h"

namespace {

bool MovesContainMove(const std::vector<Board>& moves, const std::string& fen) {
  for (const auto& move: moves) {
    if (move == fen) {
      return true;
    }
  }
  return false;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

std::ostream& operator<<(std::ostream& ostr, const std::vector<Board>& moves) {
  for (const auto& move : moves) {
    ostr << move << std::endl;
  }
  return ostr;
}

#pragma GCC diagnostic pop

// ==================================================================================

TEST_PROCEDURE(MoveCalculator_exception_thrown_on_invalid_position) {
  TEST_START
  Board board("8/8/3k4/4P3/8/3K4/8/8 w - - 0 1");
  MoveCalculator calculator(board);
  try {
    auto moves = calculator.calculateAllMoves();
  } catch (MoveCalculator::InvalidPositionException& e) {
    RETURN;
  }
  NOT_REACHED("Exception InvalidPositionException was not thrown");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_exception_thrown_on_pawn_on_last_row) {
  TEST_START
  Board board("8/8/3k4/8/8/3K4/8/1p6 b - - 0 1");
  MoveCalculator calculator(board);
  try {
    calculator.calculateAllMoves();
  } catch (MoveCalculator::InvalidPositionException& e) {
    RETURN;
  }
  NOT_REACHED("Exception InvalidPositionException was not thrown");
  TEST_END
}


TEST_PROCEDURE(MoveCalculator_pawn_moves) {
  TEST_START
  {
    Board board("2k5/5P1p/8/3p4/3pP3/3P4/8/4K3 w - - 1 1");
    MoveCalculator calculator(board);
    std::vector<Board> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3P4/3p4/3P4/8/4K3 b - - 0 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3pP3/3p4/3P4/8/4K3 b - - 0 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k2N2/7p/8/3p4/3pP3/3P4/8/4K3 b - - 0 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k2B2/7p/8/3p4/3pP3/3P4/8/4K3 b - - 0 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k2R2/7p/8/3p4/3pP3/3P4/8/4K3 b - - 0 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k2Q2/7p/8/3p4/3pP3/3P4/8/4K3 b - - 0 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3p4/3pP3/3P4/8/3K4 b - - 2 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3p4/3pP3/3P4/3K4/8 b - - 2 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3p4/3pP3/3P4/4K3/8 b - - 2 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3p4/3pP3/3P4/5K2/8 b - - 2 1"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/3p4/3pP3/3P4/8/5K2 b - - 2 1"));
    VERIFY_EQUALS(moves.size(), 11lu);
  }
  {
    Board board("2k5/5P1p/8/3p4/3pP3/3P4/8/4K3 b - - 1 1");
    MoveCalculator calculator(board);
    std::vector<Board> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P2/7p/3p4/3pP3/3P4/8/4K3 w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P2/8/3p3p/3pP3/3P4/8/4K3 w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/8/3pp3/3P4/8/4K3 w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "1k6/5P1p/8/3p4/3pP3/3P4/8/4K3 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/1k3P1p/8/3p4/3pP3/3P4/8/4K3 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/2k2P1p/8/3p4/3pP3/3P4/8/4K3 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/3k1P1p/8/3p4/3pP3/3P4/8/4K3 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "3k4/5P1p/8/3p4/3pP3/3P4/8/4K3 w - - 1 2"));
    VERIFY_EQUALS(moves.size(), 8lu);
  }
  {
    Board board("k7/8/8/8/8/8/7p/K7 b - d3 0 1");
    MoveCalculator calculator(board);
    std::vector<Board> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6q w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6r w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6b w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6n w - - 0 2"));
  }
  {
    Board board("k7/8/8/5Pp1/8/8/8/K7 w - g6 5 77");
    MoveCalculator calculator(board);
    std::vector<Board> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/6P1/8/8/8/8/K7 w - - 0 77"));
  }
  {
    Board board("k7/8/8/8/3Pp3/8/8/K7 b - d3 10 10");
    MoveCalculator calculator(board);
    std::vector<Board> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/3p4/8/K7 b - - 0 11"));
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_bishop_moves) {
  TEST_START
  Board board("8/2B5/7k/2K5/5b2/8/8/8 b - - 0 14");
  MoveCalculator calculator(board);
  auto moves = calculator.calculateAllMoves();
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K1b3/8/8/8/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/3b3k/2K5/8/8/8/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2b5/7k/2K5/8/8/8/8 w - - 0 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K3b1/8/8/8/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K5/8/6b1/8/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K5/8/8/7b/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K5/8/4b3/8/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K5/8/8/3b4/8 w - - 1 15"));
  VERIFY_TRUE(MovesContainMove(moves, "8/2B5/7k/2K5/8/8/8/2b5 w - - 1 15"));
  VERIFY_EQUALS(moves.size(), 14ul);

  TEST_END
}

TEST_PROCEDURE(MoveCalculator_figure_does_not_unveil_its_king) {
  TEST_START
  Board board("8/3k4/5b2/4P3/3K4/8/8/8 w - - 0 1");
  MoveCalculator calculator(board);
  std::vector<Board> moves = calculator.calculateAllMoves();
  VERIFY_FALSE(MovesContainMove(moves, "8/3k4/4Pb2/8/3K4/8/8/8 b - - 0 1"));
  VERIFY_EQUALS(moves.size(), 8ul);
  TEST_END
}

TEST_PROCEDURE(King_does_not_walk_into_check) {
  TEST_START
  Board board("7k/8/4BK2/8/8/8/8/8 b - - 0 1");
  MoveCalculator calculator(board);
  std::vector<Board> moves = calculator.calculateAllMoves();
  VERIFY_TRUE(MovesContainMove(moves, "8/7k/4BK2/8/8/8/8/8 w - - 1 2"));
  VERIFY_EQUALS(moves.size(), 1lu);
  TEST_END
}

} // unnamed namespace
