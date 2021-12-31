/**
 * @file environment.h
 *
 * Win32 environment functions.
 */

#ifndef TM_ENVIRONMENT_H
#define TM_ENVIRONMENT_H

/**
 * Structure for Win32 application environment.
 */
typedef struct {
  LPSTR cmd_str;
  int num_args;
  LPSTR cmd_args[10];
  CHAR exec_path[400];
  CHAR pad[4];  // unknown if padding or variable
  DWORD screen_width;
  DWORD screen_height;
  DWORD bits_per_pixel;
} TmEnvironment;

/**
 * Initialize the Win32 environment with command line arguments and window
 * parameters.

 * @param[in,out]  env                 Environment structure.
 */
VOID TmEnvironmentInit (TmEnvironment* env);

/**
 * Retrieve the command line arguments.
 *
 * @param[in]      env                 Parameter description.
 *
 * @return         LPSTR*              Pointer table for the command line arguments.
 */
LPSTR* TmEnvironmentGetCmdArgs (TmEnvironment* env);

/**
 * Retrieve the number of command line arguments.
 *
 * @param[in]      env                 Parameter description.
 *
 * @return         int                 Number of command line arguments.
 */
int TmEnvironmentGetNumArgs (TmEnvironment* env);

#endif /* TM_ENVIRONMENT_H */