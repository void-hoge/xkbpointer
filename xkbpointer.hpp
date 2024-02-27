#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <thread>
#include <exception>
#include <stdexcept>
#include <atomic>
#include <optional>

namespace xkbptr {

enum command {
	moveupper,
	movelower,
	moveleft,
	moveright,
	leftbutton,
	middlebutton,
	rightbutton,
	scrollup,
	scrolldown,
	quit
};

enum button {
	left = 1,
	middle = 2,
	right = 3,
	up = 4,
	down = 5
};

class xkbpointer {
private:
	Display *display;
	std::int32_t screen;
	Window root;

	const std::map<command, button> buttonmap = {
		{command::leftbutton, button::left},
		{command::middlebutton, button::middle},
		{command::rightbutton, button::right},
		{command::scrollup, button::up},
		{command::scrolldown, button::down}
	};

	const std::array<uint32_t, 8> modmasks = {
		ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask, Mod5Mask
	};

	std::map<command, KeyCode> keybinds;
	std::map<KeyCode, std::atomic_bool> keystatus;
	std::set<KeyCode> xmodkeys;
	std::set<KeyCode> usedkeys;
	std::array<std::vector<KeyCode>, 8> xmodmap;
	std::map<button, bool> last_button_status;

	const std::chrono::microseconds polling_interval;
	std::chrono::system_clock::time_point last_processed_time;

	const double pointer_max_velocity;
	const double pointer_acceleration;
	const double pointer_initial_velocity;
	double pointer_velocity;

	const uint32_t scroll_interval = 4;
	uint32_t scroll_count;

	void wait_until_interval();

	void grab_keys();
	void ungrab_keys();
	void init_xmodmap();

	KeyCode string2keycode(const std::string& str) const;
	std::pair<std::uint8_t, std::uint8_t> keycode2idx(KeyCode kc);
	bool command_enabled(command cmd, std::uint8_t *keymap);

	std::pair<std::int32_t, std::int32_t> pointer_delta(std::uint8_t *keymap);
	void click_button(command cmd, bool pressed);
	void scroll_updown(command cmd, bool pressed);
	void handle_pointer();
	void momentary_handle_pointer(KeyCode mokey);
	void update_movement(std::uint8_t *keymap);
	void update_buttons(std::uint8_t *keymap);
public:
	xkbpointer(
		const std::map<command, std::string>& keys,
		const uint32_t frame_rate,
		const uint32_t scroll_interval,
		const double max_velocity,
		const double acceleration,
		const double initialvelocity);
	~xkbpointer();
	void mainloop();
	void momentary_mode(const std::string& momentary_key);
};

}; // namespace xkbptr
