#include "utils.cpp"
#include <windows.h>

global_variable bool running = true;

struct Render_State {
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};

global_variable Render_State render_state;

#include "platform_common.cpp"
#include "renderer.cpp"
#include "brick.h"
#include "game.cpp"

// This is a window process function. Handles messages that are dispatched in WinMain
LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (uMsg) {

		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int size = render_state.width * render_state.height * sizeof(unsigned int);

			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Create Window Class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Game Window Class";
	window_class.lpfnWndProc = window_callback;
	
	// Register Class
	RegisterClass(&window_class);

	// Create Window
	HWND window = CreateWindow(window_class.lpszClassName, L"Breakout From Scratch", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	// Declare input struct before jumping into the game loop.
	Input input = {};

	// Set initial delta time to 60fps
	float delta_time = 0.016666F;

	// Get frame begin time (in CPU counts)
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);

	// Get performance frequency (CPU counts per second)
	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}

	// Initialize the game
	initialize_game();

	while (running) 
	{
		// Input
		MSG message;

		// Set all input buttons to unchanged for the new frame.
		for (size_t i(0); i < BUTTON_COUNT; ++i)
		{
			input.buttons[i].has_changed = false;
		}

		// While there are messages to be handled, DO WORK
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) 
		{
			switch (message.message)
			{
				// Handle keyup and keydown messages 
				case WM_KEYUP:
				case WM_KEYDOWN:
				{
					// Get the key code
					// wParam is the virtual keycode only for key related messages.
					u32 vk_code = (u32)message.wParam;

					// Check to see if key is down
					// bit 31 of lParam is the current keystate. 0 for down, 1 for up. 
					bool is_down = ((message.lParam & (1 << 31)) == 0);

					// Macro for handling directional button state
					#define process_button(b, vk) \
					case vk: {\
						input.buttons[b].has_changed = (is_down != input.buttons[b].is_down); \
						input.buttons[b].is_down = is_down; \
					} break;

					// If the key is down, set changed
					switch (vk_code)
					{
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(A, 'A');
						process_button(D, 'D');
						process_button(BUTTON_ENTER, VK_RETURN);
						process_button(BUTTON_SPACE, VK_SPACE);
					}
				} break;
				default:
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				} 
			}
		}


		// Simulate
		simulate_game(&input, delta_time);

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// Get end frame time (in CPU counts)
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);

		// Divide compute time by frequency to get delta time
		// (end counts - begin counts) / (counts / second) = seconds
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;

		// set begin frame time to current end frame time
		frame_begin_time = frame_end_time;
	}

	// Clean up game objects.
	clean_up_game();

}
