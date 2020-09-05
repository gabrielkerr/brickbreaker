struct Button_state
{
	bool is_down;
	bool has_changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	A,
	D,
	BUTTON_ENTER,
	BUTTON_SPACE,

	BUTTON_COUNT // Should always be last
};

struct Input
{
	Button_state buttons[BUTTON_COUNT];
};
