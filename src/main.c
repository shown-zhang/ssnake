#include <unistd.h>

#include "core/state.h"

int main(int argc, char **argv) {
  GameState state;
  init_game_state(&state);
  init_snake(&state);

  while (!state.is_over) {
    listen_input(&state);
    move_snake(&state);
    update_grid(&state);
    print_game_state(&state);
    // print_snake(&state);
    sleep(1);
  }
  clean_snake(&state);
  return 0;
}