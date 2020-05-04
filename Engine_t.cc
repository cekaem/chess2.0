/* Component tests for class Engine */

#include <string>
#include <vector>

#include "Engine.h"
#include "utils/Mock.h"
#include "utils/Test.h"

namespace {

bool MovesEqual(const Move& move, const std::string& fen) {
  return move.board.createFEN() == fen;
}


TEST_PROCEDURE(Engine_beats_figure) {
  TEST_START
  Engine engine;
  Board board("8/8/3k4/4Q3/8/8/8/4K3 b - - 0 1");
  Move move = engine.calculateBestMove(board, 2);
  VERIFY_TRUE(MovesEqual(move, "8/8/8/4k3/8/8/8/4K3 w - - 0 2"));
  TEST_END
}

TEST_PROCEDURE(Engine_promotes_pawn) {
  TEST_START
  Engine engine;
  Board board("8/4KP1k/8/8/8/8/8/8 w - - 0 1");
  Move move = engine.calculateBestMove(board, 2);
  VERIFY_TRUE(MovesEqual(move, "5Q2/4K2k/8/8/8/8/8/8 b - - 0 1"));
  TEST_END
}

TEST_PROCEDURE(Engine_finds_mate_in_one) {
  TEST_START
  Engine engine;
  {
    Board board("8/8/8/8/8/5K1k/5Q2/8 w - - 0 1");
    Move move = engine.calculateBestMove(board, 2);
    VERIFY_TRUE(MovesEqual(move, "8/8/8/8/8/5KQk/8/8 b - - 1 1"));
  }
  {
    Board board("6K1/8/6k1/3Qr3/8/8/8/8 b - - 0 1");
    Move move = engine.calculateBestMove(board, 2);
    VERIFY_TRUE(MovesEqual(move, "4r1K1/8/6k1/3Q4/8/8/8/8 w - - 1 2"));
  }
  TEST_END
}

}  // unnamed namespace
