#include <cassert>
#include <iostream>

#include "Board.h"
#include "Engine.h"
#include "MoveCalculator.h"


int main() {
  Engine engine;
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  bool cont = true;
  while (cont) {
    Move move = engine.calculateBestMove(board, 2);
    std::cout << move << std::endl;
    board = move.board;
    cont = !move.mate && !move.stalemate && !move.insufficient_material;
  }
  return 0;
}
