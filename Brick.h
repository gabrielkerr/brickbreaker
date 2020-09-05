/*
	Outline for implementation of Brick class.
*/

#ifndef __BREAKOUT_BRICK_H__
#define __BREAKOUT_BRICK_H__

class Brick
{
	public: 
		Brick();

		Brick(float x_pos, float y_pos);

		~Brick();

		float x;
		float y;
		bool was_hit;

}; // end Brick

#endif
