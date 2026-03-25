#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    // placeholder map is 960x940 -> Currently the game is smaller than the map
    if (!game.init("Last Carriage", 800, 600)) {
        return 1;
    }

    game.run();
    game.clean();

    return 0;
}