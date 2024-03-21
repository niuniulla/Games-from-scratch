#pragma once

#include <iostream>
#include <cassert>

#include "SDLCommon.hpp"

/*
	PhysicObject class
	This is the base class of all objects used in game
*/
class PhysicObject
{
    public:
        PhysicObject(float x, float y, float _vx=0.0f, float _vy=0.0f) : 
            px(x), 
            py(y),
            vx(_vx),
            vy(_vy)
        {

        };

        virtual void draw(SDLCommon *engin, float offsetX, float offsetY) = 0;
        virtual int BounceDeathAction() = 0;
        virtual bool Damage(float) = 0;
        virtual bool Repair(float) = 0;
        std::string getName() {return _name;};

    public:
        float px=0.0f, py=0.0f; // positions
        float vx=0.0f, vy=0.0f; // velocity
        float ax=0.0f, ay=0.0f; // acceleration

        float radius = 4.0f;
        bool _bStable = false;
        float fFriction = 0.8f;

        int nBouncingBeforeDeath = -1;

        bool _isDead = false;
        bool _isActive = true;

        static void drawMesh(SDLCommon *engin, 
                             std::vector<Vec2<float>> &vecVerts, 
                             float x, float y, float r = 0.0f, float s = 1.0f, 
                             SDL_Color color=color::white);

        static void drawPixels(SDLCommon *engin, 
							   Uint32* vecPixels, 
							   SDL_Rect clip,
							   int32_t x, int32_t y, float scale, bool flip=false);

    public:
        float _fHealth = 1.0f;
        bool bIsPlayable = true;
        std::string _name;
};


/*
	Circle class
*/
class Circle : public PhysicObject
{
    public:
        Circle(float x, float y, float _vx=0.0f, float _vy=0.0f);

        void draw(SDLCommon *engin, float offsetX, float offsetY) override;

        int BounceDeathAction() override
        {
            return 0; 
        }

        bool Damage(float d) override
        {
            return true;
        }

        bool Repair(float d) override
        {
            return true;
        }

    private:
        static std::vector<Vec2<float>> vecModel;
};


/*
    Meteorite class
*/
class Meteorite : public PhysicObject
{
    public:
        Meteorite(float x, float y, float _vx=0.0f, float _vy=0.0f);
        void draw(SDLCommon *engin, float offsetX, float offsetY) override;

        int BounceDeathAction() override
        {
            return 20;
        }

        bool Damage(float d) override
        {
            return true;
        }

        bool Repair(float d) override
        {
            return true;
        }

    private:
        static std::vector<Vec2<float>> vecModel;

};


/*
    Food class
*/
class Food : public PhysicObject
{
    public:
        Food(float x, float y, float _vx=0.0f, float _vy=0.0f);
        void draw(SDLCommon *engin, float offsetX, float offsetY) override;

        int BounceDeathAction() override
        {
            return 0;
        }

        bool Damage(float d) override
        {
            return true;
        }

        bool Repair(float d) override
        {
            return true;
        }

        static int _numState;
        static bool _flip;

    private:
        static std::vector<Vec2<float>> vecModel;
        std::vector<SDL_Rect> _vecClips;
        std::vector<std::shared_ptr<Uint32[]>> _vecPixels;
        
};


/*
    debris class
*/
class Debris : public PhysicObject
{
    public:
        Debris(float x, float y, float _vx=0.0f, float _vy=0.0f);

        void draw(SDLCommon *engin, float offsetX, float offsetY) override;

        int BounceDeathAction() override
        {
            return 0; 
        }

        bool Damage(float d) override
        {
            return true;
        }

        bool Repair(float d) override
        {
            return true;
        }

    private:
        static std::vector<Vec2<float>> vecModel;

};


/*
    Dino class
*/
class Dino : public PhysicObject
{
    public:
        Dino(float x, float y, float _vx=0.0f, float _vy=0.0f);
        void draw(SDLCommon *engin, float offsetX, float offsetY) override;

        int BounceDeathAction() override
        {
            return 0; 
        }

        bool Damage(float d) override;
        bool Repair(float d) override;

    public:
        float _fShootAngle = 0.0f;

        Uint32 *_pPixels;
        std::vector<SDL_Rect> _vecClips;
        std::vector<std::shared_ptr<Uint32[]>> _vecPixels;
        static int _numState;
        static bool _flip;
        
    private:
        static std::vector<Vec2<float>> vecModel;
        static std::vector<Vec2<float>> vecDead;
};

