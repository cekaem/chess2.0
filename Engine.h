#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <utility>
#include <vector>

#include "MoveCalculator.h"

class EngineMove;

class Engine {
 public:
  struct NoValidMoveException {
    NoValidMoveException(const std::string& f) : fen(f) {}
    const std::string fen;
  };

  Engine();
  Move calculateBestMove(const Board& board);

 private:
  void evaluateMove(EngineMove& engine_move) const;
  Move findBestMove(const EngineMove& move) const;
  float calculateMoveEvaluation(const Move& move) const;

  bool we_play_white_;
};

#endif // ENGINE_H
