#include "Engine.h"

#include <cassert>
#include <cstdlib>
#include <ctime>

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

Engine::Engine() {
  srand(static_cast<unsigned int>(clock()));
}

void Engine::calculateMoveEvaluation(Move& move) const {
  int result = 0;
  for (size_t line = 0; line < Board::kBoardSize; ++line) {
    for (size_t row = 0; row < Board::kBoardSize; ++row) {
      char figure = move.board.at(line, row);
      if (figure == 0x0) {
        continue;
      }
      result += getFigureValue(figure);
    }
  }
  move.evaluation = result;
}

std::pair<int, size_t> Engine::calculateBestEvaluation(std::vector<Move>& moves) const {
  size_t number_of_best_moves = 0ul;
  bool white_to_move = moves[0].board.whiteToMove();
  int best_move_value = white_to_move ? -1000 : 1000;
  for (auto& move: moves) {
    calculateMoveEvaluation(move);
    int move_value = move.evaluation;
    if (move_value == best_move_value) {
      ++number_of_best_moves;
    }
    if ((white_to_move && move_value > best_move_value) ||
        (!white_to_move && move_value < best_move_value)) {
      best_move_value = move_value;
      number_of_best_moves = 1lu;
    }
  }
  return std::make_pair(best_move_value, number_of_best_moves);
}

size_t Engine::chooseRandomMove(const std::vector<Move>& moves,
                                std::pair<int, size_t> eval) const {
  unsigned which_appearance = generateRandomValue(eval.second);
  unsigned number_of_appearances = 0u;
  size_t index_of_chossen_move = 0lu;
  for (size_t index = 0; index < moves.size(); ++index) {
    if (moves[index].evaluation == eval.first) {
      if (number_of_appearances == which_appearance) {
        index_of_chossen_move = index;
        break;
      }
      ++number_of_appearances;
    }
  }
  return index_of_chossen_move;
}

Move Engine::calculateBestMove(const Board& board) {
  MoveCalculator calculator(board);
  auto moves = calculator.calculateAllMoves();
  if (moves.empty()) {
    throw NoValidMoveException(board.createFEN());
  }
  auto best_evaluation = calculateBestEvaluation(moves);
  size_t index = chooseRandomMove(moves, best_evaluation);
  assert(index < moves.size());
  return moves[index];
}
