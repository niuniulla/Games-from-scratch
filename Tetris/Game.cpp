#include "Game.hpp"

Game::Game() :
    _mask {0}
{
    // create tetrominos
    // draw in a grid
    //   _________   _________  _________  _________  _________  _________  _________ 
    //   |_|_|x|_|   |_|_|_|_|  |_|x|_|_|  |_|x|x|_|  |_|x|x|_|  |_|x|_|_|  |_|_|x|_| 
    //   |_|_|x|_|   |_|x|x|_|  |_|x|x|_|  |_|x|_|_|  |_|_|x|_|  |_|x|x|_|  |_|x|x|_|
    //   |_|_|x|_|   |_|x|x|_|  |_|x|_|_|  |_|x|_|_|  |_|_|x|_|  |_|_|x|_|  |_|x|_|_|
    //   |_|_|x|_|   |_|_|_|_|  |_|_|_|_|  |_|_|_|_|  |_|_|_|_|  |_|_|_|_|  |_|_|_|_|   

    _tetrominos = {
        "..X...X...X...X.",
        ".....XX..XX.....",
        ".X...XX..X......",                
        ".XX..X...X......",
        "..XX...X...X....",
        ".X...XX...X.....",
        "..X..XX..X......"
    };

}

Game::~Game()
{
    // SDL_DestroyRenderer(_pRenderer);
    // SDL_DestroyWindow(_pWindow);
    // _pWindow = nullptr;
    // _pRenderer = nullptr;
    // TTF_Quit();
    // SDL_Quit();
}

/**
 * init the SDL stuffs and somme constants.
 *
 * This initialize all SDL related stuffs such as window, renderer, 
 * font for rendering. 
 *
 * @return true if everything was initialized correctly.
 */
