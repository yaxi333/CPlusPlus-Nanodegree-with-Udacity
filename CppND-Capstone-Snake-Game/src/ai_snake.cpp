#include "ai_snake.h"
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <cmath>

AISnake::AISnake(int grid_width, int grid_height) : Snake(grid_width, grid_height) {
  head_x = grid_width / 4;
  head_y = grid_height / 4;
  speed = 0.1f;
}

AISnake::~AISnake() {
  StopPathfinding();
}

void AISnake::StopPathfinding() {
  stop_thread = true;
  if (pathfinding_thread.joinable()) {
    pathfinding_thread.join();
  }
}

void AISnake::StartPathfinding(const SDL_Point &food, const Snake &player_snake, const SDL_Point &obstacle) {
  if (pathfinding_thread.joinable()) return;
  
  auto obstacles = std::make_shared<std::vector<SDL_Point>>();
  obstacles->push_back(obstacle);
  for (const auto &pt : player_snake.body) obstacles->push_back(pt);
  
  stop_thread = false;
  pathfinding_thread = std::thread(&AISnake::PathfindingWorker, this, food, obstacles);
}

void AISnake::PathfindingWorker(SDL_Point food, std::shared_ptr<std::vector<SDL_Point>> obstacles) {
  SDL_Point start = {static_cast<int>(head_x), static_cast<int>(head_y)};
  auto new_path = AStar(start, food, obstacles);
  
  std::lock_guard<std::mutex> lock(path_mutex);
  path = std::move(new_path);
}

void AISnake::UpdateAI(const SDL_Point &food, const Snake &player_snake, const SDL_Point &obstacle) {
  bool path_empty = false; 
  std::lock_guard<std::mutex> lock(path_mutex);
  if (!path.empty()) {
    direction = path.front();
    path.pop();
  }
  path_empty = path.empty();
      
  if (path_empty & pathfinding_thread.joinable()) {
      pathfinding_thread.join();
  }
  
  if (!pathfinding_thread.joinable()) {
    StartPathfinding(food, player_snake, obstacle);
  }
}

int AISnake::Heuristic(int x1, int y1, int x2, int y2) {
  return std::abs(x1 - x2) + std::abs(y1 - y2);
}

bool AISnake::IsValid(int x, int y, std::shared_ptr<std::vector<SDL_Point>> obstacles) {
  if (SnakeCell(x, y)) return false;
  for (const auto &obs : *obstacles) {
    if (obs.x == x && obs.y == y) return false;
  }
  return true;
}

std::queue<Snake::Direction> AISnake::AStar(const SDL_Point &start, const SDL_Point &goal, std::shared_ptr<std::vector<SDL_Point>> obstacles) {
  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
  std::set<std::pair<int, int>> closed;
  std::map<std::pair<int, int>, std::pair<int, int>> parent;
  
  open.push({start.x, start.y, 0, Heuristic(start.x, start.y, goal.x, goal.y)});
  
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};
  Snake::Direction dirs[] = {Snake::Direction::kUp, Snake::Direction::kDown, Snake::Direction::kLeft, Snake::Direction::kRight};
  
  while (!open.empty() && !stop_thread) {
    Node current = open.top();
    open.pop();
    
    if (current.x == goal.x && current.y == goal.y) {
      std::queue<Snake::Direction> path;
      int x = goal.x, y = goal.y;
      
      while (x != start.x || y != start.y) {
        auto prev = parent[{x, y}];
        for (int i = 0; i < 4; i++) {
          if (prev.first + dx[i] == x && prev.second + dy[i] == y) {
            path.push(dirs[i]);
            break;
          }
        }
        x = prev.first;
        y = prev.second;
      }
      
      std::queue<Snake::Direction> reversed;
      while (!path.empty()) {
        reversed.push(path.front());
        path.pop();
      }
      return reversed;
    }
    
    closed.insert({current.x, current.y});
    
    for (int i = 0; i < 4; i++) {
      int nx = current.x + dx[i];
      int ny = current.y + dy[i];
      
      if (closed.count({nx, ny}) || !IsValid(nx, ny, obstacles)) continue;
      
      open.push({nx, ny, current.g + 1, Heuristic(nx, ny, goal.x, goal.y)});
      parent[{nx, ny}] = {current.x, current.y};
    }
  }
  
  return std::queue<Snake::Direction>();
}
