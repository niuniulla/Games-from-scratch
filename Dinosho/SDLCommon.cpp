#include "SDLCommon.hpp"

#include <algorithm>


// constructor
SDLCommon::SDLCommon()
{
    _pTexture = nullptr;
}


// destructor
SDLCommon::~SDLCommon()
{
    SDL_FreeFormat(_format);
    SDL_FreeSurface(_pScreenSurface);
    SDL_FreeSurface(_pTextSurface);
    if (!_pTexture)
        SDL_DestroyTexture(_pTexture);
    if (!_pTextTexture)
        SDL_DestroyTexture(_pTextTexture);
    delete[] _backgroundPixels;
    delete[] _texturePixels;
    TTF_CloseFont(_pTextFont);
    _pTextFont = nullptr;

    SDL_DestroyRenderer(_pRenderer);
    SDL_DestroyWindow(_pWindow);
    _pWindow = nullptr;
    _pRenderer = nullptr;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

}


// init all sdl stuff and game related resources
bool SDLCommon::init(int w, int h, int px, int py)
{
    _pRenderer = nullptr;
    _pWindow = nullptr;

    _screenHeight = h;
    _screenWidth = w;
    _windowHeight = h * py;
    _windowWidth = w * px;
    _pixelSizeX = px;
    _pixelSizeY = py;

    // initialize SDL
    std::cout << "DEBUG - initialize SDL."  << std::endl;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL cannot initialize. SDL_Error :" << SDL_GetError() << std::endl;
        return false;
    }

    // create window
    _pWindow = SDL_CreateWindow(_appName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    if (!_pWindow)
    {
        std::cout << "SDL window cannot be created. SDL_Error :" << SDL_GetError() << std::endl;
        return false;
    }
    else
    {
        // create renderer
        _pRenderer = SDL_CreateRenderer(_pWindow, -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderSetLogicalSize(_pRenderer, _screenWidth, _screenHeight);

        // set bg color
        SDL_SetRenderDrawColor(_pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(_pRenderer);

        // update window drawing
        SDL_RenderPresent(_pRenderer);
    }

    // initialize SDL_TTF
    std::cout << "DEBUG - initialize TTF." << std::endl;
    if (TTF_Init() < 0)
    {
        std::cout << "Error initializing SDL ttf" << TTF_GetError() << std::endl;
    }

    // load font
    if (_fontFileName != nullptr)
    { 
        std::cout << "DEBUG - Opening :" << _fontFileName << std::endl;
       
        // font size
        _fontSizeX = 30;
        _pTextFont = TTF_OpenFont(_fontFileName, _fontSizeX);
    }
    else
    {
        std::cout << "WARNING - no font provided." << std::endl;
    }

    if (!_pTextFont)
    {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        //return false;
    }
    else
    {
        // line height
        _fontSizeY = TTF_FontLineSkip(_pTextFont);
    }
    
    _atomIsRunning = true;
    _frameStart = SDL_GetPerformanceCounter();
    _frameEnd = SDL_GetPerformanceCounter();

    // get color format
    _format = SDL_AllocFormat(SDL_GetWindowPixelFormat(_pWindow));

    //_pTextSurface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

    std::cout << "DEBUG - finished init." << std::endl;
    return true;
}


// game loop
void SDLCommon::execute()
{
    if (!onUserInit())
    {
        _atomIsRunning = false;
    }
    
    while(_atomIsRunning)
    {
        //clear the screen
        // set bg color
        SDL_SetRenderDrawColor(_pRenderer, 0, 255, 255, 255);
        SDL_RenderClear(_pRenderer);
        createTexture();
        resetTextTexture();
        
        //Handle Timing
        _frameEnd = SDL_GetPerformanceCounter();
        float frameTime = (_frameEnd - _frameStart) / 
                            (float)(SDL_GetPerformanceFrequency());
        _frameStart = _frameEnd;

        onUserUpdate(frameTime); // user defined game loop
        
        display(); // reset pixels

        showText(); // TODO use pixel approach to show the text instead of using ttf

        SDL_RenderPresent(_pRenderer);
    }

}


//draw text on renderer
bool SDLCommon::setText(const std::string str, Vec2<int> pos, int size, SDL_Color color)
{
    SDL_Surface *TextSurface = nullptr;
    _textSize = size;

    _vecTexts.push_back(str);
    SDL_Rect rect = {pos.x, pos.y, (int)(_fontSizeX*str.size()), _fontSizeY};
    _vecRect.push_back(rect);

    if (_pTextFont)
        TextSurface = TTF_RenderText_Solid(_pTextFont, str.c_str(), color);
        
    // // create texture
    if (!TextSurface || !_pTextSurface)
        std::cout << "ERR - failed to create surface: " << SDL_GetError()<< std::endl;

    SDL_Rect temprect = {0, (int)(_fontSizeY*(_vecTexts.size()-1)), 0, 0};

    
    if (SDL_BlitSurface(TextSurface, NULL, _pTextSurface, &temprect) < 0)
        std::cout << "ERR - failed to blit: " << SDL_GetError()<< std::endl;

    SDL_FreeSurface(TextSurface);

    return true;
}

// sent text to screen
// this should be done after the game scene update
// TODO: change
void SDLCommon::showText()
{
    _pTextTexture = SDL_CreateTextureFromSurface(_pRenderer, _pTextSurface);
    SDL_SetTextureAlphaMod( _pTextTexture, 200);
    SDL_SetTextureBlendMode( _pTextTexture, SDL_BLENDMODE_BLEND);

    // init render rect
    for (int i=0; i<_vecTexts.size(); i++)
    {
        SDL_Rect src = _vecRect[i];
        src.x = 0;
        src.y = i*_fontSizeY;

        SDL_Rect dst = _vecRect[i];
        dst.w = _textSize * _vecTexts[i].size();
        dst.h = _textSize;
        
        SDL_RenderCopy(_pRenderer, _pTextTexture, &src, &dst); 
    }

    SDL_FreeSurface(_pTextSurface);
    _pTextSurface = nullptr;
    
}

// helper function to reset the scene texture
void SDLCommon::resetTextTexture()
{
    // init render rect
    if (_pTextTexture) 
    {
        SDL_DestroyTexture(_pTextTexture);
        _pTextTexture = nullptr;
    }
    if (_pTextSurface)
    {
        SDL_FreeSurface(_pTextSurface);
    }

    //_pTextSurface = SDL_CreateRGBSurface(0, _screenWidth, _screenHeight, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
    _pTextSurface = createColorSurface(_screenWidth, _screenHeight);
    _vecRect.clear();
    _vecTexts.clear();

}

// function to display the scene
void SDLCommon::display()
{
    SDL_UpdateTexture(_pTexture, nullptr, _texturePixels, _textureWidth*sizeof(Uint32));

    // copy the texture to the renderer
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = _screenWidth;
    srcRect.h = _screenHeight;

    // check if the border of the text horizontally
    srcRect.x = _cameraPos.x;
    srcRect.y = _cameraPos.y;

    SDL_RenderCopy(_pRenderer, _pTexture, &srcRect, NULL);
    
}

// helper function to create a transparant surface
SDL_Surface * SDLCommon::createColorSurface(int w, int h)
{

    Uint32 rmask, gmask, bmask, amask;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    // created the texture that will store the image
    SDL_Surface *surf = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);

    if (surf)
        return surf;
    else
        return nullptr;
} 


// helper function to create a texture
void SDLCommon::createTexture()
{
    // delete any previously created texture
    if (_pTexture != nullptr)
    {
        SDL_DestroyTexture(_pTexture);
    }

    // created the texture that will store the image
    SDL_Surface *tempSurface = createColorSurface(_textureWidth, _textureHeight);
    _pTexture = SDL_CreateTextureFromSurface(_pRenderer, tempSurface);
    SDL_FreeSurface(tempSurface);
} 

// helper function to unitialize scene background
void SDLCommon::initBackground(int w, int h, std::vector<SDL_Color> &pixels)
{
    _viewHeight = h;
    _viewWidth = w;
    _textureWidth = w; // * _pixelSizeX;
    _textureHeight = h; // * _pixelSizeY;
    _textureColors.resize(_viewWidth * _viewHeight);
   ///memcpy(backgroundPixels, pixels, w * h * sizeof(Uint32));

    //clear the pixel buffer
    _backgroundPixels = new Uint32[_textureHeight * _textureWidth];
    _texturePixels = new  Uint32[_textureHeight * _textureWidth];

    memset(_backgroundPixels, 0, _textureWidth * _textureHeight * sizeof(Uint32));
    memset(_texturePixels, 0, _textureWidth * _textureHeight * sizeof(Uint32));
    
    // update the pixel buffer
    for (int _x=0; _x<_textureWidth; _x++)
    {
        for (int _y=0; _y<_textureHeight; _y++)
        {
            SDL_Color c = pixels[_y*_viewWidth + _x];
            _backgroundPixels[_y*_textureWidth + _x] = convertColorUint(c);
        }
    }
    
    memcpy(_texturePixels, _backgroundPixels, sizeof(Uint32) * _textureHeight * _textureWidth);
    
}

// helper function to copy cached scene background
void SDLCommon::updateBackground(std::vector<SDL_Color> &pixels)
{
     // update the pixel buffer
    for (int _x=0; _x<_textureWidth; _x++)
    {
        for (int _y=0; _y<_textureHeight; _y++)
        {
            SDL_Color c = pixels[_y*_viewWidth + _x];
            _backgroundPixels[_y*_textureWidth + _x] = convertColorUint(c);
        }
    }
    memcpy(_texturePixels, _backgroundPixels, sizeof(Uint32) * _textureHeight * _textureWidth);
}

// draw background by copy pixel values to texture
void SDLCommon::setBackground()
{
    memcpy(_texturePixels, _backgroundPixels, sizeof(Uint32) * _textureHeight * _textureWidth);
}

// helper function to convert color from rgba to uint32
Uint32 SDLCommon::convertColorUint(SDL_Color color)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 pixelColor = (color.r << 24) + (color.g << 16) + (color.b << 8) + 255;
    #else
        Uint32 pixelColor = (255 << 24) + (color.b << 16) + (color.g << 8) + color.r;
    #endif

    return pixelColor;
}


