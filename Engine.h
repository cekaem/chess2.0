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
  void updateBestEvaluation(EngineMove& move) const;
  void updateMovesToMate(EngineMove& move) const;
  void findBorderValuesInChildren(
      const EngineMove& move,
      int& the_biggest_value,
      int& the_lowest_value,
      int& the_biggest_negative_value,
      int& the_lowest_positive_value,
      bool& is_move_without_mate) const;
};

#endif // ENGINE_H
