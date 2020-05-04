/* Component tests for class MoveCalculator */

#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "Board.h"
#include "MoveCalculator.h"
#include "utils/Test.h"

namespace {

bool MovesContainMove(const std::vector<Move>& moves, const std::string& fen) {
  for (const auto& move: moves) {
    if (move.board == fen) {
      return true;
    }
  }
  return false;
}

bool MovesContainMove(const std::vector<Move>& moves,
                      const char* old_square,
                      const char* new_square,
                      Castling castling,
                      bool capture,
                      char promotion) {
  for (const auto& move: moves) {
    if (move.old_square == Square(old_square) &&
        move.new_square == Square(new_square) &&
        move.castling == castling &&
        move.capture == capture &&
        move.promotion == promotion) {
      return true;
    }
  }
  return false;
}

bool MovesContainMove(const std::vector<Move>& moves,
                      const char* old_square,
                      const char* new_square,
                      bool insufficient_material) {
  for (const auto& move: moves) {
    if (move.old_square == Square(old_square) &&
        move.new_square == Square(new_square) &&
        move.insufficient_material == insufficient_material) {
      return true;
    }
  }
  return false;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

std::ostream& operator<<(std::ostream& ostr, const std::vector<Move>& moves) {
  for (const auto& move : moves) {
    ostr << move;
    ostr << move.board << std::endl << std::endl;
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
    std::vector<Move> moves = calculator.calculateAllMoves();
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
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P2/7p/3p4/3pP3/3P4/8/4K3 w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P2/8/3p3p/3pP3/3P4/8/4K3 w - h6 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "2k5/5P1p/8/8/3pp3/3P4/8/4K3 w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "1k6/5P1p/8/3p4/3pP3/3P4/8/4K3 w - - 2 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/1k3P1p/8/3p4/3pP3/3P4/8/4K3 w - - 2 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/2k2P1p/8/3p4/3pP3/3P4/8/4K3 w - - 2 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/3k1P1p/8/3p4/3pP3/3P4/8/4K3 w - - 2 2"));
    VERIFY_TRUE(MovesContainMove(moves, "3k4/5P1p/8/3p4/3pP3/3P4/8/4K3 w - - 2 2"));
    VERIFY_EQUALS(moves.size(), 8lu);
  }
  {
    Board board("k7/8/8/8/8/8/7p/K7 b - d3 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6q w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6r w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6b w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/8/8/K6n w - - 0 2"));
  }
  {
    Board board("k7/8/8/5Pp1/8/8/8/K7 w - g6 5 77");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/6P1/8/8/8/8/K7 b - - 0 77"));
  }
  {
    Board board("k7/8/8/8/3Pp3/8/8/K7 b - d3 10 10");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "k7/8/8/8/8/3p4/8/K7 w - - 0 11"));
  }
  {
    Board board("8/8/8/8/8/7k/7p/6BK b - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "8/8/8/8/8/7k/8/6qK w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/8/8/8/7k/8/6rK w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/8/8/8/7k/8/6bK w - - 0 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/8/8/8/7k/8/6nK w - - 0 2"));
    VERIFY_EQUALS(moves.size(), 7lu);
  }
  {
    Board board("rnbqk1nr/1pppb2p/p5p1/5pP1/1P2pP1P/7N/P1PPP1B1/RNBQK2R b KQkq - 0 8");
    MoveCalculator calculator(board);
    auto moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "rnbqk1nr/2ppb2p/p5p1/1p3pP1/1P2pP1P/7N/P1PPP1B1/RNBQK2R w KQkq b6 0 9"));
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

