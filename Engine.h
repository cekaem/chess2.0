#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <utility>
#include <vector>

#include "MoveCalculator.h"


class Engine {
 public:
  struct NoValidMoveException {
    NoValidMoveException(const std::string& f) : fen(f) {}
    const std::string fen;
  };

  Engine();
  Move calculateBestMove(const Board& board);

 private:
  void calculateMoveEvaluation(Move& move) const;
  std::pair<int, size_t> calculateBestEvaluation(std::vector<Move>& moves) const;
  size_t chooseRandomMove(const std::vector<Move>& moves, std::pair<int, size_t> eval) const;
};

#endif // ENGINE_H
