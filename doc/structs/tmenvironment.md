# TMEnvironment

This struct is used with the environment functions (such as `envInit() @ 0x004C45C0`)
to store command line information and basic screen data/capabilities.

```c
typedef struct {
  LPSTR cmdLineStr;
  int numArgs;
  char * cmdArgs[10];
  char executablePath[400];
  char pad4[4];
  DWORD screenWidth;
  DWORD screenHeight;
  DWORD bitsPerPixel;
} TMEnvironment;
```
