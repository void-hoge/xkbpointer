#include "xkbpointer.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>

//#define DEBUG

using namespace xkbptr;

const std::map<std::string, command> str2command = {
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

const std::map<std::string, std::string> default_config = {
	{"framerate",       "60"   },
	{"scrollinterval",  "4"    },
	{"maxvelocity",     "100.0"},
	{"acceleration",    "1.0"  },
	{"initialvelocity", "1.0"  }
};

const std::map<command, std::string> default_keybinds = {
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

int main(const int argc, const char** argv) {
	std::stringstream ss;
#if defined(DEBUG)
	ss << "../xkbpointer.conf";
#else
	ss << std::getenv("HOME") << "/.config/xkbpointer.conf";
#endif
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
	if (argc == 1) {
		xkbptr.mainloop();
	}else if (argc == 3) {
		if (std::string(argv[1]) != std::string("-m") and
			std::string(argv[1]) != std::string("--momentary")) {
			std::stringstream ss;
			ss << "Invalid command line option " << argv[1] << ".";
			throw std::invalid_argument(ss.str());
		}
		std::string mokey(argv[2]);
		xkbptr.momentary_mode(mokey);
	}else {
		std::cerr << "usage: xkbpointer [-m keystring | --momentary keystring]" << std::endl;
		return -1;
	}
}