TEST_PROCEDURE(MoveCalculator_rook_moves) {
  TEST_START
  Board board("8/8/5K2/8/3p4/8/1N1r2k1/8 b - - 5 1");
  MoveCalculator calculator(board);
  auto moves = calculator.calculateAllMoves();
  VERIFY_TRUE(MovesContainMove(moves, "8/8/5K2/8/3p4/3r4/1N4k1/8 w - - 6 2"));
  VERIFY_TRUE(MovesContainMove(moves, "8/8/5K2/8/3p4/8/1Nr3k1/8 w - - 6 2"));
  VERIFY_TRUE(MovesContainMove(moves, "8/8/5K2/8/3p4/8/1N4k1/3r4 w - - 6 2"));
  VERIFY_TRUE(MovesContainMove(moves, "8/8/5K2/8/3p4/8/1N2r1k1/8 w - - 6 2"));
  VERIFY_TRUE(MovesContainMove(moves, "8/8/5K2/8/3p4/8/1N3rk1/8 w - - 6 2"));
  VERIFY_TRUE(MovesContainMove(moves, "8/8/5K2/8/3p4/8/1r4k1/8 w - - 0 2"));
  VERIFY_EQUALS(moves.size(), 15ul);
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_knight_moves) {
  TEST_START
  {
    Board board("8/8/1k6/5n2/2K5/8/8/8 b - - 0 1");
    MoveCalculator calculator(board);
    auto moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/2K5/6n1/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/2K5/4n3/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/2Kn4/8/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k1n4/8/2K5/8/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/4n3/1k6/8/2K5/8/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/6n1/1k6/8/2K5/8/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k5n/8/2K5/8/8/8 w - - 1 2"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/2K4n/8/8/8 w - - 1 2"));
    VERIFY_EQUALS(moves.size(), 14ul);
  }
  {
    Board board("8/8/1k6/8/b1K5/8/1N6/8 w - - 0 1");
    MoveCalculator calculator(board);
    auto moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/b1K5/8/8/3N4 b - - 1 1"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/b1K5/3N4/8/8 b - - 1 1"));
    VERIFY_TRUE(MovesContainMove(moves, "8/8/1k6/8/N1K5/8/8/8 b - - 0 1"));
    VERIFY_EQUALS(moves.size(), 8ul);
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_figure_does_not_unveil_its_king) {
  TEST_START
  {
    Board board("8/3k4/5b2/4P3/3K4/8/8/8 w - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "8/3k4/4Pb2/8/3K4/8/8/8 b - - 0 1"));
    VERIFY_EQUALS(moves.size(), 8ul);
  }
  {
    Board board("8/8/8/8/8/7k/8/5rQK w - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "8/8/8/8/8/7k/8/5Q1K b - - 0 1"));
    VERIFY_EQUALS(moves.size(), 1ul);
  }
  TEST_END
}

TEST_PROCEDURE(King_does_not_walk_into_check) {
  TEST_START
  Board board("7k/8/4BK2/8/8/8/8/8 b - - 0 1");
  MoveCalculator calculator(board);
  std::vector<Move> moves = calculator.calculateAllMoves();
  VERIFY_TRUE(MovesContainMove(moves, "8/7k/4BK2/8/8/8/8/8 w - - 1 2"));
  VERIFY_EQUALS(moves.size(), 1lu);
  TEST_END
}

TEST_PROCEDURE(Removing_check) {
  TEST_START
  Board board("8/8/8/8/4b3/1n5k/NQ6/K7 w - - 0 1");
  MoveCalculator calculator(board);
  std::vector<Move> moves = calculator.calculateAllMoves();
  VERIFY_TRUE(MovesContainMove(moves, "8/8/8/8/4b3/1Q5k/N7/K7 b - - 0 1"));
  VERIFY_EQUALS(moves.size(), 1lu);
  TEST_END
}

TEST_PROCEDURE(Registration_of_moves_is_correct) {
TEST_START
  {
    Board board("8/8/8/8/8/7k/7p/6BK b - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "h2", "g1", Castling::LAST, true, 'q'));
    VERIFY_TRUE(MovesContainMove(moves, "h2", "g1", Castling::LAST, true, 'r'));
    VERIFY_TRUE(MovesContainMove(moves, "h2", "g1", Castling::LAST, true, 'b'));
    VERIFY_TRUE(MovesContainMove(moves, "h2", "g1", Castling::LAST, true, 'n'));
  }
  {
    Board board("4k3/8/8/8/1Pp5/8/8/4K3 b - b3 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "c4", "b3", Castling::LAST, true, 0x0));
  }
  {
    Board board("r3k3/8/8/8/8/8/8/4K2R w KQkq - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "e1", "g1", Castling::K, false, 0x0));
  }
  {
    Board board("r3k3/8/8/8/8/8/8/4K2R b KQkq - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "e8", "c8", Castling::q, false, 0x0));
  }
TEST_END
}

TEST_PROCEDURE(InsufficientMaterial) {
TEST_START
  {
    Board board("8/8/8/5k2/8/6Kp/8/8 w - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "g3", "f3", false));
    VERIFY_TRUE(MovesContainMove(moves, "g3", "h3", true));
  }
  {
    Board board("8/8/8/5K2/3B4/5Bk1/8/8 b - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "g3", "f3", true));
    VERIFY_TRUE(MovesContainMove(moves, "g3", "h2", false));
  }
  {
    Board board("8/8/8/5k2/3b4/5nK1/8/8 w - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "g3", "f3", true));
    VERIFY_TRUE(MovesContainMove(moves, "g3", "h3", false));
  }
  {
    Board board("8/8/1b6/5k2/8/5rK1/8/8 w - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "g3", "f3", true));
    VERIFY_TRUE(MovesContainMove(moves, "g3", "h2", false));
  }
  {
    Board board("8/8/8/5K2/8/5Qk1/8/8 b - - 0 1");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "g3", "f3", true));
    VERIFY_TRUE(MovesContainMove(moves, "g3", "h2", false));
  }
  {
    Board board("8/8/2Pk4/3n4/8/K7/6B1/4b3 b - - 0 154");
    MoveCalculator calculator(board);
    auto moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "d6", "c6", true));

  }
TEST_END
}


TEST_PROCEDURE(Castlings) {
TEST_START
  {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 11");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/R4RK1 b kq - 1 11"));
    VERIFY_TRUE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/2KR3R b kq - 1 11"));
    VERIFY_FALSE(MovesContainMove(moves, "r4rk1/8/8/8/8/8/8/R3K2R b KQ - 1 11"));
    VERIFY_FALSE(MovesContainMove(moves, "2kr3r/8/8/8/8/8/8/R3K2R b KQ - 1 11"));
  }
  {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 11");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/R4RK1 b - - 1 11"));
    VERIFY_FALSE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/2KR3R b - - 1 11"));
    VERIFY_FALSE(MovesContainMove(moves, "r4rk1/8/8/8/8/8/8/R3K2R b - - 1 11"));
    VERIFY_FALSE(MovesContainMove(moves, "2kr3r/8/8/8/8/8/8/R3K2R b - - 1 11"));
  }
  {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 11");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/R4RK1 w kq - 1 12"));
    VERIFY_FALSE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/2KR3R w kq - 1 12"));
    VERIFY_TRUE(MovesContainMove(moves, "r4rk1/8/8/8/8/8/8/R3K2R w KQ - 1 12"));
    VERIFY_TRUE(MovesContainMove(moves, "2kr3r/8/8/8/8/8/8/R3K2R w KQ - 1 12"));
  }
  {
    Board board("4k3/8/8/8/5r2/8/8/R3K2R w KQkq - 1 5");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "4k3/8/8/8/5r2/8/8/R4RK1 b kq - 2 5"));
    VERIFY_TRUE(MovesContainMove(moves, "4k3/8/8/8/5r2/8/8/2KR3R b kq - 2 5"));
  }
  {
    Board board("r3k2r/8/8/8/2R5/8/8/4K3 b KQkq - 0 5");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "2kr3r/8/8/8/2R5/8/8/4K3 w KQ - 1 6"));
    VERIFY_TRUE(MovesContainMove(moves, "r4rk1/8/8/8/2R5/8/8/4K3 w KQ - 1 6"));
  }
  {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 7 44");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/R4K1R b kq - 8 44"));
    VERIFY_TRUE(MovesContainMove(moves, "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 8 44"));
    VERIFY_TRUE(MovesContainMove(moves, "r3k2r/8/8/8/8/R7/8/4K2R b Kkq - 8 44"));
  }
  {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 11 90");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_TRUE(MovesContainMove(moves, "r6r/4k3/8/8/8/8/8/R3K2R w KQ - 12 91"));
    VERIFY_TRUE(MovesContainMove(moves, "r3k3/8/8/8/8/8/8/R3K2r w KQq - 0 91"));
    VERIFY_TRUE(MovesContainMove(moves, "3rk2r/8/8/8/8/8/8/R3K2R w KQk - 12 91"));
  }
  {
    Board board("4k3/8/8/8/4r3/8/8/R3K2R w KQ - 0 5");
    MoveCalculator calculator(board);
    std::vector<Move> moves = calculator.calculateAllMoves();
    VERIFY_FALSE(MovesContainMove(moves, "4k3/8/8/8/4r3/8/8/R4RK1 b - - 1 5"));
    VERIFY_FALSE(MovesContainMove(moves, "4k3/8/8/8/4r3/8/8/2KR3R b - - 1 5"));
  }
