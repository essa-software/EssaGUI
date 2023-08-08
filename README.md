# EssaGUI & Utils

EssaGUI is a GUI framework focused on simulations. It is largely inspired by SerenityOS's [LibGUI](https://github.com/SerenityOS/serenity/tree/master/Userland/Libraries/LibGUI) and [Qt](https://qt.io).

## Features

### Util

EssaUtil is a general-purpose supplement to C++ standard library (but still a bit biased to simulations):

- Custom strings, streams, buffers
- `TRY()` error handling - taken from Serenity and added some things like multiple errors from a single objects
- Math utilities: vectors, matrices, some geometry
- ThreadPool
- JSON
- Various other generally useful utilities - see the [source code](https://github.com/essa-software/EssaGUI/tree/main/EssaUtil)

### GUI

GUI system, because of its simulational origins and purpose, runs continuously in a main loop.

We have a custom library, [LLGL](https://github.com/essa-software/EssaGUI/tree/main/Essa/LLGL), built on top of SDL, OpenGL, GLX and a [bit of direct X11](https://github.com/essa-software/EssaGUI/blob/fb900d94082450af056a0685bd22703795c8f015/Essa/LLGL/Window/Impls/SDLHelpers.cpp#L36).

Some features include:

- Event loop (timers etc.)
- Widget system, box layouts, multiple window support
- Custom markup language ("EML")
- Common widgets and dialogs
- MDI (windows in windows)
- Tiling "window manager" for dockable UI (in progress)
- Theming
- Basic rich text

### Engine

EssaEngine implements some commonly used 3D utilities:

- OBJ models loader
- Shape renderers
- Simple lighting shaders

## Example apps

- [ESSA Space App](https://github.com/essa-software/essa)
- [EssaDB](https://github.com/essa-software/EssaDB)
- A bunch of [examples](https://github.com/essa-software/EssaGUI/tree/main/examples) and [tools](https://github.com/essa-software/EssaGUI/tree/main/EssaTools)

## How to use this for your project

**NOTE**: EssaGUI is not stable or production ready. API may change at any time (and changes frequently, actually).

See [the tutorial](https://github.com/essa-software/EssaGUI/wiki/Your-first-app) for more details.

## Documentation

There is no full documentation of EssaGUI *yet*. What is done you can read [here](https://github.com/essa-software/EssaGUI/tree/main/docs).
