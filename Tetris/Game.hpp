#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cmath>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"


class Game
{
    public:
        const int _screen_shiftX {10}; // to shift the game board to the middle of the window
        const int _screen_shiftY {10};
        static const int _field_width {12}; // size of the game board
        static const int _field_height {18};
        static const int _piece_size = 4; //the piece is defined using a grid of size 4
        static const int _pixel_sizeX = 20; //we fixed the size of the drawing
        static const int _pixel_sizeY = 20;

    public:
        Game();
        ~Game();
        bool init_sdl();
        void run();
        static int rot(int, int, int);
        static bool detectCollision(std::string, int, int, int, std::array<int, _field_height * _field_width>&);
        bool drawText(SDL_Renderer *pRenderer, const std::string str, int x, int y, SDL_Color color);
        void drawSquare(int x, int y, const SDL_Color color);

    private:

        std::array<int, _field_height * _field_width> _mask {0};
        std::array<std::string, 7> _tetrominos;

        bool _isRunning;
        SDL_Renderer *_pRenderer;
        SDL_Window *_pWindow;
        int _screenHeight = 800;
        int _screenWidth = 600;

        TTF_Font *_pTextFont;
        int _fontSize;
        int _lineSkip;
        const std::string _fontFile = "../monospace-medium.ttf";
        SDL_Color _textColor {255, 255, 255};
};
