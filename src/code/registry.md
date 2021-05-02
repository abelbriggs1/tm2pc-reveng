# Registry

`char regKeyBuffer[408]; // @ 0x00BDFB38`

`BOOL regKeyBufferEmpty = TRUE; // @ 0x004E8528`

```c
void setRegKeyPath (LPCSTR key) { // 0x004AC4B0, 0x004AC4E8 (duplicate functions)
  if (key) {
    lstrcpyA (regKeyBuffer, key);
  } else if (regKeyBufferEmpty) {
    regKeyBufferEmpty = FALSE;
    lstrcpyA (regKeyBuffer, "Software\SingleTrac\default"); // string @ 0x004E35B4
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
LSTATUS closeRegKey (PHKEY pKey) { // @ 0x004AC590
  return RegCloseKey (*pKey);
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
LSTATUS queryRegKeyValueExA (PHKEY handle, LPCSTR valueName, LPBYTE data) { // @ 0x004AC5BC
  DWORD dataSize = 4;
  return RegQueryValueExA (*handle, valueName, NULL, NULL, data, &dataSize);
}
```

