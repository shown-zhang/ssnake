#include <unistd.h>

#include "core/state.h"

int main(int argc, char **argv) {
  GameState state;
  init_game_state(&state);
  init_snake(&state);

  while (!state.is_over) {
    move_snake(&state);
    update_grid(&state);
    print_game_state(&state);
    sleep(1);
  }
  clean_snake(&state);
  return 0;
}