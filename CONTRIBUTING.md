# Contributing

To contribute, you will need:

- A copy of either Ghidra or Hex-Rays IDA.
- A copy of Twisted Metal 2 PC

Experience reading x86 assembly and low-level C programming is highly recommended.
Experience reading and writing Win32 applications is helpful.

To contribute, simply open TM2.EXE in your favorite reverse engineering tool and
start adding information from this repository.

Most function definitions and static/global variables/structures will have their addresses documented with `@address`. Functions with `@address N/A` are usually wrapper functions
which are heavily likely to have existed in the codebase as public C interfaces for
a C++ module, but were optimized out due to inlining.

There are many `TODO:` comments scattered across the code indicating notable fields,
functions, or modules which need investigation.

Once you've made progress in an undocumented module/function/structure, feel free to submit
a pull request.

## Tools

### Development

Any IDE or editor can be used. If you would like your IDE files to be ignored, please submit an
issue or PR.

This repository includes a default configuration for Visual Studio Code which provides automatic
generation of Doxygen comments and automatic code formatting with the supplied `clang-format` file.
To use this default configuration, rename `.vscode/settings.json.default` to
`.vscode/settings.json`.

### Reverse Engineering

### Ghidra

[Ghidra](https://ghidra-sre.org/) is an open-source reverse engineering suite created by the
United States National Security Agency (NSA) as an alternative to the popular, but
extremely expensive, Hex-Rays IDA Pro.

As of December 2021, Ghidra
[does not support](https://github.com/NationalSecurityAgency/ghidra/issues/156) `watcall`,
the calling convention used by legacy Watcom compilers. A user of Ghidra created a custom calling
convention which works, but has several issues.

On the other hand, Ghidra's decompiler appears to produce cleaner code than IDA Pro's decompiler.

### IDA Pro

[Hex-Rays IDA Pro](https://www.hex-rays.com/ida-pro/) is the other option, an expensive but
state-of-the-art reverse engineering suite.

IDA has no trouble importing TM2.EXE and appears to be better at recognizing standard library
calls than Ghidra.

## Conventions

### Format and Spacing

Please use the supplied `.clang-format` file with `clang-format` to automatically format your
code. The `.clang-format` file is based on the Chromium style with minor modifications.

### Identifiers

- Use PascalCase for:
  - Typenames (`typedef`, `struct`, `class`, `enum`, `union`)
  - Functions
- Use snake_case for:
  - Variables
  - File names
- Use SNAKE_CASE for:
  - Constants
  - Macros
- Avoid Hungarian notation.
- Avoid capitalizing abbreviations in identifiers - prefer `TmRead()` over `TMRead()`.

### C Programming

- Use Win32 standard data types and idioms(including pointer typedefs) unless external
   functions require otherwise.

### Comments

Please use Doxygen-style comment blocks above all function prototypes/definitions and
at the top of new files.

For commenting blocks of code, please use single-line `//` comments, with blank
comment lines before and after the comment text.

For commenting single lines of code, inline comments are acceptable.

```c
/**
 * Short summary of function.
 *
 * Longer summary with more details and sentences if necessary.
 *
 * @address     0xDEADBEEF
 *
 * @param[in]   handle        The handle to do something with.
 * @param[out]  val           The pointer where the result should be written.
 *
 * @return      S_OK          The operation succeeded and a value was written
 *                            to val.
 * @return      E_INVALIDARG  handle is NULL.
 */
HRESULT DoSomething (HANDLE handle, LPDWORD val)
{
  //
  // This is a comment for a code block.
  //
  DoSomethingElse (); // This is an inline comment
  ...
}
```

When you are doing any of the following, please comment their location
in the code with the format `@address 0x(address)`. This helps other contributors
add symbols to their projects.

- Defining a global or static variable/structure which exists in TM2.EXE
- Defining a Twisted Metal function

## Directory Structure

TBD.
