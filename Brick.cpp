#include "Brick.h"

Brick::Brick()
: x(0)
, y(0)
, was_hit(false)
{

}

Brick::Brick(float x_pos, float y_pos)
: x(x_pos)
, y(y_pos)
, was_hit(false)
{
}


Brick::~Brick()
{

}