#include "xkbpointer.hpp"

using namespace xkbptr;

int grab_error_handler(Display *display, XErrorEvent *error) {
	if (error->error_code == BadAccess and error->request_code == 33) {
		return 0;
	}else {
		char text[1024];
		XGetErrorText(display, error->error_code, text, sizeof(text));
		std::stringstream ss;
		ss << text << "\n"
		   << "request_code: " << (int)error->request_code << "\n"
		   << "error_code: " << (int)error->error_code;
		throw std::runtime_error(ss.str());
	}
}

void xkbpointer::grab_keys() {
	for (auto [cmd, keycode]: this->keybinds) {
		for (std::uint32_t r = 0; r <= 8; r++) {
			std::vector<bool> vec(8, false);
			std::fill(vec.end() - r, vec.end(), true);
			do {
				std::uint32_t modmask = 0;
				for (std::uint32_t i = 0; i < 8; i++) {
					if (vec[i]) {
						modmask |= this->modmasks[i];
					}
				}
				XGrabKey(this->display, keycode, modmask, this->root, true, GrabModeAsync, GrabModeAsync);
			} while (std::next_permutation(vec.begin(), vec.end()));
		}
	}
}

void xkbpointer::ungrab_keys() {
	for (auto [cmd, keycode]: this->keybinds) {
		for (std::uint32_t r = 0; r <= 8; r++) {
			std::vector<bool> vec(8, false);
			std::fill(vec.end() - r, vec.end(), true);
			do {
				std::uint32_t modmask = 0;
				for (std::uint32_t i = 0; i < 8; i++) {
					if (vec[i]) {
						modmask |= this->modmasks[i];
					}
				}
				XUngrabKey(this->display, keycode, modmask, this->root);
			} while (std::next_permutation(vec.begin(), vec.end()));
		}
	}
}

void xkbpointer::init_xmodmap() {
	auto modmap = XGetModifierMapping(this->display);
	this->xmodkeys.clear();
	if (modmap == nullptr) {
		throw std::runtime_error("Failed to get modifier mapping.");
	}
	KeyCode *keycodes = modmap->modifiermap;
	for (std::uint32_t i = 0; i < 8; i++) {
		this->xmodmap[i] = std::vector<KeyCode>();
		for (std::uint32_t j = 0; j < modmap->max_keypermod; j++) {
			if (keycodes[i * modmap->max_keypermod + j]) {
				this->xmodmap[i].push_back(keycodes[i * modmap->max_keypermod + j]);
				this->xmodkeys.insert(keycodes[i * modmap->max_keypermod + j]);
			}
		}
	}
}

void xkbpointer::wait_until_interval() {
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - this->last_processed_time);
	auto wait = this->polling_interval - elapsed;
	std::this_thread::sleep_for(std::max(wait, std::chrono::microseconds(0)));
	this->last_processed_time = std::chrono::system_clock::now();
}

std::pair<std::uint8_t, std::uint8_t> xkbpointer::keycode2idx(KeyCode kc) {
	return {kc / 8, kc % 8};
}

bool xkbpointer::command_enabled(command cmd, std::uint8_t *keymap) {
	auto keycode = this->keybinds[cmd];
	auto [byte, bit] = this->keycode2idx(keycode);
	if (this->keystatus.at(keycode) == true and ((keymap[byte] & (1 << bit)) != 0) == false) {
		this->keystatus.at(keycode) = false;
	}
	return this->keystatus.at(keycode);
}

std::pair<std::int32_t, std::int32_t> xkbpointer::pointer_delta(std::uint8_t *keymap) {
	bool left  = this->command_enabled(command::moveleft, keymap);
	bool right = this->command_enabled(command::moveright, keymap);
	bool upper = this->command_enabled(command::moveupper, keymap);
	bool lower = this->command_enabled(command::movelower, keymap);
	if (left or right or upper or lower) {
		this->pointer_velocity = std::min(
			this->pointer_acceleration + this->pointer_velocity,
			this->pointer_max_velocity);
		if (left and right and upper and lower) {
			return {0, 0};
		}
	}else {
		this->pointer_velocity = 0.0;
		return {0, 0};
	}
	double horizontal = 0.0;
	double vertical = 0.0;
	if (left) horizontal -= this->pointer_velocity;
	if (right) horizontal += this->pointer_velocity;
	if (upper) vertical -= this->pointer_velocity;
	if (lower) vertical += this->pointer_velocity;
	if (std::pow(horizontal, 2) + std::pow(vertical, 2) > std::pow(this->pointer_max_velocity, 2)) {
		horizontal /= std::sqrt(2);
		vertical /= std::sqrt(2);
	}
	return {(std::int32_t)vertical, (std::int32_t)horizontal};
}

