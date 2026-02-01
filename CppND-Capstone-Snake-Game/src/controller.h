#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Controller {
 public:
  void HandleUserInput(bool &running, Snake &snake) const;

 private:
  void HandleInput(bool &running, Snake &snake, SDL_Event &e) const;  // overloaded for keyboard or event input
  void HandleInput(Snake &snake, const char *text, char *input) const; // overloaded for text input
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif