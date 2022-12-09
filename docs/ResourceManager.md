# EssaGUI Resource Manager

The Resource Manager is responsible for loading, searching for and giving out various resources like fonts and textures. 

All the resources are placed in the resource directory, that is detected by the required `.essaguiresources` in the root of it.

## Structure of resource directory

- `.essaguiresources` - file used for detecting EssaGUI resource directories
- `textures`
    - ... all the textures
- `fonts`
    - `gui` - default EssaGUI textures
    - ... all the fonts
- `Config.ini` - the config file
- `Theme.ini` - the theme file (see [the implementation](/EssaGUI/GUI/Theme.cpp))

## Searching for resource directory

Resources are looked up in the following paths, in the given order (`$EXEC_DIR` is the directory in which the executable is placed):

* `$EXEC_DIR/assets`, `$EXEC_DIR/../assets` all the way up to the root directory
* `$CMAKE_INSTALL_PREFIX/EssaGUI/assets`

## Config file syntax

```ini
# @string Default regular font
DefaultFont=font.ttf

# @string Default bold font
DefaultBoldFont=bold-font.ttf

# @string Default fixed width font
DefaultFixedWidthFont=fixed-width-font.ttf
```
