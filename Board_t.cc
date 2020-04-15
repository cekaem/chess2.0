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
    VERIFY_TRUE(board.canCastle(Castling::Q));
    VERIFY_TRUE(board.canCastle(Castling::q));
    VERIFY_TRUE(board.canCastle(Castling::K));
    VERIFY_TRUE(board.canCastle(Castling::k));
  }
  {
    Board board("6kR/pppq1rB1/n2pr3/3Pp3/1PP3Q1/P3P3/6K1/7R b - - 0 29");
    VERIFY_FALSE(board.canCastle(Castling::Q));
    VERIFY_FALSE(board.canCastle(Castling::q));
    VERIFY_FALSE(board.canCastle(Castling::K));
    VERIFY_FALSE(board.canCastle(Castling::k));
  }
  {
    Board board("r1bqkbnr/ppppp1pp/2n5/4Pp2/8/8/PPPP1PPP/RNBQKBNR w Kq f6 0 3");
    VERIFY_FALSE(board.canCastle(Castling::Q));
    VERIFY_TRUE(board.canCastle(Castling::q));
    VERIFY_TRUE(board.canCastle(Castling::K));
    VERIFY_FALSE(board.canCastle(Castling::k));
  }
  TEST_END
}

TEST_PROCEDURE(Board_fen_constructor_invalid_fens) {
  TEST_START
  std::vector<std::string> invalid_fens = {
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

  for (const auto& fen: invalid_fens) {
    bool exception_was_thrown = false;
    try {
      Board board(fen);
    } catch(Board::InvalidFENException& e) {
      exception_was_thrown = true;
    }
    if (exception_was_thrown == false) {
      NOT_REACHED(std::string("Exception InvalidFENException was not thrown for fen \"") + fen + "\"");
    }
  }
  TEST_END
}

TEST_PROCEDURE(Board_getSquare_invalid_value) {
  TEST_START
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  std::vector<std::string> invalid_squares = {"i1", "a9", "invalid", "q", ""};

  for (const auto& square: invalid_squares) {
    bool exception_was_thrown = false;
    try {
      board.getSquare(square);
    } catch(Board::InvalidSquareException& e) {
      exception_was_thrown = true;
    }
    if (exception_was_thrown == false) {
      NOT_REACHED(std::string("Exception InvalidFENException was not thrown for square \"") + square + "\"");
    }
  }
  TEST_END
}

TEST_PROCEDURE(Board_indexing) {
  TEST_START
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  VERIFY_EQUALS(board[0][0], 'R');
  VERIFY_EQUALS(board[0][1], 'P');
  VERIFY_EQUALS(board[1][0], 'N');
  VERIFY_EQUALS(board[2][2], ' ');
  VERIFY_EQUALS(board[3][6], 'p');
  VERIFY_EQUALS(board[7][7], 'r');
  board[2][2] = 'q';
  VERIFY_EQUALS(board[2][2], 'q');
  TEST_END
}

TEST_PROCEDURE(Board_createFEN) {
  TEST_START
  std::vector<std::string> fens = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
    "6kR/pppq1rB1/n2pr3/3Pp3/1PP3Q1/P3P3/6K1/7R b - - 0 29"
  };

  for (const auto& fen: fens) {
    Board board(fen);
    VERIFY_EQUALS(board.createFEN(), fen);
  }
  TEST_END
}

} // unnamed namespace
