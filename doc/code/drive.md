# CD Drive / Directory Functions

`static char CdDriveLetter; // @ 0x00A3A5D0`

```c
static  char pathExecutable[300]; // @ 0x00A39C68
static char pathShell[300]; // @ 0x00A39D94
static char pathDatabase[300]; // @ 0x00A39EC0
static char pathSaveGame[300]; // @ 0x00A39FEC
static char pathConfiguration[300]; // @ 0x00A3A118
static char pathSounds[300]; // @ 0x00A3A244
static char pathMovieDirectory[300]; // @ 0x00A3A370
static char pathTm2Ico[300]; // @ 0x00A3A49C
```

```c
char findCdDrive (const char * filename) { // @ 0x00499884
  char fileNameBuf[300];
  char rootPathBuf[28];

  char letter = 'A';
  while (letter <= 'Z') {
    wsprintfA (rootPathBuf, "%c:\\", letter);
    
    if (GetDriveTypeA (rootPathBuf) == DRIVE_CDROM) {
      wsprintfA (fileNameBuf, "%c:\\%s", letter, filename);
      HANDLE fileHandle = CreateFileA (
                             fileNameBuf, 
                             GENERIC_READ, 
                             FILE_SHARE_READ, 
                             NULL, 
                             OPEN_EXISTING, 
                             FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL,
                             NULL
                             );
      DWORD fileSize = (fileHandle == -1) ? 0 : GetFileSize (fileHandle, NULL);
      if (fileHandle != -1) {
        break;
      }
    }
    letter++;
  }
  
  if (letter > 'Z') {
    letter = -1;
  }
  
  CdDriveLetter = letter;
  return letter;
}
```

```c
int initPaths (const char * filename) { // @ 0x00498DDC
  PHKEY key;
  int bufSize = 300;
  char bufCdDrivePrefix[bufSize];
  char bufInstallDirectory[bufSize];
  char bufMovieDirectory[bufSize];
  LPBYTE tempDataPtr;

  lstrcpyA (pathTm2Ico, filename);
  setRegKeyPath (NULL); // @ 0x004AC4B0
  
  // Poll for a valid CD drive
  char letter = findCdDrive (filename); // @ 0x00499884
  while (letter != -1) {
    if (DisplayErrorMessage (ERR_CD_INSERT_CD) != IDRETRY) { // @ 0x004BC660
      return 0;
    }
    letter = findCdDrive (filename); // @ 0x00499884
  }
  wsprintfA (bufCdDrivePrefix, "%c:", letter);
  
  // NB: RegKey functions return zero on success
  if (openRegKey (key, HKEY_LOCAL_MACHINE, NULL)) { // @ 0x004AC528
    lstrcpyA (pathExecutable, ".");
    lstrcpyA (pathShell, ".");
    lstrcpyA (pathDatabase, ".");
    lstrcpyA (pathSaveGame, ".");
    lstrcpyA (pathConfiguration, ".");
    lstrcpyA (pathSounds, ".");
    lstrcpyA (pathMovieDirectory, '\0');
  } else {
    // We found the registry key, so try to populate paths from the installation directory.
    // If registry keys are missing, fall back to the CD drive.
    if (queryRegKey (key, "Install Directory", bufInstallDirectory, &bufSize)) { // @ 0x004AC5E4
      lstrcpyA (bufInstallDirectory, bufCdDrivePrefix);
    }
    
    if (queryRegKeyDword (key, "Executable", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
      lstrcpyA (pathExecutable, bufCdDrivePrefix);
    } else {
      lstrcpyA (pathExecutable, bufInstallDirectory);
    }
    
    if (queryRegKeyDword (key, "Shell", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
      lstrcpyA (pathShell, bufCdDrivePrefix);
    } else {
      lstrcpyA (pathShell, bufInstallDirectory);
    }
    
    if (queryRegKeyDword (key, "Database", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
      lstrcpyA (pathDatabase, bufCdDrivePrefix);
    } else {
      lstrcpyA (pathDatabase, bufInstallDirectory);
    }
    
    if (queryRegKeyDword (key, "Save Game", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
      lstrcpyA (pathSaveGame, bufCdDrivePrefix);
    } else {
      lstrcpyA (pathSaveGame, bufInstallDirectory);
    }
    
    if (queryRegKeyDword (key, "Configuration", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
      lstrcpyA (pathConfiguration, bufCdDrivePrefix);
    } else {
      lstrcpyA (pathConfiguration, bufInstallDirectory);
    }
    
    if (queryRegKeyDword (key, "Sounds", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
      lstrcpyA (pathSounds, bufCdDrivePrefix);
    } else {
      lstrcpyA (pathSounds, bufInstallDirectory);
    }
    
    if (queryRegKey (key, "Movie Directory", bufMovieDirectory, &bufSize) { // @ 0x004AC5E4
      if (queryRegKeyDword (key, "Movies", tempDataPtr) || !tempDataPtr) { // @ 0x004AC5BC
        lstrcpyA (pathMovieDirectory, '\0');
      } else {
        lstrcpyA (pathMovieDirectory, bufInstallDirectory);
      }
    } else {
      lstrcpyA (pathMovieDirectory, bufMovieDirectory);
    }
    
    closeRegKey (key); // @ 0x004AC590
  }
  return 1;
}
```


