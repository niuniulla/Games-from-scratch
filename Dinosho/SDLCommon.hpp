#pragma once

#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <memory>
#include <math.h>
#include <fstream>
#include <algorithm>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

const float PI = 3.1415926;

// a helper class to replace std::pair
template<class T>
struct Vec2
{
    T x = 0;
    T y = 0;
};

// color definition
namespace color
{
    const SDL_Color white{255, 255, 255};
    const SDL_Color black{0, 0, 0};
    const SDL_Color red{255, 0, 0};
    const SDL_Color blue{0, 0, 255};
    const SDL_Color green{0, 255, 0};
    const SDL_Color cyan{0, 255, 255};
    const SDL_Color grey{128, 128, 128};
    const SDL_Color fuchsia{255, 0, 255};
    const SDL_Color purple{128, 0, 128};
    const SDL_Color teal{0, 128, 128};
    const SDL_Color yellow{255, 255, 0};
    const SDL_Color orange{255, 165, 0};
    const SDL_Color tomato{255, 99, 71};
    const SDL_Color darkgreen{0, 100, 0};
    const SDL_Color darkblue{0, 0, 139};
    const SDL_Color olive{128, 128, 0};
    const SDL_Color brown{165, 42, 42};
    const SDL_Color lightblue{173, 216, 230};
    const SDL_Color aquamarine{127, 255, 212};
    const SDL_Color silver{192, 192, 192};
}


// base class to handle graphics using sdl
class SDLCommon
{
    public:
        int _screenHeight;
        int _screenWidth;
        int _windowHeight;
        int _windowWidth;
        int _pixelSizeX;
        int _pixelSizeY;
        int _viewHeight;
        int _viewWidth;
        int _textureHeight;
        int _textureWidth;
        const char* _fontFileName = "../Brocats.ttf";
        std::string _appName;
        Uint32 *_texturePixels;
        SDL_Texture *_pTexture;
        std::vector<SDL_Color> _textureColors;
        Vec2<int> _cameraPos;

        static SDL_PixelFormat* _pSpriteFormat;
        inline static std::atomic<bool> _atomIsRunning; // variable to control global game loop

    public:
        virtual bool onUserInit() = 0;
        virtual void onUserUpdate(float frameTime) = 0;
        void onUserStop();
        void execute();
        void create();

        void createTexture();
        void initBackground(const int x, const int y, const Uint32 *pixels);

        void setPixel(const int x, const int y, SDL_Color color, bool updatebk=false);
        void setPixel(const int x, const int y, Uint32 color, bool updatebk=false);
        void blendPixel(const int x, const int y, SDL_Color color, bool updatebk=false);
        void blendPixel(const int x, const int y, Uint32 color, bool updatebk=false);
        void setLine(const int x1, const int y1, const int x2, const int y2, SDL_Color color);
        void display();

        void initBackground(int w, int h, std::vector<SDL_Color> &pixels);
        void setBackground();
        void updateBackground(std::vector<SDL_Color> &pixels);

        void panTexture(int dx, int dy);

        Vec2<int> getMousePosOnRender();

    public:
        SDLCommon();
        ~SDLCommon();

        bool init(int w, int h, int px, int py);

        bool setText(const std::string str, Vec2<int> pos, int size, SDL_Color color);
        
        inline int pixelSizeX() const {return _pixelSizeX;};
        inline int pixelSizeY() const {return _pixelSizeY;};
        inline int screenWidth() const {return _screenWidth;};
        inline int screenHeight() const {return _screenHeight;};

        static SDL_Surface* loadImageToSurface(const std::string filename, int& w, int& h);
        static std::vector<std::shared_ptr<Uint32[]>> loadImageToPixels(const std::string filename, std::vector<SDL_Rect> rects);
        
    private:

        static Uint32 convertColorUint(SDL_Color color);
        static SDL_Color convertColorRGBA(Uint32 color);
   
        SDL_Window *_pWindow = nullptr;
        SDL_Renderer *_pRenderer = nullptr;
        TTF_Font *_pTextFont = nullptr;
        SDL_Surface *_pScreenSurface = nullptr;
        Uint32 *_backgroundPixels = nullptr;
        SDL_Texture *_pTextTexture = nullptr;
        SDL_Surface *_pTextSurface = nullptr;

        int _fontSize;
        int _fontSizeX;
        int _fontSizeY;

        Uint64 _frameStart;
        Uint64 _frameEnd;
        SDL_PixelFormat* _format;

        // for text
        std::vector<std::string> _vecTexts;
        std::vector<SDL_Rect> _vecRect;
        int _textSize; // for showing
        
        void showText();
        void resetTextTexture();

        SDL_Surface * createColorSurface(int w, int h);
};