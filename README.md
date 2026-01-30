# testforme

## Hello World Program

This directory contains test programs written in C.

### Hello World
Simple "Hello, World!" program to test basic C compilation.

#### Building and Running Hello World
```bash
gcc -o hello_world hello_world.c
./hello_world
```

### MP3 Player
A more complex program that decodes and plays MP3 files using libmpg123 and ALSA.

#### Dependencies for MP3 Player
This program requires:
- libmpg123-dev (for MP3 decoding)
- libasound2-dev (for audio playback)

Install dependencies on Debian/Ubuntu:
```bash
sudo apt-get install -y libmpg123-dev libasound2-dev
```

#### Building All Programs
```bash
make
```

#### Usage

Play an MP3 file:
```bash
./mp3_player <mp3_file>
```

## Example

```bash
./hello_world
./mp3_player example.mp3
```

## Features

- Simple "Hello, World!" program
- MP3 decoder using libmpg123
- Audio playback through ALSA
- Makefile for easy compilation