# MCI Wrapper Functions

`BOOL aviVideoOpen; // @ 0x00C8511C`

```c
MCIERROR openAviVideo () { // @ 0x004B9BF0
  MCIERROR result = mciSendStringA ("open avivideo", NULL, NULL, NULL); // string @ 0x004E3770
  aviVideoOpen = (result == 0);
  return result;
}
```