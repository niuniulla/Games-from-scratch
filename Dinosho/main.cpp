#include "Game.hpp"

int main()
{
    Game game;
    if (game.init(256, 160, 6, 6))
        game.execute();
    return 0;
}