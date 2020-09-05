#include "Brick.h"
#include <vector>
#include <algorithm>

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].has_changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].has_changed)

float player_1_p = 0.0F;
float player_1_v = 0.0F;
float arena_half_size_x = 85;
float arena_half_size_y = 45;
float player_half_size_x = 12;
float player_half_size_y = 2.5;
float ball_p_x = 0;
float ball_p_y = -10;
float ball_half_size = 1;
float initial_ball_v_y = 70.0F;
float ball_v_x = 0.0;
float ball_v_y = 0;
int player_1_score = 0;
float friction_coef = 10.0F;
float human_player_acc = 1500.0F;
u32 arena_color = 0x63A4FF;
u32 ball_color = 0xEEEEEE;
float brick_half_size_x = 5;
float brick_half_size_y = 2;
const int num_bricks = 50; // TODO Update this to be more bricks
Brick* bricks[num_bricks];

// TODO Update this to place bricks modded into correct row and column
internal void initialize_game()
{
	float brick_x_pos = -60;
	float brick_y_pos = 0;

	for (size_t row_ix(0); row_ix < 5; ++row_ix)
	{
		size_t num_bricks_in_row = 10;
		for (size_t col_ix(0); col_ix < num_bricks_in_row; ++col_ix)
		{
			int brick_ix = col_ix + (row_ix * num_bricks_in_row);
			bricks[brick_ix] = new Brick();
			bricks[brick_ix]->x = brick_x_pos;
			bricks[brick_ix]->y = brick_y_pos;

			brick_x_pos += 11;
		}
		brick_y_pos += 6;
		brick_x_pos = -60;
	}
}

internal void clean_up_game()
{
	for (size_t brick_ix(0); brick_ix < num_bricks; ++brick_ix)
	{
		delete bricks[brick_ix];
		bricks[brick_ix] = 0;
	}
}

internal void simulate_player(float* p, float* v, float acc, float dt)
{
	// Update friction
	acc -= *v * friction_coef;

	// Update position
	*p += *v * dt + acc * dt * dt;

	// Update velocity
	*v += acc * dt;

	if (*p > arena_half_size_x - player_half_size_x)
	{
		// Collide, freeze position
		*p = arena_half_size_x - player_half_size_x;

		// Set player_v = 0
		*v = 0;
	}
	else if (*p < -arena_half_size_x + player_half_size_x)
	{
		// Collide, freeze position
		*p = -arena_half_size_x + player_half_size_x;

		// Set player_v = 0
		*v = 0;
	}
}

internal bool aabb_vs_aabb(float x1, float y1, float hsx1, float hsy1, float x2, float y2, float hsx2, float hsy2)
{
	return 	(x1 + hsx1 > x2 - hsx2) &&
			(x1 - hsx1 < x2 + hsx2) &&
			(y1 + hsy1 > y2 - hsy2) &&
			(y1 - hsy1 < y2 + hsy2);
}

internal std::vector<float> normalize(std::vector<float> v)
{
	std::vector<float> norm_v;
	float v_magnitude = 0;

	for (size_t i(0); i < v.size(); ++i)
	{
		v_magnitude += (v[i] * v[i]);
	}

	v_magnitude = sqrt(v_magnitude);

	for (size_t i(0); i < v.size(); ++i)
	{
		norm_v.push_back(v[i] / v_magnitude);
	}

	return norm_v;
}

