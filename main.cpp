#include "xkbpointer.hpp"

#include <cstdlib>
#include <fstream>

using namespace xkbptr;

std::map<std::string, command> str2command = {
	{"moveupper",    command::moveupper   },
	{"movelower",    command::movelower   },
	{"moveleft",     command::moveleft    },
	{"moveright",    command::moveright   },
	{"leftbutton",   command::leftbutton  },
	{"middlebutton", command::middlebutton},
	{"rightbutton",  command::rightbutton },
	{"scrollup",     command::scrollup    },
	{"scrolldown",   command::scrolldown  },
	{"quit",         command::quit        }
};

std::map<std::string, std::string> default_config = {
	{"framerate",       "60"   },
	{"scrollinterval",  "4"    },
	{"maxvelocity",     "100.0"},
	{"acceleration",    "1.0"  },
	{"initialvelocity", "1.0"  }
};

std::map<command, std::string> default_keybinds = {
	{command::moveupper,    "w"},
	{command::movelower,    "s"},
	{command::moveleft,     "a"},
	{command::moveright,    "d"},
	{command::leftbutton,   "j"},
	{command::middlebutton, "k"},
	{command::rightbutton,  "l"},
	{command::scrollup,     "p"},
	{command::scrolldown,   "n"},
	{command::quit,	        "q"}
};

int main() {
	std::stringstream ss;
	ss << std::getenv("HOME") << "/.config/xkbpointer.conf";
	std::string configfile = ss.str();
	std::ifstream ifs(configfile);
	auto keybinds = default_keybinds;
	auto config = default_config;
	if (ifs) {
		std::string item, value;
		while (ifs >> item) {
			ifs >> value;
			try {
				keybinds.at(str2command.at(item)) = value;
			} catch (const std::exception& e) {
				try {
					config.at(item) = value;
				} catch (const std::exception& e) {
					std::stringstream ss;
					ss << "There is no item \"" << item << "\" for configuration of xkbpointer.";
					throw std::invalid_argument(ss.str());
				}
			}
		}
	}
	auto xkbptr = xkbpointer(
		keybinds,
		std::stoi(config.at("framerate")),
		std::stoi(config.at("scrollinterval")),
		std::stod(config.at("maxvelocity")),
		std::stod(config.at("acceleration")),
		std::stod(config.at("initialvelocity")));
	xkbptr.mainloop();
}