// helper function to convert color from uint32 to rgba
SDL_Color SDLCommon::convertColorRGBA(Uint32 color)
{
    SDL_Color out;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        out.r = (color >> 24) & 0xff;
        out.g = (color >> 16) & 0xff;
        out.b = (color >> 8) & 0xff;
        out.a = color & 0xff;

    #else
        out.a = (color >> 24) & 0xff;
        out.b = (color >> 16) & 0xff;
        out.g = (color >> 8) & 0xff;
        out.r = color & 0xff;
    #endif

    return out;
}

// function to change indivitual pixel value at a position on texture
void SDLCommon::setPixel(const int x, const int y, SDL_Color color, bool updatebk)
{

    if((x >= 0) && (x < _textureWidth) && (y >= 0) && (y < _textureHeight))
    {
        if (updatebk)
        {
            _backgroundPixels[y * _textureWidth + x] = convertColorUint(color);
        }
        _texturePixels[y * _textureWidth + x] = convertColorUint(color);
    }

}

// overloaded function of setPixel
void SDLCommon::setPixel(const int x, const int y, Uint32 color, bool updatebk)
{

    if((x >= 0) && (x < _textureWidth) && (y >= 0) && (y < _textureHeight))
    {
        if (updatebk)
        {
            _backgroundPixels[y * _textureWidth + x] = color;
        }
        _texturePixels[y * _textureWidth + x] = color;
    }

}


