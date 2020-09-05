#ifndef __RENDERER_CPP__
#define __RENDERER_CPP__

internal void
clear_screen(u32 color) {
	unsigned int* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

internal void
draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {
	
	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	for (int y = y0; y < y1; y++) {
		u32* pixel = (u32*)render_state.memory + x0 + y*render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

global_variable float render_scale = 0.01f;

internal void
draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color) {

	x *= render_state.height*render_scale;
	y *= render_state.height * render_scale;
	half_size_x *= render_state.height * render_scale;
	half_size_y *= render_state.height * render_scale;

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;

	// Change to pixels
	int x0 = static_cast<int>(x - half_size_x);
	int x1 = static_cast<int>(x + half_size_x);
	int y0 = static_cast<int>(y - half_size_y);
	int y1 = static_cast<int>(y + half_size_y);

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}

// Alphabet
const char* letters[][7] =
{
	" 00 ",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000 ",
	"0  0",
	"0  0 ",
	"000 ",
	"0  0",
	"0  0",
	"000 ",

	"0000",
	"0  0",
	"0   ",
	"0   ",
	"0   ",
	"0  0",
	"0000",
	
	"000 ",
	"0  0",
	"0  0",
	"0  0",
	"0  0 ",
	"0  0",
	"000 ",

	"0000",
	"0   ",
	"0   ",
	"0000",
	"0   ",
	"0   ",
	"0000",

	"0000",
	"0   ",
	"0   ",
	"0000",
	"0   ",
	"0   ",
	"0   ",

	"0000",
	"0  0",
	"0   ",
	"0 00",
	"0  0",
	"0  0",
	"0000",

	"0  0",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"0000",
	" 0  ",
	" 0  ",
	" 0  ",
	" 0  ",
	" 0  ",
	"0000",

	"0000",
	"  0 ",
	"  0 ",
	"  0 ",
	"0 0 ",
	"0 0 ",
	"000 ",

	"0  0",
	"0  0",
	"0 0 ",
	"00  ",
	"0 0 ",
	"0  0",
	"0  0",

	"0   ",
	"0   ",
	"0   ",
	"0   ",
	"0   ",
	"0   ",
	"0000",

	"0   0",
	"00 00",
	"0 0 0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
		
	"0   0",
	"00  0",
	"0 0 0",
	"0  00",
	"0   0",
	"0   0",
	"0   0",

	"0000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",

	"000 ",
	"0  0",
	"0  0",
	"000 ",
	"0   ",
	"0   ",
	"0   ",
		
	"0000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",
	"   0",

	"000 ",
	"0  0",
	"0  0",
	"000 ",
	"0  0",
	"0  0",
	"0  0",

	"0000",
	"0   ",
	"0   ",
	" 00 ",
	"   0",
	"   0",
	"0000",

	"0000",
	"  0 ",
	"  0 ",
	"  0 ",
	"  0 ",
	"  0 ",
	"  0 ",

	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",

	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00 ",

	"0   0",
	"0   0",
	"0   0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	" 000 ",

	"0   0",
	"0   0",
	" 0 0 ",
	"  0  ",
	" 0 0 ",
	"0   0",
	"0   0",

	"0   0",
	"0   0",
	" 0 0 ",
	"  0  ",
	"  0  ",
	"  0  ",
	"  0  ",

	"0000",
	"   0",
	"   0",
	" 00 ",
	"0   ",
	"0   ",
	"0000",
};

// Function that takes in string and draws letters with rectangles 
internal void draw_text(const char* text, float x, float y, float size, u32 color)
{
	float half_size = size / 2.0F;
	float origin_x = x;
	float origin_y = y;

	// Get first row of letters
	size_t text_ix = 0;
	while (text[text_ix])
	{
		const char** letter = letters[text[text_ix] - 'A'];

		// For each row, draw a square if there is a zero.
		for (int i(0); i < 7; ++i)
		{
			const char* letter_row = letter[i];
			while (*letter_row)
			{
				if (*letter_row == '0')
				{
					draw_rect(x, y, half_size, half_size, color);
				}
				x += size;
				letter_row++;
			}
			x = origin_x;
			y -= size;
		}

		x += 6 * size;
		origin_x = x;
		y = origin_y;
		text_ix++;
	}
}

internal void draw_number(int number, float x, float y, float size, u32 color)
{
	// Number is composed of 15 rectangles
	float rect_half_size = size * 0.5F;
	float x_offset = 0;
	bool did_draw_number = false;

	// Modulo the number to cycle each digit
	while (number > 0 || !did_draw_number)
	{
		int digit = number % 10;
		did_draw_number = true;
		switch (digit)
		{
			case 0:
			{
				draw_rect(x - 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
				draw_rect(x + x_offset, y + 2, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
			} break;
			case 1:
			{
				draw_rect(x + x_offset, y, rect_half_size, rect_half_size * 5, color);
			} break;
			case 2:
			{
				draw_rect(x + x_offset, y + 2, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y, rect_half_size * 3, rect_half_size, color);
				draw_rect(x - 1 + x_offset, y - 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size * 3, rect_half_size, color);
			} break;
			case 3:
			{
				draw_rect(x + x_offset, y + 2, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y - 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size * 3, rect_half_size, color);
			} break;
			case 4:
			{
				draw_rect(x + 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
				draw_rect(x - 1 + x_offset, y + 1, rect_half_size, rect_half_size * 3, color);
				draw_rect(x + x_offset, y, rect_half_size, rect_half_size, color);
			} break;
			case 5:
			{
				draw_rect(x + x_offset, y + 2, rect_half_size * 3, rect_half_size, color);
				draw_rect(x - 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y - 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size * 3, rect_half_size, color);
			} break;
			case 6:
			{
				draw_rect(x + x_offset, y + 2, rect_half_size * 3, rect_half_size, color);
				draw_rect(x - 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y - 1, rect_half_size, rect_half_size, color);
				draw_rect(x - 1 + x_offset, y - 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size * 3, rect_half_size, color);
			} break;
			case 7:
			{
				draw_rect(x + 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
				draw_rect(x + x_offset, y + 2, rect_half_size * 3, rect_half_size, color);
				draw_rect(x - 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
			} break;
			case 8:
			{
				draw_rect(x - 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
				draw_rect(x + x_offset, y + 2, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
			} break;
			case 9:
			{
				draw_rect(x + x_offset, y + 2, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
				draw_rect(x - 1 + x_offset, y + 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y, rect_half_size * 3, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y - 1, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size * 3, rect_half_size, color);
			} break;
			default:
			{
				draw_rect(x - 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
				draw_rect(x + x_offset, y + 2, rect_half_size, rect_half_size, color);
				draw_rect(x + x_offset, y - 2, rect_half_size, rect_half_size, color);
				draw_rect(x + 1 + x_offset, y, rect_half_size, rect_half_size * 5, color);
			} break;
		}
		
		// Divide number by 10 so modulo of next digit grabs the next number
		number /= 10;

		// Set offset for next number
		x_offset -= 5;

		if (digit == 1)
		{
			x_offset += 2;
		}
	}
	
}

#endif