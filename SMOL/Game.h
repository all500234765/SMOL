#pragma once

#include "pc.h"

class Game {
private:


public:
    virtual void GameStart() {};
    virtual void Step(f32 delta_time) {};
    virtual void GameEnd() {};

    virtual void RoomStart() {};
    virtual void RoomEnd() {};


};