internal int
val_clamp(int min, int val, int max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

enum class HitDirection {
	HIT_UP,
	HIT_RIGHT,
	HIT_DOWN,
	HIT_LEFT
};

internal HitDirection get_hit_direction(std::vector<float> v)
{
	HitDirection hd = HitDirection::HIT_UP;

	std::vector<std::vector<float>> hit_vectors = { { 0.0F, 1.0F}, // UP
													{ 1.0F, 0.0F }, // RIGHT
													{ 0.0F, -1.0F }, // DOWN
													{ -1.0F, 0.0F } }; // LEFT

	float min_dot_product = -1.0F;
	for (size_t vector_ix(0); vector_ix < hit_vectors.size(); ++vector_ix)
	{
		std::vector<float> hv = hit_vectors[vector_ix];
		float dot_product = v[0] * hv[0] + v[1] * hv[1];
		if (dot_product > min_dot_product)
		{
			min_dot_product = dot_product;
			hd = (HitDirection)vector_ix;
		}
	}

	return hd;
}

enum class Gamemode
{
	GM_GAMEPLAY,
	GM_MENU
};


//Gamemode gamemode = Gamemode::GM_MENU;
Gamemode gamemode = Gamemode::GM_GAMEPLAY; 
int hotbutton = 0;
bool has_served_ball = false;

internal void
simulate_game(Input* input, float dt)
{
	clear_screen(0x004ba0);

	// If in gameplay, run the game as normal
	if (Gamemode::GM_GAMEPLAY == gamemode)
	{
		// If button state has changed and is down, draw center rect
		float speed = 50.0F;
		float player_1_acc = 0;

		if (is_down(D) || is_down(BUTTON_RIGHT))
		{
			player_1_acc += human_player_acc;
		}
		if (is_down(A) || is_down(BUTTON_LEFT))
		{
			player_1_acc -= human_player_acc;
		}

		simulate_player(&player_1_p, &player_1_v, player_1_acc, dt);

		// Simulate ball
		{
			ball_p_y += ball_v_y * dt;
			ball_p_x += ball_v_x * dt;
		}

		//// Rendering ////

		// Draw arena
		draw_rect(0, 0, arena_half_size_x, arena_half_size_y, arena_color);

		// Draw bricks
		// Iterate through list of bricks and draw them.
		for (size_t brick_ix(0); brick_ix < num_bricks; ++brick_ix)
		{
			Brick* b = bricks[brick_ix];

			// Check to see if brick was hit
			if (!(b->was_hit))
			{
				// Draw brick
				draw_rect(b->x, b->y, brick_half_size_x, brick_half_size_y, 0xff3300);
				
				// If there is a collision with a brick, destroy it.
				if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size,
					b->x, b->y, brick_half_size_x, brick_half_size_y))
				{
					b->was_hit = true;
					player_1_score += 10;

					// Determine which side of brick the ball collided with
					// Find D
					std::vector<float> d = { ball_p_x - b->x, ball_p_y - b->y };

					// Find P = clamp(Ct + D)
					std::vector<float> p = { b->x + d[0], b->y + d[1] };

					float brick_x_max = b->x + brick_half_size_x;
					float brick_x_min = b->x - brick_half_size_x;
					p[0] = max(brick_x_min, min(p[0], brick_x_max)); 

					float brick_y_max = b->y + brick_half_size_y;
					float brick_y_min = b->y - brick_half_size_y;
					p[1] = max(brick_y_min, min(p[1], brick_y_max));

					// Find v
					std::vector<float> v = { ball_p_x - p[0], ball_p_y - p[1] };

					// Normalize v to get u
					std::vector<float> u = normalize(v);

					if (get_hit_direction(u) == HitDirection::HIT_UP)
					{
						ball_p_y =  b->y + brick_half_size_y + ball_half_size;
						ball_v_y *= -1;
					}
					else if (get_hit_direction(u) == HitDirection::HIT_DOWN)
					{
						ball_p_y =  b->y - brick_half_size_y - ball_half_size;
						ball_v_y *= -1;
					}
					else if (get_hit_direction(u) == HitDirection::HIT_RIGHT)
					{
						ball_p_x =  b->x + brick_half_size_x + ball_half_size;
						ball_v_x *= -1;
					}
					else if (get_hit_direction(u) == HitDirection::HIT_LEFT)
					{
						ball_p_x =  b->x - brick_half_size_x - ball_half_size;
						ball_v_x *= -1;
					}

				}
			}
		}

		if (has_served_ball)
		{
			// Collision with player paddle
			if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size,
				player_1_p, -30, player_half_size_x, player_half_size_y))
			{
				// Reset ball position
				ball_p_y = -30 + player_half_size_y + ball_half_size;
				ball_v_y *= -1;
				ball_v_x = (ball_p_x - player_1_p) * 2.0F;
			}

			// Collision with arena ceiling 
			if (ball_p_y > arena_half_size_y - ball_half_size)
			{
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_v_y *= -1;
			}

			// Collision with arena walls
			if (ball_p_x > arena_half_size_x - ball_half_size)
			{
				ball_p_x = arena_half_size_x - ball_half_size;
				ball_v_x *= -1;
			}
			else if (ball_p_x < -arena_half_size_x + ball_half_size)
			{
				ball_p_x = -arena_half_size_x + ball_half_size;
				ball_v_x *= -1;
			}
		}
		// TODO Ball should move with player until space bar is hit to serve
		else
		{
			ball_p_x = player_1_p;
			ball_p_y = -30 + player_half_size_y + ball_half_size;

			// Launch ball on space
			if (pressed(BUTTON_SPACE))
			{
				ball_v_y = initial_ball_v_y;
				ball_v_x = player_1_v;
				has_served_ball = true;
			}
		}


		// Draw ball
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, ball_color);

		// Draw player 1
		draw_rect(player_1_p, -30, player_half_size_x, player_half_size_y, 0x1976d2);

		// Draw score
		draw_number(player_1_score, -75, 40, 1, 0xeeeeee);
	}
	else
	{
		// TODO Game menu and game over
	}
}
