
#include <vector>
#include <math.h>

#include "Object.hpp"


/*
	PhysicObject class
	This is the base class of all objects used in game
*/

// static function
// to draw the object which is defined by vertices and drawn as mesh
void PhysicObject::drawMesh(SDLCommon *engin, 
							std::vector<Vec2<float>> &vecVerts, 
							float x, float y, float r, float s, 
							SDL_Color color)
{

	// Create translated model vector of coordinate pairs
	int verts = vecVerts.size();
	assert(verts > 0);
	std::vector<Vec2<float>> vecTransformedVerts(verts, {0.0f, 0.0f});

	// Rotate
	for (int i = 0; i < verts; i++)
	{
		vecTransformedVerts[i].x = vecVerts[i].x * cosf(r) - vecVerts[i].y * sinf(r);
		vecTransformedVerts[i].y = vecVerts[i].x * sinf(r) + vecVerts[i].y * cosf(r);
	}

	// Scale
	for (int i = 0; i < verts; i++)
	{
		vecTransformedVerts[i].x = vecTransformedVerts[i].x * s;
		vecTransformedVerts[i].y = vecTransformedVerts[i].y * s;
	}

	// Translate
	for (int i = 0; i < verts; i++)
	{
		vecTransformedVerts[i].x = vecTransformedVerts[i].x + x;
		vecTransformedVerts[i].y = vecTransformedVerts[i].y + y;
	}

	// Draw Closed Polygon
	for (int i = 0; i < verts+1; i++)
	{
		int j = (i + 1);
		engin->setLine( (int)vecTransformedVerts[i % verts].x, 
						(int)vecTransformedVerts[i % verts].y,
						(int)vecTransformedVerts[j % verts].x, 
						(int)vecTransformedVerts[j % verts].y,
						color);
	}
}


// static function
// to draw the object which is defined by pixels
void PhysicObject::drawPixels( SDLCommon* engin, 
							   Uint32* Pixels, 
							   SDL_Rect clip,
							   int32_t x, int32_t y, float scale, bool flip)
{

	if (Pixels == nullptr)
	{
		std::cout << "ERR - no valid pixels."<< std::endl;
		return;
	}

	if (!flip)
	{
		for (int32_t i = 0; i < clip.w; i++)
		{
			for (int32_t j = 0; j < clip.h; j++)
			{
				engin->blendPixel(x + (i * scale), y + (j * scale), Pixels[j*clip.w+i]);
			}
		}
	}
	else
	{
		for (int32_t i = 0; i < clip.w; i++)
		{
			for (int32_t j = 0; j < clip.h; j++)
			{
				engin->blendPixel(x + (i * scale), y + (j * scale), Pixels[j*clip.w+(clip.w-i-1)]);
			}
		}
	}

}


/*
	Circle class
*/
std::vector<Vec2<float>> defineCircle()
{
    std::vector<Vec2<float>> vec;
    //vec.push_back({0.0f, 0.0f});
    int num = 10;
    for (int i=0; i<num; i++)
    {
        float a = cosf((i/9.0f) * 2.0f * PI);
        float b = sinf((i/9.0f) * 2.0f * PI);
        vec.push_back({a, b});
    }
    return vec;
};

// define static member
std::vector<Vec2<float>> Circle::vecModel = defineCircle();


// constructor
Circle::Circle(float x, float y, float _vx, float _vy) : PhysicObject(x, y, _vx, _vy) 
{ 
	_name = "circle";
	radius = 4;
	nBouncingBeforeDeath = 30;
};

void Circle::draw(SDLCommon *engin, float offsetX, float offsetY)
{
	PhysicObject::drawMesh(engin, vecModel, px, py, atan2f(vy, vx), radius, color::white);
}


/*
    Meteorite class
*/

