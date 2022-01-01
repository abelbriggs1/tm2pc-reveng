/**
 * @file fs.h
 *
 * Filesystem functions.
 */

#ifndef TM_FS_H
#define TM_FS_H

/**
 * Initialize filesystem paths for game files.
 *
 * @param[in]      filename            Icon file name.
 *
 * @return         0                   Initialization failed.
 * @return         1                   Initialization was successful.
 */
int TmFsInit (const char* filename);

/**
 * Verify that the TM2 CD is inserted in a CD drive by looking for the given
 * file path.
 *
 * @address        0x00499884
 *
 * @param[in]      file_path           Relative path from the TM2 root for a file whose
 *                                     existence verifies the TM2 CD.
 * @return         char                Win32 CD drive letter if the TM2 CD is inserted, -1
 *                                     otherwise.
 */
char TmFsVerifyCdDrive (const char* file_path);

/**
 * Given a file name or relative path of a TM2 file, return an absolute path to
 * the file.
 *
 * @param[out]     abs_path            Pointer to store the absolute path of the file.
 * @param[in]      file_name           File name string. Can be a relative path from the
 *                                     root of the TM2 directory.
 * @param[in]      unk_val             TODO: What is this value?
 */
void TmFsGetFileAbsolutePath (char* abs_path, const char* file_name, int unk_val);

/**
 * Read the entirety of a file into an arbitrary buffer.
 *
 * @param[out]     buffer              Pointer to buffer to read the file into.
 * @param[in]      file_path           Absolute path of the file to read.
 *
 * @return         int                 Number of bytes read into the buffer.
 */
int TmFsReadFile (void* buffer, const char* file_path);

#endif /* TM_FS_H */
