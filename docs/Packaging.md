# Packaging of Essa apps

Most of Essa apps require some resources to run. This document describes directory structure and algorithms used for finding these resources.

## Build types

There are 2 types of build of Essa apps:

-   Production (if `CMAKE_BUILD_TYPE` is `Release`)
-   Development (otherwise)

Development builds are non optimized, contain debug features, are **not** portable and **not** reproducible (because they may contain absolute paths). If `CMAKE_BUILD_TYPE` is `Debug`, Sanitizers (UBSAN & ASAN) are enabled unless the `ESSA_ENABLE_SANITIZERS` CMake variable is set to `0`.

Production builds are intended to be installed.

## Directory structure

**Note** that portable packages are not implemented yet.

Essa Framework assumes the following directory structure for portable production packages:

-   App binaries
-   Essa binaries [1]
-   `assets`
    - `builtin` - builtin assets (see [docs](./ResourceManager.md))
    - ... resource dirs for every target

Package-manager optimized installs use the standard Unix directory structure (with binaries in `/bin`, libraries in `/lib` and resources in `/share/$CMAKE_PROJECT_NAME/<target name>/assets`). Builtin resources are installed to `/share/Essa/builtin`.

[1] - they need to be in the same dir because most operating systems assume so
