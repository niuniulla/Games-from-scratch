#include "Game.hpp"

Game::Game()
{
   _appName = "DinoSho";
}

Game::~Game()
{
    
}


/**
 * game init.
 */
bool Game::onUserInit()
{
    _map = new unsigned char[_mapWidth * _mapHeight];
    memset(_map, 0, _mapHeight * _mapWidth * sizeof(unsigned char));
    createMap();
    currentState = GAME_INIT;
    currentState = GAME_INIT;
    return true;
}

/**
 * game main loop.
 *
 * all game logics were run here.
 *
 */
void Game::onUserUpdate(float frameTime)
{
    // if (frameTime > 0.01)
    //     std::cout << "frame time: " << frameTime << std::endl;

    SDL_Delay(10);
    SDL_Event event;

    switch(currentState)
    { 
        case (GAME_INIT) :
        {
            std::cout << "init "  << std::endl;
            _gameTime = 0;
            _score = 0;
            _gameOver = false;
            createMap();

            _objectList.clear();
            auto dino = new Dino(100, 10);
            _pActionObj = dino;
            _objectList.push_back(std::unique_ptr<Dino>(dino));

            nextState = GAME_ON;
            break;
        }

        case (GAME_ON) :
        {
            // ramdom generate meteorites
            if (((int)(_gameTime*1000) % 200) == 0)
            {
                // create meat or meteorites
                float wx = (float)rand()/RAND_MAX  * _mapWidth;
                float vx = (float)rand()/RAND_MAX  * 200 - 100;
                float vy = 0.0f;
                std::cout << "test: " <<  (float)rand()/RAND_MAX  * _mapWidth << std::endl;
                _objectList.push_back(std::unique_ptr<Meteorite>(std::make_unique<Meteorite>(Meteorite(wx, 0, vx, vy))));
            }

            // ramdom generate food
            if (((int)(_gameTime*1000) % 500) == 0)
            {
                // create meat or meteorites
                float wx = (float)rand()/RAND_MAX  * _mapWidth;
                _objectList.push_back(std::unique_ptr<Food>(std::make_unique<Food>(Food(wx, 0))));
            }

            // generate some bubbles
            if (((int)(_gameTime*1000) % 100) == 0)
            {
                //create some bubbles
                float vx = (float)rand()/RAND_MAX * 50.0f;
                float vy = -(float)rand()/RAND_MAX * 200.0f;

                _objectList.push_back(std::unique_ptr<Circle>(std::make_unique<Circle>(Circle(20.0f, 45.0f, vx, vy)))); 
            }

            // generate some bubbles
            if (((int)(_gameTime*1000) % 50) == 0)
            {
                //create some bubbles
                float vx = (float)rand()/RAND_MAX * 50.0f;
                float vy = -(float)rand()/RAND_MAX * 200.0f;

                _objectList.push_back(std::unique_ptr<Circle>(std::make_unique<Circle>(Circle(120.0f, 45.0f, vx, vy)))); 
            }

            nextState = GAME_ON; // by default, stay at gaming state

            break; 
        }

        case (GAME_END) :
        {
            Vec2<int> pos= {80, 50};
            this->setText("GAME OVER", pos, 8, color::black);
            pos.x = 68; pos.y = 60;
            this->setText("Press Space", pos, 8, color::black);
            pos.x = 68; pos.y = 70;
            this->setText(" to restart", pos, 8, color::black);
            //nextState = GAME_END;
            if (SDL_PollEvent(&event) != 0)
            {
                
                if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_SPACE))
                {
                    //std::cout << "restart; " << currentState << std::endl;
                    nextState = GAME_INIT;
                }  
            }
            
            break;
        }
        default:
            break;
    }

    // game loop
    
    _gameTime += frameTime;
    _score += _gameTime / 10000;
    //SDL_Event event;
    
    if (SDL_PollEvent(&event) != 0)
    {
        //std::cout << "test: " <<   _pActionObj->_name << std::endl;
        
        if (event.type == SDL_QUIT)
        {
            _atomIsRunning = false;
        }

        switch (event.type)
        { 
            case SDL_KEYDOWN:
            {
                
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    //std::cout <<  _pActionObj->_name << std::endl;
                    _pActionObj->px += -1000.0f * frameTime;
                    _pActionObj->py += -1000.0f * frameTime;
                    Dino::_flip = false;
                    Dino::_numState = (++Dino::_numState)%4;

                    // center the dino on the map
                    panTexture(_pActionObj->px-_screenWidth/2, 0);
                    
                }
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    _pActionObj->px += 1000.0f * frameTime;
                    _pActionObj->py += -1000.0f * frameTime;
                    Dino::_flip = true;
                    Dino::_numState = (++Dino::_numState)%4;

                    // center the dino on the map
                    panTexture(_pActionObj->px-_screenWidth/2, 0);
                }
                // create dummy for testing
                if (event.key.keysym.sym == SDLK_d)
                {
                    Vec2<int> mPos = getMousePosOnRender();
                    _objectList.push_back(std::make_unique<Circle>(Circle(mPos.x, mPos.y)));
                }
                // create meteorite
                if (event.key.keysym.sym == SDLK_b)
                {
                    Vec2<int> mPos = getMousePosOnRender();
                    _objectList.push_back(std::make_unique<Meteorite>(Meteorite(mPos.x, mPos.y, 0.0f, 0.0f)));
                }
                // create energy
                if (event.key.keysym.sym == SDLK_e)
                {
                    Vec2<int> mPos = getMousePosOnRender();
                    _objectList.push_back(std::make_unique<Food>(Food(mPos.x, mPos.y)));
                }
                // // create dino
                // if (event.key.keysym.sym == SDLK_SPACE)
                // {
                //     Vec2<int> mPos = getMousePosOnRender();
                //     auto dino = new Dino(mPos.x, mPos.y);
                //     _pActionObj = dino;
                //     _objectList.push_back(std::unique_ptr<Dino>(dino));
                // }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    // dino jump
                    _pActionObj->py += -2000.0f * frameTime;
                
                }  
                
                break;
            }
            
        }
    }
    drawMap();
    
    // physics of objects
    std::list<std::unique_ptr<PhysicObject>> tempList; // for created objs during the loop
    for (auto &&obj : _objectList)
    {
        // update gravity
        obj->ay += 2.0f;

        // update velocity
        obj->vx += obj->ax * frameTime;
        obj->vy += obj->ay * frameTime;

        // update position
        Vec2<float> potentialPos;
        potentialPos.x = obj->px + obj->vx * frameTime;
        potentialPos.y = obj->py + obj->vy * frameTime;

        // test for map border
        if (potentialPos.x < 0) potentialPos.x = 0;
        if (potentialPos.y < 0) potentialPos.y = 0;
        if (potentialPos.x >= _mapWidth) potentialPos.x = _mapWidth - 1;
        if (potentialPos.y >= _mapHeight) potentialPos.y = _mapHeight - 1;

        // test collision
        //std::cout << "test1" << std::endl;
        Vec2<float> vResponse;
        bool isCollide = testCollision(obj, potentialPos, vResponse);
        if (isCollide)
        {
            //std::cout << "test2" << std::endl;
            obj->_bStable = true;
            float fMagVelocity, fMagReponse;
            bounce(obj, vResponse, fMagVelocity, fMagReponse);
            //std::cout << "test3" << std::endl;
            if (obj->nBouncingBeforeDeath == -1)
            {
                obj->_isDead = true;
            }
            //std::cout << "test4" << std::endl;
            if (obj->nBouncingBeforeDeath > 0) // if there is action after bouncing
            {
                //std::cout << "test5" << std::endl;
                obj->nBouncingBeforeDeath--;
                obj->_isDead = (obj->nBouncingBeforeDeath == 0);

                //after death action
                if (obj->_isDead)
                {
                    //std::cout << "test6" << std::endl;
                    int nReponse = obj->BounceDeathAction();
                    if (nReponse > 0) // if explode
                    {
                        //std::cout << "boom: "  << obj->getName()<< std::endl;
                        Boom(obj->px, obj->py, nReponse, tempList);
                        //std::cout << "boom end: "  << std::endl;
                    }
                }
            }
            
            // test if game over
            if (!(obj->_isActive) && !_gameOver)
            {
                //std::cout << "test8: " << _pActionObj->getName() << std::endl;
                nextState = GAME_END;
                _gameOver = true;
            }

        }
        else
        {
            // update obj position
            
            if (obj->_name == "food")
            {

                gain(potentialPos.x, potentialPos.y, 10.0f);
            }
                
            obj->px = potentialPos.x;
            obj->py = potentialPos.y;
            
        }
    }

    
    // remove dead objects
    //std::cout << "removing dead" << std::endl;
    _objectList.erase(std::remove_if(_objectList.begin(), 
                                    _objectList.end(), 
                                    [](std::unique_ptr<PhysicObject> &o)
                                    {
                                        return o->_isDead;
                                    }), 
                                    _objectList.end());
    // merge the templist to the obj list
    _objectList.merge(tempList);

    //std::cout << "remove dead" << std::endl;
    // draw all objects
    for (auto &obj : _objectList)
    {
        // draw whatever the obj
        obj->draw(this, _cameraPos.x, _cameraPos.y);
        //std::cout << "test9" << std::endl;
        // if is dino, draw dino accessories
        if (_pActionObj != nullptr && obj.get() == (Dino*)(_pActionObj))
        {
            // if damage
            float mx = _pActionObj->px;
            float my = _pActionObj->py;
            //std::cout << "eng: " << _pActionObj->_fHealth << std::endl;
            for (int i=0; i<10*obj->_fHealth; i++)
            {
                setLine(mx-7+i, my-2, mx-7+i, my-4, color::green);
                setLine(mx-7+i, my-4, mx-7+i, my-5, color::red);
            }

        }
    }

    //std::cout << "test10" << std::endl;
    //show fcounttime
    int s = 10;
    Vec2<int> pos= {5, 5};
    this->setText("Time: ", pos, s, color::darkgreen);
    pos.x = 55;
    this->setText(std::to_string((int)(_gameTime)), pos, s, color::darkgreen);
    pos.y = 15; pos.x = 5;
    this->setText("Score: ", pos, s, color::darkgreen);
    pos.y = 15; pos.x = 55;
    this->setText(std::to_string((int)_score), pos, s, color::darkgreen);

    currentState = nextState;

}


