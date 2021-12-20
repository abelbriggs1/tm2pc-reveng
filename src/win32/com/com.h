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
void TmComLoadLibrary (HMODULE* module, LPCSTR name);

#endif /* TM_COM_H */