// constructor
Meteorite::Meteorite(float x, float y, float _vx, float _vy) : PhysicObject(x, y, _vx, _vy) 
{
	fFriction = 0.0f;
	radius = 8.5f;
	nBouncingBeforeDeath = 1;
	vx = _vx;
	vy = _vy;
	_name = "meteorite";
}

void Meteorite::draw(SDLCommon *engin, float offsetX, float offsetY)
{
	PhysicObject::drawMesh(engin, vecModel, px, py, atan2f(vy, vx), radius, color::orange);
}

// helper function to generate mesh
std::vector<Vec2<float>> defineMeteorite()
{
    std::vector<Vec2<float>> vec;
    vec.push_back({0.0f, 0.0f});
    int num = 10;
    for (int i=0; i<num; i++)
    {
        float a = cosf((i/9.0f) * 2.0f * PI) * rand()/RAND_MAX ;
        float b = sinf((i/9.0f) * 2.0f * PI) * rand()/RAND_MAX ;
        vec.push_back({a, b});
    }

    //normalize size
    for(auto &v : vec)
    {
        v.x /= 2.5f; v.y /= 2.5f;
    }
    return vec;
}

// define static member
std::vector<Vec2<float>> Meteorite::vecModel = defineMeteorite();


/*
	Food class
*/

//constructor
Food::Food(float x, float y, float _vx, float _vy) : PhysicObject(x, y, _vx, _vy) 
{
    std::string name = "../g1.png";
    fFriction = 0.2f;
    radius = 19.0f;
    nBouncingBeforeDeath = -1;
    _fHealth = 1.0f;
    vx = _vx;
    vy = _vy;

    _vecClips.push_back(SDL_Rect{0, 0, 15, 13});

    _vecPixels = SDLCommon::loadImageToPixels(name, _vecClips);

    if (_vecPixels.size()==0)
    {
        std::cout << "failed to load energy" << std::endl;
    }
    std::cout << "drawing energy: " << std::endl;

    _name = "food";
}


// define static members
int Food::_numState = 0;
bool Food::_flip = false;

// draw loaded pixels
void Food::draw(SDLCommon *engin, float offsetX, float offsetY)
{
    if (_numState >=0 && _numState<1)
    {
        PhysicObject::drawPixels(engin, _vecPixels[_numState].get(), _vecClips[_numState], px, py, 0.5f, _flip);
    }
    else
    {
        std::cout << "ERR - out of range" << std::endl;
    }
}


/* 
	Debris Class
*/

// constructor
Debris::Debris(float x, float y, float _vx, float _vy) : PhysicObject(x, y, _vx, _vy) 
{
	fFriction = 0.8f;
	radius = 1.0f;
	vx = 100.0f * cosf(((float)rand() / (float)RAND_MAX) * 2.0f * PI);
	vy = 100.0f * sinf(((float)rand() / (float)RAND_MAX) * 2.0f * PI);
	nBouncingBeforeDeath = 5;
	_isActive = true;
	_name = "debris";
}

std::vector<Vec2<float>> defineDebris()
{
    std::vector<Vec2<float>> vec;

    vec.push_back({ 0.0f, 0.0f });
	vec.push_back({ 1.0f, 0.0f });
	vec.push_back({ 1.0f, 1.0f });
	vec.push_back({ 0.0f, 1.0f });

    return vec;
}

std::vector<Vec2<float>> Debris::vecModel = defineDebris();

void Debris::draw(SDLCommon *engin, float offsetX, float offsetY)
{
	SDL_Color c = {255, (Uint8)(((float)rand() / (float)RAND_MAX) * 255.0f), 0};
	PhysicObject::drawMesh(engin, vecModel, px, py, atan2f(vy, vx), radius, c);
}


/* 
	Dino Class
*/

