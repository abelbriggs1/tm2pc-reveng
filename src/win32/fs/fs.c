/**
 * @file fs.c
 *
 * Win32 filesystem functions.
 */

#include <windows.h>

#include "error/error.h"
#include "registry/registry.h"

#include "fs.h"

#define PATH_LEN 300

static CHAR drive_letter;               // @address 0x00A3A5D0
static CHAR path_executable[PATH_LEN];  // @address 0x00A39C68
static CHAR path_shell[PATH_LEN];       // @address 0x00A39D94
static CHAR path_database[PATH_LEN];    // @address 0x00A39EC0
static CHAR path_saves[PATH_LEN];       // @address 0x00A39FEC
static CHAR path_config[PATH_LEN];      // @address 0x00A3A118
static CHAR path_sounds[PATH_LEN];      // @address 0x00A3A244
static CHAR path_movies[PATH_LEN];      // @address 0x00A3A370
static CHAR path_icon[PATH_LEN];        // @address 0x00A3A49C

/**
 * Verify that the TM2 CD is inserted in a CD drive by looking for the given
 * file path.
 *
 * @address        0x00499884
 *
 * @param[in]      file_path           Relative path from the drive cd_root for a file whose
 *                                     existence verifies the TM2 CD.
 * @return         CHAR                Win32 CD drive letter if the TM2 CD is inserted, -1
 *                                     otherwise.
 */
CHAR TmFsVerifyCdDrive (LPCSTR file_path)
{
  CHAR abs_path[PATH_LEN];
  CHAR cd_root[28];

  CHAR letter = 'A';
  while (letter <= 'Z') {
    wsprintfA (cd_root, "%c:\\", letter);

    if (GetDriveTypeA (cd_root) == DRIVE_CDROM) {
      wsprintfA (abs_path, "%c:\\%s", letter, file_path);
      HANDLE handle = CreateFileA (abs_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                   FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
      DWORD size = (handle == -1) ? 0 : GetFileSize (handle, NULL);
      if (handle != -1) {
        break;
      }
    }
    letter++;
  }

  if (letter > 'Z') {
    letter = -1;
  }

  drive_letter = letter;
  return letter;
}

/**
 * Initialize filesystem paths for game files.
 *
 * TODO: This function is extremely hard to read in IDA and probably needs a look-over
 * for any missing logic.
 *
 * @address        0x00498DDC
 *
 * @param[in]      filename            Icon file name.
 *
 * @return         0                   Initialization failed.
 * @return         1                   Initialization was successful.
 */
DWORD TmFsInit (LPCSTR filename)
{
  CHAR cd_root[PATH_LEN];
  CHAR install_dir[PATH_LEN];
  CHAR movie_dir[PATH_LEN];

  lstrcpyA (path_icon, filename);
  TmRegistryInit (NULL);

  // Poll for a valid CD drive
  CHAR letter = TmFsGetCdDrive (filename);
  while (letter != -1) {
    if (TmErrorMessage (TM_ERROR_CD_INSERT_CD) != IDRETRY) {
      return 0;
    }
    letter = TmFsGetCdDrive (filename);
  }
  wsprintfA (cd_root, "%c:", letter);

  PHKEY key;
  DWORD temp;
  DWORD size = PATH_LEN;
  if (TmRegistryOpenKey (key, HKEY_LOCAL_MACHINE, NULL) == ERROR_SUCCESS) {
    // We found the registry key, so try to populate paths from the installation directory.
    // If registry keys are missing, fall back to the CD drive.
    if (TmRegistryQueryKeyRaw (key, "Install Directory", install_dir, &size) != ERROR_SUCCESS) {
      lstrcpyA (install_dir, cd_root);
    }

    if (TmRegistryQueryKeyDword (key, "Executable", &temp) == ERROR_SUCCESS) {
      lstrcpyA (path_executable, install_dir);
    } else {
      lstrcpyA (path_executable, cd_root);
    }

    if (TmRegistryQueryKeyDword (key, "Shell", &temp) == ERROR_SUCCESS) {
      lstrcpyA (path_shell, install_dir);
    } else {
      lstrcpyA (path_shell, cd_root);
    }

    if (TmRegistryQueryKeyDword (key, "Database", &temp) == ERROR_SUCCESS) {
      lstrcpyA (path_database, install_dir);
    } else {
      lstrcpyA (path_database, cd_root);
    }

    if (TmRegistryQueryKeyDword (key, "Save Game", &temp) == ERROR_SUCCESS) {
      lstrcpyA (path_saves, install_dir);
    } else {
      lstrcpyA (path_saves, cd_root);
    }

    if (TmRegistryQueryKeyDword (key, "Configuration", &temp) == ERROR_SUCCESS) {
      lstrcpyA (path_config, install_dir);
    } else {
      lstrcpyA (path_config, cd_root);
    }

    if (TmRegistryQueryKeyDword (key, "Sounds", &temp) == ERROR_SUCCESS) {
      lstrcpyA (path_sounds, install_dir);
    } else {
      lstrcpyA (path_sounds, cd_root);
    }

    if (TmRegistryQueryKeyRaw (key, "Movie Directory", movie_dir, &size) == ERROR_SUCCESS) {
      if (TmRegistryQueryKeyDword (key, "Movies", &temp) == ERROR_SUCCESS) {
        lstrcpyA (path_movies, movie_dir);
      } else {
        lstrcpyA (path_movies, install_dir);
      }
    } else {
      lstrcpyA (path_movies, '\0');
    }

    TmRegistryCloseKey (key);
  } else {
    lstrcpyA (path_executable, ".");
    lstrcpyA (path_shell, ".");
    lstrcpyA (path_database, ".");
    lstrcpyA (path_saves, ".");
    lstrcpyA (path_config, ".");
    lstrcpyA (path_sounds, ".");
    lstrcpyA (path_movies, '\0');
  }
  return 1;
}
