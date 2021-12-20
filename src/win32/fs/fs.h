/**
 * @file fs.h
 *
 * Win32 filesystem functions.
 */

#ifndef TM_FS_H
#define TM_FS_H

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
CHAR TmFsVerifyCdDrive (LPCSTR file_path);

/**
 * Initialize filesystem paths for game files.
 *
 * @param[in]      filename            Icon file name.
 *
 * @return         0                   Initialization failed.
 * @return         1                   Initialization was successful.
 */
DWORD TmFsInit (LPCSTR filename);

#endif /* TM_FS_H */
