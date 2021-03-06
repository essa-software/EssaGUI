# EssaGUI

A GUI framework written in C++. It was initially a component of [ESSA](https://github.com/essa-software/essa). Currently is developed along with [EssaDB](https://github.com/essa-software/EssaDB).

**EssaGUI** uses SFML for system events and OpenGL 3.2 for rendering. EssaGUI apps run a simple window manager in the main (system) window. You can open dialogs and other windows here.

## Features

* Various widgets (buttons, frames, model & view system, slider, tab view, textbox, text editor, console)
* Common dialogs (file picker, message box, text prompt)
* Simple box layout system
* Theming

## Build

From the project root directory:
```sh
mkdir build
cd build
cmake .. -GNinja
ninja
sudo ninja install
```