// function to alpha blend a pixels value to oher pixel values
void SDLCommon::blendPixel(const int x, const int y, SDL_Color color, bool updatebk)
{

    SDL_Color bg = convertColorRGBA(_backgroundPixels[y * _textureWidth + x]);
    int alpha = (int)color.a;

    if((x >= 0) && (x < _textureWidth) && (y >= 0) && (y < _textureHeight))
    {
        if (updatebk)
        {
            _backgroundPixels[y * _textureWidth + x] = convertColorUint(color);
        }

        SDL_Color blendedColor;

        blendedColor.r = ((int)color.r * alpha + (int)bg.r * (255 - alpha)) >> 8;
        blendedColor.g = ((int)color.g * alpha + (int)bg.g * (255 - alpha)) >> 8;
        blendedColor.b = ((int)color.b * alpha + (int)bg.b * (255 - alpha)) >> 8;

        _texturePixels[y * _textureWidth + x] = convertColorUint(blendedColor);
    }

}

// overloaded function of blendPixel
void SDLCommon::blendPixel(const int x, const int y, Uint32 c, bool updatebk)
{
    //std::cout << "convert";
    SDL_Color color = convertColorRGBA(c);
    blendPixel(x, y, color, updatebk);

}

// function to draw a line
void SDLCommon::setLine(const int a1,  const int b1, const int x2, const int y2, SDL_Color color)
{
    //Bresenham's Line Algorithm
    // @https://www.roguebasin.com/index.php/Bresenham%27s_Line_Algorithm
    int x1 = a1;
    int y1 = b1;
    int delta_x(x2 - x1);
    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;

    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;

    setPixel(x1, y1, color);

    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error(delta_y - (delta_x >> 1));
 
        while (x1 != x2)
        {
            // reduce error, while taking into account the corner case of error == 0
            if ((error > 0) || (!error && (ix > 0)))
            {
                error -= delta_x;
                y1 += iy;
            }
            // else do nothing

            error += delta_y;
            x1 += ix;

            setPixel(x1, y1, color);
        }
    }
    else
    {
        // error may go below zero
        int error(delta_x - (delta_y >> 1));

        while (y1 != y2)
        {
            // reduce error, while taking into account the corner case of error == 0
            if ((error > 0) || (!error && (iy > 0)))
            {
                error -= delta_y;
                x1 += ix;
            }
            // else do nothing

            error += delta_x;
            y1 += iy;
 
            setPixel(x1, y1, color);
        }
    }
}

