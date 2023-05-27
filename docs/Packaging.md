# Packaging of Essa apps

Most of Essa apps require some resources to run. This document describes directory structure and algorithms used for finding these resources.

## Build types

There are 2 types of build of Essa apps:

-   Production (if `CMAKE_BUILD_TYPE` is `Release`)
-   Development (otherwise)

Development builds are non optimized, contain debug features, are **not** portable and **not** reproducible (because they may contain hardcoded absolute paths). If `CMAKE_BUILD_TYPE` is `Debug`, Sanitizers (UBSAN & ASAN) are enabled unless the `ESSA_ENABLE_SANITIZERS` CMake variable is set to `0`.

Production builds are intended to be installed. Because of that, **they may error out when running from default output location**. This means that following **won't work**:

```sh
# from /build
ninja
./myapp # This **will** fail with resource not found errors!

ninja install
/usr/local/bin/myapp # This should be ok, resources are found at /usr/local/share/MyProject/myapp
```

## Directory structure

### Global installations

```
<install prefix>
├── bin
│   └── <your binaries>
├── lib
│   ├── <Essa libs>
│   └── <your libs>
└── share
    ├── Essa
    │   └── builtin
    │       ├── .essaguiresources
    │       └── <Essa resources copied from /assets>
    └── <your project>
        └── <your target>
            ├── .essaguiresources
            └── <your resources>
```

### Portable packages

**Note** that portable packages are not implemented yet.

Essa Framework assumes the following directory structure for portable production packages:

-   App binaries
-   Essa binaries [1]
-   `assets`
    - `builtin` - builtin assets (see [docs](./ResourceManager.md))
    - ... resource dirs for every target

Package-manager optimized installs use the standard Unix directory structure (with binaries in `/bin`, libraries in `/lib` and resources in `/share/$CMAKE_PROJECT_NAME/<target name>/assets`). Builtin resources are installed to `/share/Essa/builtin`.

[1] - they need to be in the same dir because most operating systems assume so
