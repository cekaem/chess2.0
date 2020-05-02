#include "Engine.h"

#include <cassert>
#include <cstdlib>
#include <ctime>

#include <iostream>

namespace {

constexpr int kQueenValue = 9;
constexpr int kRookValue = 5;
constexpr int kBishopValue = 3;
constexpr int kKnightValue = 3;
constexpr int kPawnValue = 1;

constexpr int kMateEvaluation = 1000;

// Generates random value out of [0, max)
unsigned generateRandomValue(int max) {
  return rand() % max;
}

int getFigureValue(char figure) {
  switch(figure) {
    case 'Q':
      return kQueenValue;
    case 'q':
      return -kQueenValue;
    case 'R':
      return kRookValue;
    case 'r':
      return -kRookValue;
    case 'B':
      return kBishopValue;
    case 'b':
      return -kBishopValue;
    case 'N':
      return kKnightValue;
    case 'n':
      return -kKnightValue;
    case 'P':
      return kPawnValue;
    case 'p':
      return -kPawnValue;
    case 'K':
    case 'k':
      return 0;
    default:
      assert(!"Unkown figure");
      break;
  }
  return 0;
}

}  // unnamed namespace


struct EngineMove {
  EngineMove(const Move& m, EngineMove* p, float eval) :
    move(m), parent(p), evaluation(eval) {
  }

  void updateBestEvaluation() {
    bool white_to_move = move.board.whiteToMove();
    float best_move_value = white_to_move ? -10000.0 : 10000.0;
    for (auto& move: children) {
      float move_value = move.evaluation;
      if ((white_to_move && move_value > best_move_value) ||
          (!white_to_move && move_value < best_move_value)) {
        best_move_value = move_value;
      }
    }
    evaluation = best_move_value;
  }

  Move move;
  std::vector<EngineMove> children;
  EngineMove* parent{nullptr};
  int evaluation{0};
  int moves_to_mate{0};
};

Engine::Engine() {
  srand(static_cast<unsigned int>(clock()));
}

float Engine::calculateMoveEvaluation(const Move& move) const {
  float result = 0.0;
  for (size_t line = 0; line < Board::kBoardSize; ++line) {
    for (size_t row = 0; row < Board::kBoardSize; ++row) {
      char figure = move.board.at(line, row);
      if (figure == 0x0) {
        continue;
      }
      result += getFigureValue(figure);
    }
  }
  return result;
}

Move Engine::findBestMove(const EngineMove& move) const {
  float best_evaluation = move.evaluation;
  std::vector<Move> best_moves;
  for (const EngineMove& child: move.children) {
    if (child.evaluation == best_evaluation) {
      best_moves.push_back(child.move);
    }
  }
  size_t index = generateRandomValue(best_moves.size());
  return best_moves[index];
}

void Engine::evaluateMove(EngineMove& engine_move) const {
  if (engine_move.children.empty()) {
    MoveCalculator calculator(engine_move.move.board);
    auto moves = calculator.calculateAllMoves();
    engine_move.children.reserve(moves.size());
    for (Move& move: moves) {
      float eval = calculateMoveEvaluation(move);
      EngineMove e_move(move, &engine_move, eval);
      engine_move.children.push_back(e_move);
    }
  } else {
    for (EngineMove& child: engine_move.children) {
      evaluateMove(child);
    }
  }
  engine_move.updateBestEvaluation();
}

Move Engine::calculateBestMove(const Board& board) {
  we_play_white_ = board.whiteToMove();
  Move move(board, 0, 0, 0, 0);
  EngineMove root(move, nullptr, 0.0);
  evaluateMove(root);
  return findBestMove(root);
}
