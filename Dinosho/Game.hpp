#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <array>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cmath>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "SDLCommon.hpp"
#include "Object.hpp"


class Game : public SDLCommon
{
    public:
        Game();
        ~Game();

        bool onUserInit() override;
        void onUserUpdate(float frameTime) override;

    private:
  
        int _fontSize;
        int _lineSkip;

        std::vector<SDL_Color> _pixels;
        unsigned char *_map;
        int _mapWidth = 300;
        int _mapHeight = 160;

        std::list<std::unique_ptr<PhysicObject>> _objectList;

        float _score = 0;
        float _gameTime = 0.0f;

        PhysicObject* _pActionObj = nullptr;

        bool _gameOver = false;

    private:
        void PerlinNoise1D(int nCount, float *fSeed, int nOctaves, float fBias, float *fOutput);   
        void createMap();
        void drawMap();
        bool testCollision(std::unique_ptr<PhysicObject> &obj, Vec2<float> &potentialPos, Vec2<float> &vResponse);
        void bounce(std::unique_ptr<PhysicObject> &obj, Vec2<float> &vResponse, float &fMagVelociry, float &fMagReponse);
        void Boom(float wx, float wy, float fRadius, std::list<std::unique_ptr<PhysicObject>> &tempList);
        void circleBresenham(int xc, int yc, int r) ;
        void gain(float wx, float wy, float fRadius);

        enum GAME_STATE // used for game state switch
        {
            GAME_INIT,
            GAME_ON,
            GAME_END,
        } currentState, nextState;

};
