#include "../src/core/game.h"
#include "tools/tools.h"

int main(int argc, char *argv[]) {
    Game& game = Game::getInstance();
    game.init();
    game.run();
    game.clean();
    return 0;
}