#include "Engine.h"

#include <cassert>
#include <cstdlib>
#include <ctime>

#include <iostream>

#include "utils/Timer.h"

namespace {

constexpr int kQueenValue = 9;
constexpr int kRookValue = 5;
constexpr int kBishopValue = 3;
constexpr int kKnightValue = 3;
constexpr int kPawnValue = 1;

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
  EngineMove(const Move& m, float eval)
    : move_(m), evaluation_(eval) {}

  Move move_;
  std::vector<EngineMove> children_;
  int evaluation_{0};
  int moves_to_mate_{0};
};

Engine::Engine(size_t depth, size_t time_for_move_ms)
 : depth_(depth), time_for_move_ms_(time_for_move_ms) {
  srand(static_cast<unsigned int>(clock()));
}

void Engine::findBorderValuesInChildren(
    const EngineMove& move,
    int& the_biggest_value,
    int& the_lowest_value,
    int& the_biggest_negative_value,
    int& the_lowest_positive_value,
    bool& is_move_without_mate) const {
  the_biggest_value = -10000;
  the_lowest_value = 10000;
  the_biggest_negative_value = -10000;
  the_lowest_positive_value = 10000;
  is_move_without_mate = false;
  for (const auto& child: move.children_) {
    const int mate_in = child.moves_to_mate_;
    if (mate_in == 0) {
      is_move_without_mate = true;
    } else if (mate_in > 0 && mate_in < the_lowest_positive_value) {
      the_lowest_positive_value = mate_in;
    } else if (mate_in < 0 && mate_in > the_biggest_negative_value) {
      the_biggest_negative_value = mate_in;
    }
    if (mate_in > the_biggest_value) {
      the_biggest_value = mate_in;
    }
    if (mate_in < the_lowest_value) {
      the_lowest_value = mate_in;
    }
  }
  if (the_biggest_negative_value == -10000) {
    the_biggest_negative_value = 0;
  }
  if (the_lowest_positive_value == 10000) {
    the_lowest_positive_value = 0;
  }
}

void Engine::updateMovesToMate(EngineMove& move) const {
  int the_biggest_value;
  int the_lowest_value;
  int the_biggest_negative_value;
  int the_lowest_positive_value;
  bool is_move_without_mate;
  findBorderValuesInChildren(move,
                             the_biggest_value,
                             the_lowest_value,
                             the_biggest_negative_value,
                             the_lowest_positive_value,
                             is_move_without_mate);

  const bool white_to_move = move.move_.board.whiteToMove();
  if (white_to_move) {
    if (the_lowest_positive_value > 0) {
      move.moves_to_mate_ = the_lowest_positive_value + 1;
    } else if (is_move_without_mate) {
      move.moves_to_mate_ = 0;
    } else {
      assert(the_lowest_value < 0);
      move.moves_to_mate_ = the_lowest_value - 1;
    }
  } else {
    if (the_biggest_negative_value < 0) {
      move.moves_to_mate_ = the_biggest_negative_value - 1;
    } else if (is_move_without_mate) {
       move.moves_to_mate_ = 0;
    } else {
      assert(the_biggest_value > 0);
      move.moves_to_mate_ = the_biggest_value + 1;
    }
  }
}

void Engine::evaluateMove(EngineMove& engine_move) const {
  if (engine_move.moves_to_mate_ != 0) {
    return;
  }
  if (engine_move.children_.empty()) {
    MoveCalculator calculator(engine_move.move_.board);
    auto moves = calculator.calculateAllMoves();
    if (moves.empty()) {
      if (calculator.isCheck()) {
        engine_move.moves_to_mate_ = engine_move.move_.board.whiteToMove() ? -1 : 1;
      } else {
        // stalemate
        engine_move.evaluation_ = 0;
      }
    }
    engine_move.children_.reserve(moves.size());
    for (Move& move: moves) {
      float eval = calculateMoveEvaluation(move);
      EngineMove new_move(move, eval);
      engine_move.children_.push_back(new_move);
    }
  } else if (!time_out_) {
    for (EngineMove& child: engine_move.children_) {
      evaluateMove(child);
    }
  }
  if (!engine_move.children_.empty()) {
    updateMovesToMate(engine_move);
    updateBestEvaluation(engine_move);
  }
}


void Engine::updateBestEvaluation(EngineMove& move) const {
  bool white_to_move = move.move_.board.whiteToMove();
  float best_move_value = white_to_move ? -10000.0 : 10000.0;
  for (auto& child: move.children_) {
    float move_value = child.evaluation_;
    if ((white_to_move && move_value > best_move_value) ||
        (!white_to_move && move_value < best_move_value)) {
      best_move_value = move_value;
    }
  }
  move.evaluation_ = best_move_value;
}

float Engine::calculateMoveEvaluation(const Move& move) const {
  ++nodes_calculated_;
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

Move Engine::findBestMove(const EngineMove& parent) const {
  float best_evaluation = parent.evaluation_;
  std::vector<Move> best_moves;
  int shift = parent.move_.board.whiteToMove() ? 1 : -1;
  for (const EngineMove& child: parent.children_) {
    if (parent.moves_to_mate_ != 0) {
      if (child.moves_to_mate_ + shift == parent.moves_to_mate_) {
        best_moves.push_back(child.move_);
      }
    } else if (child.evaluation_ == best_evaluation) {
      best_moves.push_back(child.move_);
    }
  }
  size_t index = generateRandomValue(best_moves.size());
  return best_moves[index];
}

void Engine::timerCallback() {
  time_out_ = true;
}

Move Engine::calculateBestMove(const Board& board) {
  utils::Timer timer;
  nodes_calculated_ = 0ull;
  Move move(board, 0, 0, 0, 0);
  EngineMove root(move, 0.0);
  time_out_ = false;
  timer.start(time_for_move_ms_, std::bind(&Engine::timerCallback, this));
  for (size_t i = 0; i < depth_; ++i) {
    evaluateMove(root);
  }
  timer.stop();
  if (root.children_.empty()) {
    throw NoValidMoveException(board.createFEN());
  }
  return findBestMove(root);
}
