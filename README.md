# XKBPOINTER
- xkbpointer - A pointing device emulator using a keyboard on X11.

## Synopsis
`xkbpointer`

## Overview
- This program allows you to move a pointer on the screen by using predefined keyboard keys.
- This pointer movement replicates the functionality of a physical mouse, touchpad or trackpoint.
- Keystrokes unrelated to cursor movement are processed normally.
- Simultaneous operation with modifiers and other keys is also possible such as alt-left/rightdrag.

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

| item             | description                                                | default |
|------------------|------------------------------------------------------------|---------|
| moveupper        | keybind for move pointer upper                             | w       |
| movelower        | keybind for move pointer lower                             | s       |
| moveleft         | keybind for move pointer left                              | a       |
| moveright        | keybind for move pointer right                             | d       |
| leftbutton       | keybind for left button                                    | j       |
| middlebutton     | keybind for middle button                                  | k       |
| rightbutton      | keybind for right button                                   | l       |
| scrollup         | keybind for scroll up                                      | p       |
| scrolldown       | keybind for scroll down                                    | n       |
| quit             | keybind for quit                                           | q       |
| framerate        | frame rate of pointer position updates (frames per second) | 60      |
| scrollinterval   | scroll key repeat interval                                 | 4       |
| max velocity     | maximum pointer velocity (pixels per frame)                | 100.0   |
| acceleration     | pointer acceleration (pixels per per frame)                | 1.0     |
| initial velocity | pointer initial velocity (pixels per frame)                | 1.0     |

## Author
- Mugi Noda (void-hoge)

## License
- GPLv3
