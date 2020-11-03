## Description
This project contains utility to print log message at stdout, a file, and DLT.

example01 describes how to print log messages at stdout.

example02 describes how to print log messages at a file.

example03 describes how to print log messages at DLT.

## Dependencies
This project requires dependencies:
- dlt-daemon

## Install Dependencies
[dlt-daemon]
### Install dependencies
```bash
$ sudo apt-get install cmake zlib1g-dev libdbus-glib-1-dev
```

### Download dlt-daemon source code
```bash
$ git clone https://github.com/GENIVI/dlt-daemon
$ cd dlt-daemon
```

### Build dlt-daemon
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## Build Environment
Ubuntu 16.04 or higher

## Build instruction
```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=<debug|release> ..
$ make -j$(nproc)
```

## Build options
If you want to disable logging, then pass `release` option to CMAKE_BUILD_TYPE.

Logger won't print any log messages, even does not add any logging code at compile time. 