// helper func to create perlin noises
// Taken from Perlin Noise Video https://youtu.be/6-0UaeJBumA
void Game::PerlinNoise1D(int nCount, float *fSeed, int nOctaves, float fBias, float *fOutput)
{
    // Used 1D Perlin Noise
    for (int x = 0; x < nCount; x++)
    {
        float fNoise = 0.0f;
        float fScaleAcc = 0.0f;
        float fScale = 1.0f;

        for (int o = 0; o < nOctaves; o++)
        {
            int nPitch = nCount >> o;
            assert(nPitch);
            int nSample1 = (x / nPitch) * nPitch;
            int nSample2 = (nSample1 + nPitch) % nCount;
            float fBlend = (float)(x - nSample1) / (float)nPitch;
            float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];
            fScaleAcc += fScale;
            fNoise += fSample * fScale;
            fScale = fScale / fBias;
        }

        // Scale to seed range
        fOutput[x] = fNoise / fScaleAcc;
    }
}

// create map using random values
void Game::createMap()
{
    float *fSurface = new float[_mapWidth];
    float *fSeeds = new float[_mapWidth];

    //generate noise
    for (int i=0; i<_mapWidth; i++)
    {
        fSeeds[i] = float(rand()) / float(RAND_MAX);
    }
    fSeeds[0] = 0.5f;

    PerlinNoise1D(_mapWidth, fSeeds, 4, 3.0f, fSurface);

    assert(fSurface != nullptr);

    _pixels.resize(_mapWidth*_mapHeight);
    int count = 0;

    // generate map
    for(int x=0; x<_mapWidth; x++)
    {
        for (int y=0; y<_mapHeight; y++)
        {
            if (y > fSurface[x] * _mapHeight * 1.5)
            { 
                _map[y * _mapWidth + x] = 1;
                _pixels[y * _mapWidth + x] = color::olive;
                count++;
            }

            else
            { 
                _map[y * _mapWidth + x] = 0;
                _pixels[y * _mapWidth + x] = color::cyan;
            }
        }
    }

    delete[] fSurface;
    delete[] fSeeds;

    initBackground(_mapWidth, _mapHeight, _pixels);

    std::cout << "DEBUG : finished map creation." << std::endl;

}