// function to change the view of the map
void SDLCommon::panTexture(int dx, int dy)
{

    // update camera
    _cameraPos.x += (int)(floor(dx / _pixelSizeX));
    _cameraPos.y += (int)(floor(dy / _pixelSizeY));
    
    //clip if reach the border of the map

    if (_cameraPos.x < 0)
        _cameraPos.x = 0;
    if (_cameraPos.x > _textureWidth - _screenWidth)
        _cameraPos.x = _textureWidth- _screenWidth+20;
    if (_cameraPos.y < 0)
        _cameraPos.y = 0;
    if (_cameraPos.y > _textureHeight - _screenHeight)
        _cameraPos.y =  _textureHeight - _screenHeight; 

}

// function to get the mouse position on the map
Vec2<int> SDLCommon::getMousePosOnRender()
{
    int x, y;
    Vec2<int> pos;
    SDL_GetMouseState(&x, &y); // window position
    std::cout << "created a object at: " << x << " " << y << std::endl;
    pos.x = (int)(floor(x / _pixelSizeX)) + _cameraPos.x;
    pos.y = (int)(floor(y / _pixelSizeY)) + _cameraPos.y;
    return pos;
}

// function to load an image to a surface
SDL_Surface* SDLCommon::loadImageToSurface(const std::string filename, int& w, int& h)
{
    //The final optimized image
    //outSurface = nullptr;
    if (!std::ifstream(filename.c_str()).good())
    {
        std::cout << "file not exist" << std::endl;
        return nullptr;
    }

    //Load image at specified path
    SDL_Surface *s = IMG_Load(filename.c_str());

    if(s == nullptr)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", filename.c_str(), IMG_GetError());
        return nullptr;
    }

    w = s->w;
    h = s->h;
    return s;
}

SDL_PixelFormat* SDLCommon::_pSpriteFormat = nullptr;

//function to load an image to pixel format
std::vector<std::shared_ptr<Uint32[]>> SDLCommon::loadImageToPixels(const std::string filename, std::vector<SDL_Rect> rects)
{
    std::vector<std::shared_ptr<Uint32[]>> vecPixels;
    Uint32* pixels = nullptr;

    //Load image at specified path
    int imgW, imgH; // dimension of the raw image
    SDL_Surface* loadedSurface = loadImageToSurface(filename, imgW, imgH);

    if (!loadedSurface)
    {
        std::cout << "ERR - surface not valid." << std::endl;
    }
    else
    {
        pixels = static_cast<Uint32*>(loadedSurface->pixels);
    }
    if(pixels == nullptr)
    {
        std::cout << "ERR - pixels poiter is not valid" << std::endl;
    }

    for (int i = 0; i<rects.size(); i++) // we loop through all sprites
    {
        size_t sizePixels = rects[i].w * rects[i].h;
        std::shared_ptr<Uint32[]> subPixels(new Uint32[sizePixels], std::default_delete<Uint32[]>());

        int l = 0;
        for (int k=rects[i].y; k<rects[i].y+rects[i].h; k++)
        {
            SDL_memcpy(&subPixels.get()[l*rects[i].w], &pixels[k*imgW+rects[i].x], sizeof(Uint32)*rects[i].w);
            l++;
        }

        vecPixels.push_back(subPixels);
        
    }

    _pSpriteFormat = loadedSurface->format;
    SDL_FreeSurface(loadedSurface);

    return vecPixels;
}