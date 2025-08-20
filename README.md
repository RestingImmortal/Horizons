# Horizons

Horizons is an engine in active, early development with the goal of having things be handled expressly through data rather than through code in the engine itself.

### What This Is
* An open-source engine for 2D top-down space games

### What This Is Not
* A game
* A finished project

---

## Usage Instructions

### How To Run

The engine expects an `assets` directory in the folder alongside the executable.
It expects a file.start.json, with a name of `Multi-start unsupported still. Harass Luna if this is limiting you` and an initial map field.
This should point to a map with a declared meta/name of whatever you specify. The map needs the backgrounds, ships, and objects sections.

You can make the appropriate files following the structure expected by the [asset manager](src/AssetManager.cpp).
For this, you can use either json or xml, depending on your preference. Both are currently supported. In practice, xml support lags behind slightly.

The engine also expects a file called META.json next to the executable. This expects a title field currently.

## Compilation Instructions

### Prerequisites

* CMake (>=3.22)
* C++ compiler supporting C++23 features such as `<print>` and `<expected>`.
* Make or Ninja
* Git
* Approximately 750MB of storage space for fetching and building the required dependencies.

### Build Instructions

As a basic CMake project, you can build as usual, or if on Linux can use the Stella script.
For Stella, this is as simple as running `./stella.sh [build/release]`. Without, you can do the following:

```shell
mkdir build
cd build
cmake ..
cmake --build .
cd ..
```
You can add flags as desired.

### Cross-compilation

Currently, there is support for utilizing Zig as a way of compiling a Windows executable from a Linux environment.
This is mostly just a hacky solution to allow people to test it on Windows systems without needing to set up a dev environment on their installation.
It works, but has some flaws such as the executable being falsely flagged as malicious. Ideally, it should be ignored.

## Contributors
* Should you wish to contribute to the project, read through the [contributing text](docs/CONTRIBUTING.md).
* In interactions with the community of this project, including but not limited to submitting PRs and opening issues, you are expected to follow the [code of conduct](docs/CODE_OF_CONDUCT.md).

## Licensing
* The source in this repository is held under the Apache 2 license. Other files may be held under other licenses, such as some documentation.
In the case of such, it will have a copyright notice in the file.
