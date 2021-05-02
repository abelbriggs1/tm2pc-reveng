# Environment

```c
void getEnvironment (TMEnvironment * env) { // @ 0x004C45C0
  splitCmdArgs (env); // @ 0x004C465C
  
  // env->cmdArgs[0] will always be the path to the executable
  char * path = env->cmdArgs[0];
  int finalLen = lstrlenA(path);
  
  // If " is the first character, cut it off
  if (*path == '"') {
    finalLen--;
    path++;
  }
  
  // Cut off the end of the string with the executable name - gives us path to executable
  for (char * i = path[finalLen]; finalLen > 0 && *i != '\\'; i--;) {
    finalLen--;
  }
  
  qmemcpy (env->executablePath, path, finalLen);
  env->screenWidth = GetSystemMetrics (SM_CXSCREEN);
  env->screenHeight = GetSystemMetrics (SM_CYSCREEN);
  HDC screenContext = GetDC (NULL);
  env->bitsPerPixel = GetDeviceCaps (screenContext, BITSPIXEL);
  ReleaseDC (NULL, screenContext);
}
```

```c
void splitCmdArgs (TMEnvironment * env) { // @ 0x004C465C
  char * cmdLineStr = GetCommandLineA();
  BOOL quote = FALSE;
  
  while (*CmdLineStr) {
    if (env->numArgs >= 10) {
      break;
    }
    
    env->cmdArgs[env->numArgs] = cmdLineStr;
    while (*CmdLineStr && (*CmdLineStr != ' ' || quote)) {
      if (*CmdLineStr == '"') {
        // Ensure we only erase surrounding/extraneous quotes.
        quote = !quote;
      }
      ++CmdLineStr;
    }
    
    if (*CmdLineStr == ' ') {
      *CmdLineStr = 0;
      while (*CmdLineStr == ' ') {
        CmdLineStr++;
      }
    }
    
    env->numArgs++;
  }
}
```