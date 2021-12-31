/**
 * @file com.h
 *
 * Windows COM functions.
 */

#ifndef TM_COM_H
#define TM_COM_H

/**
 * Load a module into the address space of the process and return
 * the handle of the module.
 *
 * @param[in,out]  module              Pointer to variable which will hold the loaded module
 *                                     handle. If the handle is already initialized when calling
 *                                     this function, it will be freed before attempting to load
 *                                     the given module name.
 * @param[in]      name                Name of the module to load.
 */
VOID TmComLoadLibrary (HMODULE* module, LPCSTR name);

/**
 * Free a module which was allocated into the address space of the
 * process.
 *
 * @param[in,out]  module              Pointer to a variable which holds the loaded module
 *                                     handle to be freed.
 *
 * @return         nonzero             The module was successfully freed.
 * @return         0                   *module was NULL, or the call to FreeLibrary() failed.
 */
BOOL TmComFreeLibrary (HMODULE* module)

#endif /* TM_COM_H */
