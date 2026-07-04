#include "core/Game.h"

int main() {
    Game game;
    if (!game.init("Guariteiro da Meia-Noite", 1280, 720))
        return -1;
    game.run();
    return 0;
}
