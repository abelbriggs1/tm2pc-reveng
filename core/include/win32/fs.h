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

/**
 * Given a file name or relative path of a TM2 file, return an absolute path to
 * the file.
 *
 * @param[out]     abs_path            Pointer to store the absolute path of the file.
 * @param[in]      file_name           File name string. Can be a relative path from the
 *                                     root of the TM2 directory.
 * @param[in]      unk_val             TODO: What is this value?
 */
VOID TmFsGetFileAbsolutePath (LPSTR abs_path, LPCSTR file_name, int unk_val);

/**
 * Read the entirety of a file into an arbitrary buffer.
 *
 * @param[out]     buffer              Pointer to buffer to read the file into.
 * @param[in]      file_path           Absolute path of the file to read.
 *
 * @return         DWORD               Number of bytes read into the buffer.
 */
DWORD TmFsReadFile (LPVOID buffer, LPCSTR file_path);

#endif /* TM_FS_H */
