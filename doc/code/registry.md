# Registry

`static char regKeyBuffer[408]; // @ 0x00BDFB38`

`static BOOL regKeyBufferEmpty = TRUE; // @ 0x004E8528`

```c
void setRegKeyPath (LPCSTR key) { // @ 0x004AC4B0, 0x004AC4E8 (duplicate functions)
  if (key) {
    lstrcpyA (regKeyBuffer, key);
  } else if (regKeyBufferEmpty) {
    regKeyBufferEmpty = FALSE;
    lstrcpyA (regKeyBuffer, "Software\SingleTrac\default");
  }
}
```

```c
void createRegKey (PHKEY result, HKEY key, LPCSTR keyName) { // @ 0x004AC550
  if (keyName) {
    RegCreateKeyExA (key, keyName, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, result, NULL);
  } else {
    RegCreateKeyExA (key, regKeyBuffer, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, result, NULL);
  }
}
```

```c
LSTATUS closeRegKey (PHKEY key) { // @ 0x004AC590
  return RegCloseKey (key);
}
```

```c
LSTATUS openRegKey (PHKEY result, HKEY key, LPCSTR keyName) { // @ 0x004AC528
  LSTATUS result;
  
  if (keyName) {
    result = RegOpenKeyExA (key, keyName, NULL, KEY_ALL_ACCESS, result);
  } else {
    result = RegOpenKeyExA (key, regKeyBuffer, NULL, KEY_ALL_ACCESS, result);
  }
}
```

```c
LSTATUS queryRegKeyDword (PHKEY key, LPCSTR keyName, LPBYTE data) { // @ 0x004AC5BC
  DWORD dataSize = 4;
  return RegQueryValueExA (*key, keyName, NULL, NULL, data, &dataSize);
}
```

```c
LSTATUS queryRegKey (PHKEY key, LPCSTR keyName, LPBYTE data, LPDWORD size) { // @ 0x004AC5E4
  return RegQueryValueExA (*key, keyName, NULL, NULL, data, size);
}
```

```c
LSTATUS setRegKeyBinary (PHKEY key, LPCSTR keyName, LPCBYTE data, DWORD size) { // @ 0x004AC65C
  return RegSetValueExA (*key, keyName, NULL, REG_BINARY, data, size);
}
```

```c
LSTATUS setRegKeyDword (PHKEY key, LPCSTR keyName, LPDWORD data) { // @ 0x004AC614
  return RegSetValueExA (*key, keyName, NULL, REG_DWORD, data, sizeof (DWORD));
}
```
