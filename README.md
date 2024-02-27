# XKBPOINTER
- xkbpointer - A pointing device emulator using a keyboard on X11.

## Synopsis
- `xkbpointer`: normal (toggle) mode
- `xkbpointer [-m keystring | --momentary keystring]`: momentary mode
  - 

## Overview
- This program allows you to move a pointer on the screen by using predefined keyboard keys.
- This pointer movement replicates the functionality of a physical mouse, touchpad or trackpoint.
- Keystrokes unrelated to cursor movement are processed normally.
- If you start it without any options, it will start in normal (toggle) mode.
  - Simultaneous operation with modifiers and other keys is also possible such as alt-left/rightdrag.
  - You can toggle it every time you run it using the command as below.
	- `pkill xkbpointer && xset r || xkbpointer &`
- If you start it with `-m` or `--momentary` option, it will start in momentary mode.
  - In this mode, the pointer can only be manipulated while the key is hold down that specified in the option `keystring`.
  - Unfortunately, simultaneous operation with modifier keys does not work very well.
	- This is because another program that detects the operation will detect that an unrelated key (`keystring` key) is being pressed at the same time.
	- If anyone comes up with a solution to this, please send a PR.

## Depends
- X11
- Xlib
- XTEST extension
- C++20
- CMake
- GNU Make

## Build
- `git clone https://github.com/void-hoge/xkbpointer.git`
- `cd xkbpointer`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

## Configurations
- The config file path is `$HOME/.config/xkbpointer.conf`.
- An example of config file is available in [here](./xkbpointer.conf).
- A single key can be used as each keybindings (modifier and combination keys are not available).

| item            | description                                                | default |
|-----------------|------------------------------------------------------------|---------|
| moveupper       | keybind for move pointer upper                             | w       |
| movelower       | keybind for move pointer lower                             | s       |
| moveleft        | keybind for move pointer left                              | a       |
| moveright       | keybind for move pointer right                             | d       |
| leftbutton      | keybind for left button                                    | j       |
| middlebutton    | keybind for middle button                                  | k       |
| rightbutton     | keybind for right button                                   | l       |
| scrollup        | keybind for scroll up                                      | p       |
| scrolldown      | keybind for scroll down                                    | n       |
| quit            | keybind for quit                                           | q       |
| framerate       | frame rate of pointer position updates (frames per second) | 60      |
| scrollinterval  | scroll key repeat interval                                 | 4       |
| maxvelocity     | maximum pointer velocity (pixels per frame)                | 100.0   |
| acceleration    | pointer acceleration (pixels per per frame)                | 1.0     |
| initialvelocity | pointer initial velocity (pixels per frame)                | 1.0     |

## Author
- Mugi Noda (void-hoge)

## License
- GPLv3