TEST_END
}

TEST_PROCEDURE(Regressions) {
TEST_START
  Board board("r5kr/4nqnp/p1p3p1/P7/1P3N2/4Qb2/1p1N1P2/2B2KR1 b - - 0 42");
  MoveCalculator calculator(board);
  calculator.calculateAllMoves();
TEST_END
}

TEST_PROCEDURE(NoMoves) {
TEST_START
  Board board("6r1/8/8/8/8/6bK/8/5k2 w - - 0 1");
  MoveCalculator calculator(board);
  auto moves = calculator.calculateAllMoves();
  VERIFY_EQUALS(moves.size(), 0lu);
TEST_END
}

TEST_PROCEDURE(IsCheck) {
TEST_START
  const std::vector<std::pair<std::string, bool>> pairs = {
    {"6r1/8/8/8/8/6bK/8/5k2 w - - 0 1", false},
    {"8/8/5b2/8/7K/8/8/5k2 w - - 0 1", true},
    {"8/8/5b2/3p4/2K5/6k1/5P2/8 b - - 0 1", true},
    {"8/8/5b2/3p4/2K5/6k1/6P1/8 b - - 0 1", false},
    {"8/8/5b2/3p4/2K2k2/8/4N1P1/8 b - - 0 1", true},
    {"8/8/5b2/8/2K2k2/2N5/2r3P1/8 w - - 0 1", false}
  };
  for (const auto& pair: pairs) {
    Board board(pair.first);
    MoveCalculator calculator(board);
    bool result = calculator.isCheck();
    if (result != pair.second) {
      NOT_REACHED(std::string("Bad result of MoveCalculator::isCheck for fen ") + pair.first);
    }
  }
TEST_END
}

TEST_PROCEDURE(InitialBoard) {
TEST_START
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  MoveCalculator calculator(board);
  std::vector<Move> moves = calculator.calculateAllMoves();
  VERIFY_EQUALS(moves.size(), 20lu);
TEST_END
}

} // unnamed namespace