// paint map : 1 -> terrain, 0 -> sky
void Game::drawMap()
{
    setBackground();

}


// helper function to test colliion with the terrain
// use the semi-circle approach
bool Game::testCollision(std::unique_ptr<PhysicObject> &obj, Vec2<float> &potentialPos, Vec2<float> &vResponse)
{
    // check for collisions
    float fAngle = atan2f(obj->vy, obj->vx); //center angle for checking
    bool isCollide = false;
    float vResponseX = 0.0f;
    float vResponseY = 0.0f;

    // check for each point on the semi-circle centered at fAngle
    float deltaAngle = PI / 10.0f;
    for (int i=0; i<10;i++)
    {
        float a = fAngle-PI/2.0f + deltaAngle * i;
    
        float fTestPosX = potentialPos.x + obj->radius * cosf(a);
        float fTestPosY = potentialPos.y + obj->radius * sinf(a);

        // test for map border
        if (fTestPosX < 0) fTestPosX = 0;
        if (fTestPosY < 0) fTestPosY = 0;
        if (fTestPosX >= _mapWidth) fTestPosX = _mapWidth - 1;
        if (fTestPosY >= _mapHeight) fTestPosY = _mapHeight - 1;

        if (_map[int(fTestPosY) * _mapWidth + int(fTestPosX)] != 0)
        {
            isCollide = true;
            // get the normal vector of the collision
            vResponse.x = potentialPos.x - fTestPosX;
            vResponse.y = potentialPos.y - fTestPosY;
        }
    }
    return isCollide;
}


