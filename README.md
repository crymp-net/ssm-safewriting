# SSM SafeWriting 2.9

This repository serves as an archive of SSM SafeWriting 2.9 source code. It is no longer maintained, but if anyone wants, they can still modify and compile SSM on their own.

## Structure

- `cpp/` directory contains all source code required to compile SSM DLL
- `lua/` directory contains both `Mods/` source code and `ServerFolder/` sources

## Compiling DLL
In order to compile the DLL, open `src/SafeWriting.sln` in Visual Studio. Make sure that when you go to Project > Include Directories (for compiler), `SafeWriting/CryEngine` is listed there. If not, add it, otherwise the project won't compile! The only supported target is Bin32, Bin64 offsets weren't implemented yet.

## Other notes

Besides for official release, you can also find source code of LevelDesigner in this repository. Also please note that all of this code is legacy, so code standards might not be high + some of parts of code might not even be in English.