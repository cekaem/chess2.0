#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>

#include "Board.h"
#include "Engine.h"
#include "MoveCalculator.h"

namespace {

class PGNCreator {
 public:
  enum class GameResult {
    WHITE_WON,
    BLACK_WON,
    DRAW
  };

  PGNCreator(std::ostream& output_stream) : output_stream_(output_stream) {}

  void onMoveMade(const Move& move) {
    std::stringstream move_str;
    move_str << move << std::endl;
    moves_.push_back(move_str.str());
  }

  void onGameFinished(GameResult result) {
    if (moves_.empty()) {
      return;
    }
    std::string result_string;
    switch (result) {
      case GameResult::WHITE_WON:
        result_string = "1-0";
        break;
      case GameResult::BLACK_WON:
        result_string = "0-1";
        break;
      case GameResult::DRAW:
        result_string = "1/2-1/2";
        break;
    }
    output_stream_ << "[Result \"" << result_string << "\"]" << std::endl;
    for (const auto& move: moves_) {
      output_stream_ << move;
    }
    output_stream_ << result_string << std::endl;
  }

 private:
  std::ostream& output_stream_;
  std::vector<std::string> moves_;
};

}  // unnamed namespace

int main() {
  PGNCreator pgn_creator(std::cout);
  Engine engine(6, 5000);
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  bool cont = true;
  bool was_mate = false;
  while (cont) {
    try {
      auto start_time = std::chrono::steady_clock::now();
      Move move = engine.calculateBestMove(board);
      pgn_creator.onMoveMade(move);
      auto end_time = std::chrono::steady_clock::now();
      auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
      std::cout << move << " (" << time_elapsed << "ms)" << std::endl;
      board = move.board;
      cont = cont && !move.insufficient_material;
    } catch (Engine::NoValidMoveException&) {
      cont = false;
      was_mate = true;
    }
  }
  PGNCreator::GameResult result = PGNCreator::GameResult::DRAW;
  if (was_mate) {
    result = board.whiteToMove() ? PGNCreator::GameResult::BLACK_WON : PGNCreator::GameResult::WHITE_WON;
  }
  pgn_creator.onGameFinished(result);
  return 0;
}
