/* Component tests for class Board */

#include <string>
#include <vector>

#include "Board.h"
#include "utils/Mock.h"
#include "utils/Test.h"

namespace {

TEST_PROCEDURE(Board_fen_constructor_valid_fens) {
  TEST_START
  {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    VERIFY_EQUALS(board.getSquare("a1"), 'R');
    VERIFY_EQUALS(board.getSquare("d1"), 'Q');
    VERIFY_EQUALS(board.getSquare("c2"), 'P');
    VERIFY_EQUALS(board.getSquare("b3"), ' ');
    VERIFY_EQUALS(board.getSquare("f6"), ' ');
    VERIFY_EQUALS(board.getSquare("e7"), 'p');
    VERIFY_EQUALS(board.getSquare("c8"), 'b');
    VERIFY_EQUALS(board.getSquare("a8"), 'r');
    VERIFY_EQUALS(board.getSquare("e8"), 'k');
    VERIFY_EQUALS(board.getSquare("g8"), 'n');
  }
  Board board2("6kR/pppq1rB1/n2pr3/3Pp3/1PP3Q1/P3P3/6K1/7R b - - 0 29");
  Board board3("r1bqkbnr/ppppp1pp/2n5/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
  TEST_END
}

TEST_PROCEDURE(Board_fen_constructor_invalid_fens) {
  TEST_START
  std::vector<std::string> wrong_fens = {
    "",
    "anbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/ppppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/ppppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR c KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkp - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq j2 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - d 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 q"
  };

  for (const auto& fen: wrong_fens) {
    bool exception_was_thrown = false;
    try {
      Board board(fen);
    } catch(Board::WrongFENException& e) {
      exception_was_thrown = true;
    }
    if (exception_was_thrown == false) {
      NOT_REACHED(std::string("Exception WrongFENException was not thrown for fen \"") + fen + "\"");
    }
  }
  TEST_END
}

} // unnamed namespace
