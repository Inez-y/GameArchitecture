#include <SDL3/SDL_main.h>
#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init("Last Carriage", 1280, 720, false)) {
        return 1;
    }

    game.run();
    return 0;
}