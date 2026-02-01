#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}

// Handle Text inputs: numbers from 0 to 9 
void Controller::HandleInput(Snake &snake, const char *text, char *input) const {
    if (text[0] >= '0' && text[0] <= '9') {  // only digits
        if (strlen(input) < 31) {
            strcat(input, text);
        }

        int init_speed_level = atoi(input);
        if (init_speed_level != 0) {
            snake.speed = snake.default_speed + init_speed_level * 0.01;
        } else {
            snake.speed = snake.default_speed;
        }

        printf("User set initial speed level: %s\n", input);
        printf("Init speed level is set to: %.2f\n", snake.speed);
    }
}

// Handle Event/Keyboard inputs: Down, Up, Right, Left 
void Controller::HandleInput(bool &running, Snake &snake, SDL_Event &e) const {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                ChangeDirection(snake, Snake::Direction::kUp, Snake::Direction::kDown);
                break;
            case SDLK_DOWN:
                ChangeDirection(snake, Snake::Direction::kDown, Snake::Direction::kUp);
                break;
            case SDLK_LEFT:
                ChangeDirection(snake, Snake::Direction::kLeft, Snake::Direction::kRight);
                break;
            case SDLK_RIGHT:
                ChangeDirection(snake, Snake::Direction::kRight, Snake::Direction::kLeft);
                break;
        }
    } else if (e.type == SDL_QUIT) {
        running = false;
    }
}

void Controller::HandleUserInput(bool &running, Snake &snake) const {
    SDL_Event e;
    char input[32] = "";

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_TEXTINPUT) {
            HandleInput(snake, e.text.text, input);
        } else {
            HandleInput(running, snake, e);  // overloaded function handles keyboard & quit
        }
    }
}
