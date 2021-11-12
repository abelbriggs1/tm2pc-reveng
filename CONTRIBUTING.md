# NOTE

The directory structure and contribution process are currently in flux, as this repository is
changing from a documentation project to a decompilation project.

To contribute, you will need:

- A copy of either Ghidra or Hex-Rays IDA.
- A copy of Twisted Metal 2 PC

## Tools

The choice of reverse engineering tool is yours, as you should be able to produce similar results from both.

### Ghidra

[Ghidra](https://ghidra-sre.org/) is an open-source reverse engineering suite created by the
United States National Security Agency (NSA) as an alternative to the popular, but
extremely expensive, Hex-Rays IDA Pro.

Ghidra specifically has a couple of problems with TM2.EXE. When I first attempted to use it,
I needed to patch Ghidra to get around [#2496](https://github.com/NationalSecurityAgency/ghidra/issues/2496)
and needed to manually insert a new calling convention due to [#156](https://github.com/NationalSecurityAgency/ghidra/issues/156),
as Ghidra (at time of writing) does not support the Watcom compiler, which Twisted Metal 2 PC
was compiled with.

I was never able to get the calling convention to work quite properly and there were a number of issues.

On the other hand, I found that Ghidra's decompiler produced *much* cleaner code than IDA Pro's decompiler
and was easier to nudge into removing dead code and useless operations/variables.

### IDA Pro

[Hex-Rays IDA Pro](https://www.hex-rays.com/ida-pro/) is the other option, an expensive but state-of-the-art 
reverse engineering suite.

I was able to import TM2.EXE into IDA without any issues, as it supports Watcom off the bat. I found it to be
better at recognizing standard library calls than Ghidra, but the decompiler often produces total nonsense for me,
and the tools to remove dead code/variables are much more limited.

## Conventions

### Code

The code in this repository attempts to follow the coding style of the
[EDK2 C Coding Standards](https://edk2-docs.gitbook.io/edk-ii-c-coding-standards-specification/).
with the following specific notes:
 - Use Win32 standard data types (including pointer typedefs) instead of 
 - Do not use Hungarian notation for identifiers
 - Column limit is 100 characters
 - Function definitions do not need to be laid out vertically
 - Function definitions and calls do not need the closing parenthesis `);` on a new line
 
Please use the supplied `.clang-format` file with `clang-format` to format your code. `clang-format`
is set up to programmatically enforce much of this code style, so if you simply allow it to format
your code, there should be few issues. Identifier naming cannot be programmatically enforced, so
please ensure you follow the EDK2 standard.

### Comments

When you are doing any of the following, please comment their location 
in the code with the format `@ 0x(address)`. This helps other contributors
add symbols to their projects.
 - Defining a global or static variable/structure which exists in TM2.EXE
 - Defining a Twisted Metal function

Please use Doxygen-style comment blocks above all function prototypes and definitions.

For commenting blocks of code, please use single-line `//` comments, with blank
comment lines before and after the comment text.

For commenting single lines of code, inline comments are acceptable.

```
/**
 * Short summary of function.
 *
 * Longer summary with more details and sentences if necessary.
 *
 * Address: 0xDEADBEEF
 *
 * @param[in]   Handle        The handle to do something with.
 * @param[out]  Val           The pointer where the result should be written.
 *
 * @retval      S_OK          The operation succeeded and a value was written
 *                            to Val.
 * @retval      E_INVALIDARG  Handle is NULL.
 */
HRESULT DoSomething (IN HANDLE Handle, OUT LPDWORD Val)
{
  //
  // This is a comment for a code block.
  //
  DoSomethingElse (); // This is an inline comment
  ...
}
```

## Contributing

To contribute, simply open TM2.EXE in your favorite reverse engineering tool and
start adding information from this repository. Most function definitions and static/global
variables/structures will have their addresses documented with `@ (address)`.

Once you've made progress in an undocumented module/function/structure, feel free to submit
a pull request.

## Directory Structure

TBD.

 - `code/`: raw module/library implementations
 - `control_flow/`: basic control flow across the program, game loop functions
 - `enums/`: enumerations
 - `filetypes/`: binary file formats
 - `structs/`: raw struct definitions