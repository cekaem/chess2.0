#include <cassert>
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
    move_str << move;
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
      if (result != GameResult::DRAW) {
        output_stream_ << "#";
      }
      output_stream_ << std::endl;
    }
    output_stream_ << result_string << std::endl;
  }

 private:
  std::ostream& output_stream_;
  std::vector<std::string> moves_;
};

void statsCollector(Engine::MoveStats stats) {
  std::cerr << stats.move << std::endl;
  std::cerr << "Time elapsed (ms): " << stats.time_ms << std::endl;
  std::cerr << "Nodes calculated : " << stats.nodes << std::endl;
  std::cerr << "Reached depth    : " << stats.depth << std::endl;
  std::cerr << "==========================" << std::endl;
}

}  // unnamed namespace

int main() {
  PGNCreator pgn_creator(std::cout);
  Engine engine(6, 5000);
  engine.setStatsCallback(statsCollector);
  Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  bool cont = true;
  bool was_mate = false;

  while (cont) {
    try {
      Move move = engine.calculateBestMove(board);
      pgn_creator.onMoveMade(move);
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