void xkbpointer::click_button(command cmd, bool pressed) {
	button btn = this->buttonmap.at(cmd);
	bool updated = this->last_button_status.at(btn) != pressed;
	if (updated) {
		this->last_button_status.at(btn) = pressed;
		XTestFakeButtonEvent(this->display, btn, pressed, 0);
	}
}

void xkbpointer::scroll_updown(command cmd, bool pressed) {
	button btn = this->buttonmap.at(cmd);
	if (pressed) {
		if (this->scroll_count == this->scroll_interval) {
			XTestFakeButtonEvent(this->display, btn, true, 0);
			XTestFakeButtonEvent(this->display, btn, false, 0);
			this->scroll_count = 0;
		}else {
			this->scroll_count++;
		}
	}
}

void xkbpointer::handle_pointer() {
	while (!this->keystatus.at(this->keybinds.at(command::quit))) {
		std::uint8_t keymap[32];
		XQueryKeymap(this->display, (char *)keymap);
		this->update_movement(keymap);
		this->update_buttons(keymap);

		this->wait_until_interval();
	}
}

void xkbpointer::update_movement(std::uint8_t *keymap) {
	auto [y, x] = this->pointer_delta(keymap);
	if (x != 0 or y != 0) {
		XTestFakeRelativeMotionEvent(this->display, x, y, 0);
	}
}

void xkbpointer::update_buttons(std::uint8_t *keymap) {
	bool left = this->command_enabled(command::leftbutton, keymap);
	bool middle = this->command_enabled(command::middlebutton, keymap);
	bool right = this->command_enabled(command::rightbutton, keymap);
	bool up = this->command_enabled(command::scrollup, keymap);
	bool down = this->command_enabled(command::scrolldown, keymap);
	this->click_button(command::leftbutton, left);
	this->click_button(command::middlebutton, middle);
	this->click_button(command::rightbutton, right);
	this->scroll_updown(command::scrollup, up);
	this->scroll_updown(command::scrolldown, down);
}

xkbpointer::xkbpointer(
	const std::map<command, std::string>& keystrs,
	const std::uint32_t framerate,
	const std::uint32_t scrollinterval,
	const double maxvelocity,
	const double acceleration,
	const double initialvelocity):
	polling_interval(1000000/framerate), scroll_interval(scrollinterval),
	pointer_max_velocity(maxvelocity), pointer_acceleration(acceleration),
	pointer_initial_velocity(initialvelocity) {

	XSetErrorHandler(grab_error_handler);

	this->display = XOpenDisplay(nullptr);
	if (this->display == nullptr) {
		throw std::runtime_error("Failed to connect the X server.");
	}
	this->screen = DefaultScreen(this->display);
	this->root = DefaultRootWindow(this->display);
	
	this->init_xmodmap();

	this->last_processed_time = std::chrono::system_clock::now();

	this->pointer_velocity = 0.0;
	this->scroll_count = 0;

	for (auto [cmd, str]: keystrs) {
		auto keysym = XStringToKeysym(str.c_str());
		if (keysym == NoSymbol) {
			std::stringstream ss;
			ss << "Failed to covert string (" << str << ") to Keysym.";
			throw std::runtime_error(ss.str());
		}
		auto keycode = XKeysymToKeycode(this->display, keysym);
		if (keycode == 0) {
			std::stringstream ss;
			ss << "Failed to convert Keysym (" << keysym << ") to Keycode.";
			throw std::runtime_error(ss.str());
		}
		if (this->xmodkeys.contains(keycode)) {
			std::stringstream ss;
			ss << str << " (keycode: " << (int)keycode << ") is not available as keybind.";
			throw std::runtime_error(ss.str());
		}
		this->keybinds[cmd] = keycode;
		this->keystatus[keycode] = false;
		this->usedkeys.insert(keycode);
	}
	last_button_status[button::left] = false;
	last_button_status[button::middle] = false;
	last_button_status[button::right] = false;
	last_button_status[button::up] = false;
	last_button_status[button::down] = false;
	this->grab_keys();
	XAutoRepeatOff(this->display);
}

xkbpointer::~xkbpointer() {
	this->ungrab_keys();
	XAutoRepeatOn(this->display);
	XCloseDisplay(this->display);
}

void xkbpointer::mainloop() {
	XEvent event;
	auto func = [&]() {
		this->handle_pointer();
	};
	std::thread pointer_movement_thread(func);
	while (true) {
		XNextEvent(this->display, &event);
		if (event.type == KeyPress or event.type == KeyRelease) {
			KeyCode keycode = event.xkey.keycode;
			if (this->usedkeys.contains(keycode)) {
				this->keystatus.at(keycode) = event.type == KeyPress;
				if (keycode == this->keybinds.at(command::quit)) break;
			}
		}
	}
	pointer_movement_thread.join();
}
