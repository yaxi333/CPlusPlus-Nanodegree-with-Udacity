#ifndef AI_SNAKE_H
#define AI_SNAKE_H

#include "snake.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <memory>

class AISnake : public Snake {
public:
  AISnake(int grid_width, int grid_height);
  ~AISnake();
  
  void UpdateAI(const SDL_Point &food, const Snake &player_snake, const SDL_Point &obstacle);
  void StartPathfinding(const SDL_Point &food, const Snake &player_snake, const SDL_Point &obstacle);
  void StopPathfinding();

private:
  struct Node {
    int x, y, g, h;
    bool operator>(const Node &other) const { return (g + h) > (other.g + other.h); }
  };

  std::thread pathfinding_thread;
  std::mutex path_mutex;
  std::atomic<bool> stop_thread{false};
  std::queue<Snake::Direction> path;
  
  void PathfindingWorker(SDL_Point food, std::shared_ptr<std::vector<SDL_Point>> obstacles);
  std::queue<Snake::Direction> AStar(const SDL_Point &start, const SDL_Point &goal, std::shared_ptr<std::vector<SDL_Point>> obstacles);
  int Heuristic(int x1, int y1, int x2, int y2);
  bool IsValid(int x, int y, std::shared_ptr<std::vector<SDL_Point>> obstacles);
};

#endif
