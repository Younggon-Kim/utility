## Description
This project contains utility to set timer.

Timer class supports to trigger interval and timeout when it has been expired.

example01 describes how to set interval with given interval time.

example02 describes how to set timeout with given expired time.

## Dependencies
This project requires dependencies:
- C++11 or higher
- dlt-daemon
- gtest

dlt-daemon is not required.

If you want to remove dependency with dlt-daemon, you can just not pass DLT_ENABLED definition.

## Install Dependencies
### dlt-daemon
#### Install dependencies
```bash
$ sudo apt-get install cmake zlib1g-dev libdbus-glib-1-dev
```

#### Download dlt-daemon source code
```bash
$ git clone https://github.com/GENIVI/dlt-daemon
$ cd dlt-daemon
```

#### Build dlt-daemon
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

### gtest
#### clone source code
```bash
$ git clone https://github.com/google/googletest
$ cd googletest
```
 
#### checkout
```bash
$ git checkout tags/v1.10.x
```
 
#### build
```bash
$ mkdir build
$ cd build
$ cmake -DBUILD_SHARED_LIBS=ON ..
$ make -j$(nproc)
```
 
#### install
```bash
$ sudo make install
```

### Build Environment
Ubuntu 16.04 or higher

### Build instruction
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make -j$(nproc)
```