// constructor
Dino::Dino(float x, float y, float _vx, float _vy) : PhysicObject(x-10.0f-3, y-9.0, _vx, _vy) 
{
    std::string name = "../g17.png";
    fFriction = 0.2f;
    radius = 19.0f;
    nBouncingBeforeDeath = -2;
    _fHealth = 1.0f;

    _vecClips.push_back(SDL_Rect{0, 0, 20, 20});
    _vecClips.push_back(SDL_Rect{21, 0, 20, 20});
    _vecClips.push_back(SDL_Rect{42, 0, 20, 20});
    _vecClips.push_back(SDL_Rect{64, 0, 20, 20});
    _vecClips.push_back(SDL_Rect{84, 0, 20, 20});
    _vecPixels = SDLCommon::loadImageToPixels(name, _vecClips);

    
    if (_vecPixels.size()==0)
    {
        std::cout << "ERR - failed to load dino image" << std::endl;
    }

    _name = "dino";
}

// define static members
int Dino::_numState = 0;
bool Dino::_flip = false;

// draw pixels
void Dino::draw(SDLCommon *engin, float offsetX, float offsetY)
{
    if (_fHealth > 0.0f)
    {
        if (Dino::_numState >=0 && Dino::_numState<4)
        {
            PhysicObject::drawPixels(engin, _vecPixels[Dino::_numState].get(), _vecClips[Dino::_numState], px, py, 1.0f, Dino::_flip);
        }
        else
        {
            std::cout << "ERR - out of range" << std::endl;
        }
    }
    else // if dead, draw tombestone
    {
        PhysicObject::drawMesh(engin, vecDead, px-offsetX, py-offsetY+10, 3.4f, 10, color::green);
    }   
}

// function to decrease health
bool Dino::Damage(float d)
{
    _fHealth -= d;
    if (_fHealth <= 0.0f)
    {
        _fHealth = 0.0f;
		_isActive = false;
    }
    return _fHealth > 0;
}

// function to increase health
bool Dino::Repair(float d)
{
    if (!_isActive)
        return false;

    _fHealth += d;
    if (_fHealth >= 1.0f)
    {
        _fHealth = 1.0f;
    }
    return true;
}

// define the mesh after the death
std::vector<Vec2<float>> defineDead()
{
    std::vector<Vec2<float>> vec;
	vec.push_back({0.0f, 0.0f});
    vec.push_back({-1.0f, 0.0f});
    vec.push_back({-1.0f, 0.5f});
    vec.push_back({-0.8f, 0.5f});
    vec.push_back({-0.8f, 1.5f});
	vec.push_back({-0.2f, 2.0f});
	vec.push_back({-0.2f, 2.4f});
	vec.push_back({-0.4f, 2.4f});
	vec.push_back({-0.4f, 2.6f});
	vec.push_back({-0.2f, 2.6f});
	vec.push_back({-0.2f, 2.8f});
	vec.push_back({0.2f, 2.8f});
	vec.push_back({0.2f, 2.6f});
	vec.push_back({0.4f, 2.6f});
	vec.push_back({0.4f, 2.4f});
	vec.push_back({0.2f, 2.4f});
	vec.push_back({0.2f, 2.0f});
    vec.push_back({0.0f, 2.0f});
    vec.push_back({0.8f, 1.5f});
    vec.push_back({0.8f, 0.5f});
	vec.push_back({1.0f, 0.5f});
    vec.push_back({1.0f, 0.0f});
	vec.push_back({1.0f, 0.5f});
	vec.push_back({-1.0f, 0.5f});
	vec.push_back({-1.0f, 0.0f});

	for (auto &v : vec)
    {
        v.y = (v.y-1.4f);
    }


    // vec.push_back({0.0f, 0.0f});
    // int num = 10;
    // for (int i=0; i<num; i++)
    // {
    //     float a = cosf((i/9.0f) * 2.0f * PI);
    //     float b = sinf((i/9.0f) * 2.0f * PI);
    //     vec.push_back({a, b});
    // }

    return vec;
}

// define static member
std::vector<Vec2<float>> Dino::vecDead = defineDead();