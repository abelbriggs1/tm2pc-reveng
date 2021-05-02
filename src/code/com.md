# Windows COM

`HMODULE tmLibHandle; // @ 0x00BDFB28`

```c
void loadLibrary (HMODULE * module, LPCSTR libName) { // @ 0x004C4558
  if (*a1) {
    FreeLibrary (*a1);
    *a1 = 0;
  }
  *a1 = LoadLibraryA (libName);
}
```