/**
 * @file com.c
 *
 * Windows COM functions.
 */

#include <windows.h>

#include "com.h"

/**
 * Load a module into the address space of the process and return
 * the handle of the module.
 *
 * @address        0x004C4558
 *
 * @param[in,out]  module              Pointer to variable which will hold the loaded module
 *                                     handle. If the handle is already initialized when calling
 *                                     this function, it will be freed before attempting to load
 *                                     the given module name.
 * @param[in]      name                Name of the module to load.
 */
void TmComLoadLibrary (HMODULE* module, LPCSTR name)
{
  if (*module) {
    FreeLibrary (*module);
    *module = NULL;
  }
  *module = LoadLibraryA (name);
}
