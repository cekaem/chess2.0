/* Component tests for class MoveCalculator */

#include <string>
#include <vector>

#include "Board.h"
#include "MoveCalculator.h"
#include "utils/Test.h"

namespace {

bool MovesDoesNotContainMove(const std::vector<Board>& moves, const std::string& fen) {
  for (const auto& move: moves) {
    if (move == fen) {
      return false;
    }
  }
  return true;
}


TEST_PROCEDURE(MoveCalculator_figure_does_not_unveil_its_king) {
  TEST_START
  Board board("8/3k4/5b2/4P3/3K4/8/8/8 w - - 0 1");
  MoveCalculator calculator(board);
  std::vector<Board> moves = calculator.calculateAllMoves();
  VERIFY_TRUE(MovesDoesNotContainMove(moves, "8/3k4/4Pb2/8/3K4/8/8/8 b - - 1 2"));
  VERIFY_EQUALS(moves.size(), 7ul);
  TEST_END
}

} // unnamed namespace
