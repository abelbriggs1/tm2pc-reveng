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

## Contributing

To contribute, simply open TM2.EXE in your favorite reverse engineering tool and
start copying information from this repository. Most function definitions, static/global
variables, and function calls will have their addresses documented with `// @ (address)`.

Once you've made progress in an undocumented module/function/structure, feel free to submit
a pull request.

## Directory Structure

 - `code/`: raw module/library implementations
 - `control_flow/`: basic control flow across the program, game loop functions
 - `enums/`: enumerations
 - `filetypes/`: binary file formats
 - `structs/`: raw struct definitions
 
## Conventions

 - This repository is effectively a **specification**, *not* a codebase (that comes later!).
   Code is the documentation, so prefer readability and add comments if something's not obvious.
 - When you are doing any of the following, please comment their location 
   in the code in a single line comment with the format `// @ 0x(address)`.
   This helps other contributors add symbols to their projects.
   - Referencing a global variable
   - Referencing a Twisted Metal function (not Win32 or standard library)
   - Declaring a global or static variable
   - Declaring a Twisted Metal function
 - Two spaces per indent, avoid tabs. Win32 function calls get *loooong*.