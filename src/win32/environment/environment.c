/**
 * @file environment.c
 *
 * Win32 environment functions.
 */

#include <windows.h>

#include "environment.h"

/**
 * Get the Win32 command line string for the application, split
 * the arguments by whitespace, and place the resulting arguments
 * in the environment structure.
 *
 * @address        0x004C465C
 *
 * @param[in,out]  env                 Environment structure.
 */
static void SplitCmdArgs (TmEnvironment* env)
{
  LPSTR cmd_str = GetCommandLineA ();
  BOOL quote = FALSE;

  env->num_args = 0;
  env->cmd_str = cmd_str;

  // Loop through the command line string, splitting and null-terminating
  // arguments in-place.
  while (*cmd_str) {
    if (env->num_args >= 10) {
      break;
    }

    env->cmd_args[env->num_args] = cmd_str;
    while (*cmd_str && (*cmd_str != ' ' || quote)) {
      if (*cmd_str == '"') {
        // Ensure we only erase surrounding/extraneous quotes.
        quote = !quote;
      }
      cmd_str++;
    }

    if (*cmd_str == ' ') {
      *cmd_str = '\0';
      // Skip any remaining whitespace to move to the next argument.
      while (*cmd_str == ' ') {
        cmd_str++;
      }
    }

    env->num_args++;
  }
}

/**
 * Initialize the Win32 environment with command line arguments and window
 * parameters.
 *
 * @address        0x004C45C0
 *
 * @param[in,out]  env                 Environment structure.
 */
void TmEnvironmentInit (TmEnvironment* env)
{
  SplitCmdArgs (env);

  // env->cmd_args[0] will always be the path to the executable
  LPSTR path = env->cmd_args[0];
  int len = lstrlenA (path);
  // If " is the first character, cut it off
  if (*path == '"') {
    len--;
    path++;
  }
  // Cut off the end of the string with the executable name - gives us path to executable
  for (LPSTR i = path[len]; (len > 0 && *i != '\\'); i--) {
    len--;
  }
  qmemcpy (env->exec_path, path, len);

  env->screen_width = GetSystemMetrics (SM_CXSCREEN);
  env->screen_height = GetSystemMetrics (SM_CYSCREEN);
  HDC context = GetDC (NULL);
  env->bits_per_pixel = GetDeviceCaps (context, BITSPIXEL);
  ReleaseDC (NULL, context);
}

/**
 * Retrieve the command line arguments.
 *
 * @address        0x004C471C
 *
 * @param[in]      env                 Parameter description.
 *
 * @return         LPSTR*              Pointer table for the command line arguments.
 */
LPSTR* TmEnvironmentGetCmdArgs (TmEnvironment* env)
{
  return env->cmd_args;
}

/**
 * Retrieve the number of command line arguments.
 *
 * @address        0x004C4718
 *
 * @param[in]      env                 Parameter description.
 *
 * @return         int                 Number of command line arguments.
 */
int TmEnvironmentGetNumArgs (TmEnvironment* env)
{
  return env->num_args;
}