// function to get the reflective velocity after collision
void Game::bounce(std::unique_ptr<PhysicObject> &obj, Vec2<float> &vResponse, float &fMagVelocity, float &fMagReponse)
{
    fMagVelocity = sqrtf(obj->vx * obj->vx + obj->vy * obj->vy);
    fMagReponse = sqrtf(vResponse.x * vResponse.x + vResponse.y * vResponse.y);
    // calculate reflexion vector, vresponse as normal
    float dot = obj->vx * (vResponse.x / fMagReponse) + obj->vy * (vResponse.y / fMagReponse);

    obj->vx = obj->fFriction * (obj->vx - 2.0f * dot * (vResponse.x / fMagReponse));
    obj->vy = obj->fFriction * (obj->vy - 2.0f * dot * (vResponse.y / fMagReponse)); 
}


// func to do explosion
void Game::Boom(float wx, float wy, float fRadius, std::list<std::unique_ptr<PhysicObject>> &tempList)
{
    // blowup terrain
    circleBresenham(wx, wy, fRadius*0.1); 

    //shockwave other objects
    for(auto &&o : _objectList)
    {
        float dx = o->px - wx;
        float dy = o->py - wy;
        float fDist = sqrtf(dx * dx + dy * dy) > 0.0001f ? sqrtf(dx * dx + dy * dy) : 0.0001f;
        if (fDist < fRadius)
        {
            o->vx = (dx / fDist) * fRadius;
            o->vy = (dy / fDist) * fRadius;
            o->Damage(((fRadius - fDist) / fRadius) * 0.8f);
        }
    }

    // shoot one load of debris
    for(int i=0; i<int(fRadius); i++)
    {
        tempList.push_back(std::unique_ptr<Debris>(std::make_unique<Debris>(Debris(wx, wy))));
    }

}


// func to gain food
void Game::gain(float wx, float wy, float fRadius)
{
    //shockwave other objects
    for(auto &obj : _objectList)
    {
        float dx = obj->px - wx;
        float dy = obj->py - wy;
        float fDist = sqrtf(dx * dx + dy * dy) > 0.0001f ? sqrtf(dx * dx + dy * dy) : 0.0001f;
        if (fDist < fRadius)
        {
            if ((obj->_fHealth >= 1.0f) && (obj->_name == "dino"))
                _score += 1;
            obj->Repair(0.1f);
        }
    }
}


// func to paint pixels on map
void Game::circleBresenham(int xc, int yc, int r) 
{
    auto drawLine = [this](int sx, int ex, int y){
        for(int i = sx; i<=ex; i++)
        {
            _map[y * this->_mapWidth + i] = 0;
            _pixels[y * this->_mapWidth + i] = color::cyan;
            //updateBackground(_pixels);
            setPixel(i, y, color::cyan, true);
        }
    };
    int x = 0, y = r; 
    int d = 3 - 2 * r;

    while (y >= x) 
    { 
        drawLine(xc-x, xc+x, yc-y);
        drawLine(xc-x, xc+x, yc+y);
        drawLine(xc-y, xc+y, yc-x);
        drawLine(xc-y, xc+y, yc+x);

        // check for decision parameter 
        if (d > 0) 
            d = d + 4 * (++x - --y) + 10; 
        else
            d = d + 4 * ++x + 6; 
    } 
} 
