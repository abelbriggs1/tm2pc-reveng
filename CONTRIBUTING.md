# Contributing

To contribute, you will need a copy of either Ghidra or Hex-Rays IDA. There are advantages
and disadvantages to both.

The choice of tool is yours, as you should be able to produce similar results from both.

## Ghidra

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

## IDA Pro

[Hex-Rays IDA Pro](https://www.hex-rays.com/ida-pro/) is the other option, an expensive but state-of-the-art 
reverse engineering suite.

I was able to import TM2.EXE into IDA without any issues, as it supports Watcom off the bat. I found it to be
better at recognizing standard library calls than Ghidra, but the decompiler often produces total nonsense for me,
and the tools to remove dead code/variables are much more limited.
