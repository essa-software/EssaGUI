# EssaGUI

A GUI framework written in C++. It was initially a component of [ESSA](https://github.com/essa-software/essa). Currently is developed along with [EssaDB](https://github.com/essa-software/EssaDB).

**EssaGUI** uses [LLGL](/Essa/LLGL) and OpenGL 3.2 as rendering backends. EssaGUI apps run a simple window manager in the main (system) window. You can open dialogs and other windows here.

## Features

* Various widgets (buttons, frames, model & view system, slider, tab view, textbox, text editor, console)
* Common dialogs (file picker, message box, text prompt)
* Simple box layout system
* Theming

## Build

You need to install [EssaUtil](https://github.com/essa-software/util) according to its build instructions.

From the project root directory:
```sh
mkdir build
cd build
cmake .. -GNinja
ninja
sudo ninja install
```
