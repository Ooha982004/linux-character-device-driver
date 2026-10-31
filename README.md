# Linux Character Device Driver

## Project Overview
A simple Linux kernel character device driver developed in C on Ubuntu Linux.

The driver allows user-space applications to communicate with kernel space through a character device file.

## Features
- Dynamic device number allocation
- Character device registration using `cdev`
- Automatic device file creation
- Read and write operations
- User-space/kernel-space data transfer
- 1024-byte buffer with overflow protection
- Mutex-based buffer synchronization
- Kernel logging using `pr_info()`

## Technologies Used
- C
- Linux Kernel Modules
- Ubuntu Linux
- GCC
- Make

## Prerequisites

- Ubuntu Linux
- GCC compiler
- Make utility
- Linux kernel headers
- Administrator (sudo) privileges

## Project Structure
- `hello.c` – Driver implementation
- `Makefile` – Build configuration
- `README.md` – Project documentation

## Build
```bash
make
```

## Load the Driver
```bash
sudo insmod hello.ko
```

## Check Device
```bash
ls -l /dev/mychardev
```

## Test Read and Write
```bash
sudo chmod 666 /dev/mychardev
echo "Hello Linux Driver" > /dev/mychardev
cat /dev/mychardev
```

## Unload the Driver
```bash
sudo rmmod hello
```
## Kernel Logs

To view driver messages:

sudo dmesg | tail -20

## Buffer Limit
The driver uses a fixed 1024-byte buffer and returns `ENOSPC` when the buffer is full.

## Author
Ooha

