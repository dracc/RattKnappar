# Rattknappar

(Swedish, "steering wheel buttons") - software side of the [wheelbuttons](https://github.com/dracc/wheelbuttons) project.

## Pre-requisites
CMake 3.17+
gcc-arm-none-eabi

### Debian-based
`sudo apt install cmake gcc-arm-none-eabi`

## Building
All you need apart from cmake and the compiler will be downloaded when you run cmake.
```sh
git clone https://github.com/dracc/rattknappar
cd rattknappar
mkdir build
cmake -B build -S .
cmake --build build
```

## License
MIT