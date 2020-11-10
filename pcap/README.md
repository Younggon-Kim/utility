## Description
This project contains utility to capture packets and dump as a file or notify to callback function.

You can see the dump file at wireshark.

example01 describes how to capture packets and dump them into callback function.

example02 describes how to capture packets and dump them into a file.

## Dependencies
This project requires dependencies:
- dlt-daemon
- libpcap
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

### libpcap
```bash
sudo apt install libpcap-dev
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

## Build Environment
Ubuntu 16.04 or higher

## Build instruction
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make -j$(nproc)
```

If you want to build with DLT, then pass -DDLT_ENABLED when you execute cmake

e.g. 
```bash
$ cmake .. -DDLT_ENABLED=TRUE
```

## Example instruction
These examples require super user permission to capture packets.

```bash
sudo ./example01 -i <interface name>

sudo ./example01 -i <interface name> -f "TCP PORT 443"

sudo ./example02 -i <interface name>

sudo ./example02 -i <interface name> -f "TCP PORT 443" -w "packetdump"
```