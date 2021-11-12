# Information

This Git repository is a direct decompilation and documentation project for the
1998 Windows PC game "Twisted Metal 2".

The goal of this project is to eventually compile an open-source Win32 executable which
can slot in place of the original executable and use the original assets.

After this project's completion, a full engine reimplementation in a modern language
with portability in mind would be the next step.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for more information.

## Building

See [BUILDING.md](BUILDING.md) for more information.

## General TM2.EXE Information

TM2PC was compiled with the Watcom C/C++ v10.[5|6] 32-bit compiler and runtime for Win32.
There are no debug symbols available for any version of TM2. The game itself is programmed
in C.

TM2PC uses various userland functions of the Win32 API. Additionally, TM2PC imports most of
the DirectX libraries to handle movies/sound/input/rendering. DirectDraw, DirectSound,
DirectInput, and DirectPlay are all imported.

The game normally requires the user to have the CD in a CD drive, even with the game installed
on the PC. It will continuously loop looking for a CD drive with the file "tm2.ico" and pull up a 
MessageBoxA asking the user to insert the CD if it is not found.

The game uses registry keys to save the install directory, CD drive letter prefix, and other minor
data.