bool Game::init_sdl()
{
    _isRunning = true;
    _pRenderer = nullptr;
    _pWindow = nullptr;


    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL cannot initialize. SDL_Error :" << SDL_GetError() << std::endl;
        return false;
    }

    // initialize SDL_TTF
    if (TTF_Init() < 0)
    {
        std::cout << "Error initializing SDL ttf" << TTF_GetError() << std::endl;
    }

    _pTextFont = TTF_OpenFont(_fontFile.c_str(), 20);
    if (!_pTextFont)
    {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }
    // font size
    _fontSize = 20;

    // line height
    _lineSkip = TTF_FontLineSkip(_pTextFont);

    // create window
    _pWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                _screenWidth, _screenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    if (!_pWindow)
    {
        std::cout << "SDL window cannot be created. SDL_Error :" << SDL_GetError() << std::endl;
        return false;
    }
    else
    {
        // create renderer
        _pRenderer = SDL_CreateRenderer(_pWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

        std::cout << "DEBUG1 - renderer finished." << std::endl;

        // load font
        _pTextFont = TTF_OpenFont(_fontFile.c_str(), 20);
        if (!_pTextFont)
        {
            std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
            return false;
        }

        // set bg color
        SDL_SetRenderDrawColor(_pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(_pRenderer);

        // update window drawing
        SDL_RenderPresent(_pRenderer);
        std::cout << "DEBUG3 - finished init." << std::endl;

    }
    return true;
}

/**
 * game main loop.
 *
 * all game logics were run here.
 *
 */
void Game::run()
{
    // initialize sdl
    if (!init_sdl())
    {
        std::cerr << "Failed to initialize the SDL. MSG: " << SDL_GetError() << std::endl;
        return;
    }

    // declare and initialize all parameters related to game
    bool game_on = true;
    bool quit = false;
    
    // game loop
    while(!quit)
    {
        /*
            reinitialize game state
        */ 
        int current_posX = 6; // create at 6, 0 on the top of the game field
        int current_posY = 0;
        int current_piece = std::rand() % 7;
        int current_rot = 0;
        int speed = 20;
        int numFullLine = 0;
        int score = 0;
        SDL_Color piece_color = {static_cast<Uint8>(std::rand()%255), 
                          static_cast<Uint8>(std::rand()%255), 
                            static_cast<Uint8>(std::rand()%255)};;
        int fullLineCount = 0;
        Uint32 start_tick = 0;
        int loop_count = 0;
        float cap = 0.0;

        /*
            initialize mask of the game board
            border is 9, empty game space 0
        */
        _mask = {0};
        for (int i=0; i<_field_height*_field_width; ++i)
        {
            if (i % _field_width == 0 || i % _field_width == (_field_width-1) || i > ((_field_height-1)*_field_width))
            {
                _mask[i] = 9;   
            }         
        }

        /*
            message to restart the game
        */
        SDL_SetRenderDrawColor(_pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(_pRenderer);
        SDL_Color color {255, 255, 255};
        std::string text = "Game Over";
        drawText(_pRenderer, text.c_str(), 1, 15, color);
        text = "Press Enter to restart";
        drawText(_pRenderer, text.c_str(), 1, 16, color);
        SDL_RenderPresent(_pRenderer);

        // wait for user to restart 
        SDL_Event event;
        if (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                std::cout << "closing" << std::endl;
                quit = true;
            }
            if (event.key.keysym.sym == SDLK_RETURN)
            {
                game_on = true;                
            }
        }    

        /*
            game main loop
        */
        while(game_on)
        {
            loop_count ++;
            SDL_Delay(50);

            // if no collision, then move down the current piece
            if (!detectCollision(_tetrominos[current_piece], current_rot, current_posX, current_posY+1, _mask))
            {
                if (loop_count >= speed)
                {
                    current_posY += 1;
                    loop_count = 0;
                }      
            }     
            else
            {
                // reset the piece color to draw
                piece_color = SDL_Color{static_cast<Uint8>(std::rand()%255), 
                            static_cast<Uint8>(std::rand()%255), 
                            static_cast<Uint8>(std::rand()%255)};

                if (current_posY == 0)
                {
                    game_on = false;
                }

                // freeze piece when reach the bottom of the board
                for (int i=0; i<_piece_size; ++i)
                {
                    for (int j=0; j<_piece_size; ++j)
                    {
                        if (_tetrominos[current_piece][rot(current_rot, j, i)] != '.')
                        {  
                            _mask[(current_posY+i)*_field_width+current_posX+j] = current_piece+1;
                            continue;
                        }
                    }        
                }

                // check full line
                for (int i=1; i<_field_height-1; ++i)
                {
                    bool full = true;
                    for (int j=1; j<_field_width-1; ++j)
                    {
                        full = full && (_mask[i*_field_width+j] > 0);   
                    }
                    if (full)
                    {
                        //print("detected a full line.", i);
                        ++numFullLine;
                        fullLineCount++;
                        for (int j=1; j<_field_width-1; ++j)
                            _mask[i*_field_width+j] = 0;
                    }      
                }

                // update score
                score += pow(10, numFullLine);

                //remove all full lines
                while (numFullLine>0)
                {
                    for (int i=_field_height-1; i>0; --i)
                    {
                        for (int j=1; j<_field_width-1; ++j)
                        {
                            if (_mask[i*_field_width+j] < _mask[(i-1)*_field_width+j])
                            {
                                _mask[i*_field_width+j] = _mask[(i-1)*_field_width+j];
                                _mask[(i-1)*_field_width+j] = 0;
                            }
                        }  
                    }
                    --numFullLine;
                }

                // new piece
                current_posX = 6;
                current_posY = 0;
                current_piece = std::rand() % 7;
                current_rot = std::rand() % 4;
            }

            
            // check key event
            if (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    std::cout << "closing" << std::endl;
                    game_on = false;
                    quit = true;
                }

                if (event.type == SDL_KEYDOWN)
                {
                    switch(event.key.keysym.sym)
                    {
                        case (SDLK_LEFT):
                            if (!detectCollision(_tetrominos[current_piece], current_rot, current_posX-1, current_posY, _mask))
                            {
                                --current_posX;
                            } 
                            break;      
                            
                        case (SDLK_RIGHT):
                            if (!detectCollision(_tetrominos[current_piece], current_rot, current_posX+1, current_posY, _mask))
                            {
                                ++current_posX;
                            }   
                            break; 

                        case (SDLK_DOWN):
                            if (!detectCollision(_tetrominos[current_piece], current_rot, current_posX, current_posY+1, _mask))
                            {
                                ++current_posY;
                            }    
                            break;  

                        case (SDLK_SPACE):
                            if (!detectCollision(_tetrominos[current_piece], (current_rot+1)%4, current_posX, current_posY, _mask))
                            {
                                current_rot = (current_rot + 1) % 4;
                            }  
                            break;  
                            
                        default:
                            break;
                    }
                } 
            }
                
            //clear the screen
            // set bg color
            SDL_SetRenderDrawColor(_pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(_pRenderer);
            
            //draw game board
            SDL_Color color {255, 255, 255};
            for (int l=0; l<_field_height; l++)
            {
                std::string str = "";
                for (int c=0; c<_field_width; c++)
                {
                    if (_mask[l*_field_width+c] > 0)
                        drawSquare(c+_screen_shiftX, l+_screen_shiftY, color);
                }  
    
            }

            // draw current piece
            for (int i=0; i<_piece_size; ++i)
            {
                for (int j=0; j<_piece_size; ++j)
                {
                    if (_tetrominos[current_piece][rot(current_rot, j, i)] != '.')
                    {
                        drawSquare(j+current_posX+_screen_shiftX, i+current_posY+_screen_shiftY, piece_color);
                    }  
                }        
            }

            // draw text
            Uint32 end_tock = SDL_GetTicks();
            std::string scoretext = "Score: " + std::to_string(score);
            std::string linetext = "Lines: " + std::to_string(fullLineCount);
            std::string timetext = "Time: " + std::to_string((end_tock-start_tick)/1000);
            drawText(_pRenderer, scoretext, 1, 2, color);
            drawText(_pRenderer, linetext, 1, 3, color);
            drawText(_pRenderer, timetext, 1, 4, color);
            SDL_RenderPresent(_pRenderer);
        }

    }
        
}

/**
 * rotate the pieces.
 *
 * the rotation is based on the indices order. 
 * We don't change the memory location of the piece values.
 * The indices are obtained using the below shema with counter-clock wise rotation.
 * 
 *    0  1  2  3        0  1  2  3          0  1  2  3          0  1  2  3
 *   -------------     -------------       -------------       -------------
 * 0 |0 |1 |2 |3 |   0 |3 |7 |11|15|     0 |15|14|13|12|     0 |12|8 |4 |0 |
 *   -------------     -------------       -------------       -------------
 * 1 |4 |5 |6 |7 |   1 |2 |6 |10|14|     1 |11|10|9 |8 |     1 |13|9 |5 |1 |
 *   -------------     -------------       -------------       -------------
 * 2 |8 |9 |10|11|   2 |1 |5 |9 |13|     2 |7 |6 |5 |4 |     2 |14|10|6 |2 |
 *   -------------     -------------       -------------       -------------
 * 3 |12|13|14|15|   3 |0 |4 |8 |12|     3 |3 |2 |1 |0 |     3 |15|11|7 |3 |
 *   -------------     -------------       -------------       -------------
 * 
 * @return the rotated position of the piece value corresponding to the rotation angle.
 */
int Game::rot(int r, int x, int y)
{ 
    switch(r)
    {
        case 0: return y * _piece_size + x; // 0 deg
        case 1: return (_piece_size - y - 1) + _piece_size * x; // 90 deg
        case 2: return (_piece_size - y) * _piece_size - 1 - x; // 180 deg
        case 3: return _piece_size * (_piece_size - 1) + y - x * _piece_size; // 270 deg
    }
    return 0;
}


/**
 * collision detection
 *
 * detect if the piece collides with the border of the board.
 * 
 * @return true if detected collision.
*/ 
bool Game::detectCollision(std::string piece, int r, int x, int y, std::array<int, _field_height * _field_width> &mask)
{
    for (int i=0; i<_piece_size; ++i)
    {
        for (int j=0; j<_piece_size; ++j)
        {
            if ((piece[rot(r, j, i)] != '.') && (mask[(y+i)*_field_width+x+j] != 0))
                return 1;
        }        
    }
    return 0;
}


/**
 * draw text on renderer
 *
*/ 
bool Game::drawText(SDL_Renderer *pRenderer, const std::string str, int x, int y, SDL_Color color)
{
    SDL_Surface *TextSurface = TTF_RenderText_Solid(_pTextFont, str.c_str(), color);

    // create texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(pRenderer, TextSurface);

    // init render rect
    SDL_Rect dest;
    dest = {x*_fontSize, y*_lineSkip, (static_cast<int>(str.size())-1)*_fontSize, _lineSkip}; 

    // set the rect to render on renderer, 
    // the texture will be squeezed in to this area
    SDL_RenderCopy(pRenderer, texture, NULL, &dest); 

    // free window surface
    SDL_FreeSurface(TextSurface);

    return true;
}

/**
 * draw a square on renderer
 *
*/ 
void Game::drawSquare(int x, int y, const SDL_Color color)
{
    SDL_Rect rect;
    rect.x = x * _pixel_sizeX;
    rect.y = y * _pixel_sizeY;
    rect.w = 20;
    rect.h = 20;

    SDL_SetRenderDrawColor(_pRenderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(_pRenderer, &rect);
}