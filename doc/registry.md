# Registry

| Parent Key                                           | Description          |
| ---------------------------------------------------- | -------------------- |
| `Software\\SingleTrac\\default`                      | Default parent key.  |
| `Software\\Sony Interactive\\Twisted Metal 2\\1.0.1` |                      |

| Key                 | Data Type  | Description                  |
| ------------------- | ---------- | ---------------------------- |
| `Install Directory` | REG_BINARY | Installation directory path. |
| `Executable`        | REG_BINARY | TM2 executable path.         |
| `Shell`             | REG_BINARY | `shelldb[x]` base path.      |
| `Database`          | REG_BINARY | |
| `Save Game`         | REG_BINARY | Save games path. |
| `Configuration`     | REG_BINARY | Configuration file path. |
| `Sounds`            | REG_BINARY | Sound effect path. |
| `Movies`            | REG_BINARY | Movie directory path. Has priority over `Movie Directory`. |
| `Movie Directory`   | REG_BINARY | Movie directory path. |
| `Window XPos`       | REG_DWORD  | Default horizontal position, in pixels, of TM2 window when it opens. |
| `Window YPos`       | REG_DWORD  | Default vertical position, in pixels, of TM2 window when it opens. |
| `SoundCardID`       | REG_DWORD  | Handle for default sound card. |
| `GameVolume`        | REG_BINARY | Game volume mixer structure.   |
