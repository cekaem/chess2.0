#include <cassert>
#include <iostream>

#include "Board.h"
#include "MoveCalculator.h"


int main() {
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  bool cont = true;
  while (cont) {
    MoveCalculator calculator(board);
    auto moves = calculator.calculateAllMoves();
    size_t number_of_moves = moves.size();
    assert(number_of_moves);
    Move move = moves[rand() % number_of_moves];
    std::cout << move << std::endl;
    board = move.board;
    cont = !move.mate && !move.stalemate && !move.insufficient_material;
  }
  return 0;
}
