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
    try {
      Move move = engine.calculateBestMove(board, 4);
      std::cout << move << std::endl;
      board = move.board;
      cont = cont && !move.insufficient_material;
    } catch (Engine::NoValidMoveException&) {
      cont = false;
    }
  }
  return 0;
}
