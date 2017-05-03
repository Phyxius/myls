# `myls`

This project is a recreation of a subset of the functionality of the standard `ls` command, written as a programming lab for CS481 (Operating Systems).

## Usage
`myls [OPTIONs] [FILEs]`

### Valid options:
- `-c`: Classify &mdash; Append `/` to directories, `@` to links, and `*` to executables.
- `-d`: Disk Usage; report file size on disk based on its number of allocated blocks and a parameter that specifies block size (in bytes)
- `-f`: Follow symlinks
- `-h`: Report human-readable file sizes
- `-l`: Use long listing format, the same as normal `ls` but also displaying inode number.
- `-r`: Recurse

If no directories are specified, information about the current directory will be reported instead.

## Building
A CMake file and a Makefile are provided; both work equally well. Requires several Linux and BSD libraries, so it will probably not build on Windows except via Cygwin.
