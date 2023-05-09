# EssaGUI Resource Manager

The Resource Manager is responsible for loading, searching for and giving out various resources like fonts and textures.

All the resources are placed in the resource directory, that is detected by the required `.essaguiresources` in the root of it.

## Structure of resource directory

-   `.essaguiresources` - file used for detecting EssaGUI resource directories
-   `textures`
    -   ... all the textures
-   `fonts`
    -   `gui` - default EssaGUI textures
    -   ... all the fonts
-   `Config.ini` - the config file
-   `Theme.ini` - the theme file (see [the implementation](/EssaGUI/GUI/Theme.cpp))

## Searching for resource directory

In these paths:

-   `$EXEC_DIR` is the directory in which the executable is placed.

For production builds, the following paths are used:

-   `$EXEC_DIR/assets/<target name>`, `$EXEC_DIR/../assets/<target name>` all the way up to the root directory
-   `$CMAKE_INSTALL_PREFIX/share/$CMAKE_PROJECT_NAME/<target name>`
-   `$CMAKE_INSTALL_PREFIX/share/Essa/builtin` (Essa builtin resources)

For development builds, the following paths are used:

-   Path specified by `essa_resources()` CMake function (absolute path to source-local asset root, baked into executable)
-   `$CMAKE_SOURCE_DIR/assets` (Essa builtin resources)

## Config file syntax

```ini
# @string Default regular font
DefaultFont=font.ttf

# @string Default bold font
DefaultBoldFont=bold-font.ttf

# @string Default fixed width font
DefaultFixedWidthFont=fixed-width-font.ttf
```
