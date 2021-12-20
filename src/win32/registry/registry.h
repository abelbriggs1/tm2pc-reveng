/**
 * @file registry.h
 *
 * Wrapper library for Win32 registry operations.
 */

#ifndef TM_REGISTRY_H
#define TM_REGISTRY_H

/**
 * Initialize the registry wrapper library by setting the name string for
 * the main TM2 registry key.
 *
 * If key is NULL, on the first call, the string will be set to "Software\SingleTrac\default".
 * On subsequent calls with key == NULL, an error code will be returned.
 *
 * @param[in]  key_name            Name to use for the registry key.
 * @return     0                   The library was initialized.
 * @return     1                   key_name was NULL, but the library had already been initialized
 *                                 before this call.
 */
DWORD TmRegistryInit (LPCSTR key_name);

/**
 * Create a registry key with the given parameters, or open the key if it
 * already exists.
 *
 * If key_name is NULL, the key path set previously by TmRegistryInit() will be used.
 *
 * @param[in,out]  result              Pointer to a variable which will receive the handle
 *                                     for the created/opened key.
 * @param[in]      key                 Handle for an open registry key.
 * @param[in]      key_name            Name of the subkey to create.
 */
void TmRegistryCreateKey (PHKEY result, HKEY key, LPCSTR key_name);

/**
 * Open a registry key.
 *
 * If key_name is NULL, the key path set previously by TmRegistryInit() will be used.
 *
 * @param[in,out]  result              Pointer to a variable which will receive the handle
 *                                     for the opened key.
 * @param[in]      key                 Handle for an open registry key.
 * @param[in]      key_name            Name of the subkey to open.
 *
 * @return         ERROR_SUCCESS       The key was successfully opened.
 * @return         other               An error occurred opening the key. See the documentation
 *                                     for Win32 RegOpenKeyExA() for more information.
 */
LSTATUS TmRegistryOpenKey (PHKEY result, HKEY key, LPCSTR key_name);

/**
 * Close a registry key.
 *
 * @param[in]      key                 Pointer to a key handle.
 *
 * @return         ERROR_SUCCESS       The key was successfully closed.
 * @return         other               An error occurred closing the key. See the documentation
 *                                     for Win32 RegCloseKey() for more information.
 */
LSTATUS TmRegistryCloseKey (PHKEY key);

/**
 * Retrieve data for the registry value associated with the given open registry key.
 *
 * @param[in]      key                   Pointer to an open key handle.
 * @param[in]      value_name            Registry value associated with the open key handle
 *                                       to query.
 * @param[in,out]  data                  Pointer to a buffer which will receive the data
 *                                       queried from the registry value.
 * @param[in,out]  size                  Pointer to a variable which contains the size, in bytes,
 *                                       of the given data buffer. If this function returns
 *                                       ERROR_MORE_DATA, this variable will be modified with the
 *                                       buffer size required to hold the queried data.
 *
 * @return         ERROR_SUCCESS         The query succeeded.
 * @return         ERROR_MORE_DATA       The given buffer is not large enough to hold the queried
 *                                       data.
 * @return         ERROR_FILE_NOT_FOUND  The given registry value does not exist.
 * @return         other                 An error occurred querying the key. See the documentation
 *                                       for Win32 RegQueryValueExA() for more information.
 */
LSTATUS TmRegistryQueryKeyRaw (PHKEY key, LPCSTR value_name, LPBYTE data, LPDWORD size);

/**
 * Retrieve a DWORD for a registry value associated with the given open registry key.
 *
 * @param[in]      key                   Pointer to an open key handle.
 * @param[in]      value_name            Registry value associated with the open key handle
 *                                       to query.
 * @param[in,out]  data                  Pointer to a variable which will recieve the DWORD value
 *                                       queried from the registry value.
 *
 * @return         ERROR_SUCCESS         The query succeeded.
 * @return         ERROR_FILE_NOT_FOUND  The given registry value does not exist.
 * @return         other                 An error occurred querying the key. See the documentation
 *                                       for Win32 RegQueryValueEx() for more information.
 */
LSTATUS TmRegistryQueryKeyDword (PHKEY key, LPCSTR value_name, LPDWORD data);

/**
 * Set raw byte data for a registry value associated with the given open registry key.
 *
 * @param[in]      key                   Pointer to an open key handle.
 * @param[in]      value_name            Registry value associated with the open key handle
 *                                       to set.
 * @param[in]      data                  Pointer to a buffer which contains the new data for the
 *                                       registry value.
 * @param[in]      size                  Size, in bytes, of the given data buffer.
 *
 * @return         ERROR_SUCCESS         The operation succeeded.
 * @return         other                 An error occurred setting the key. See the documentation
 *                                       for Win32 RegSetValueExA() for more information.
 */
LSTATUS TmRegistrySetKeyRaw (PHKEY key, LPCSTR value_name, LPCBYTE data, DWORD size);

/**
 * Set a DWORD for a registry value associated with the given open registry key.
 *
 * @param[in]      key                   Pointer to an open key handle.
 * @param[in]      value_name            Registry value associated with the open key handle
 *                                       to set.
 * @param[in]      data                  The DWORD value to set.
 *
 * @return         ERROR_SUCCESS         The operation succeeded.
 * @return         other                 An error occurred setting the key. See the documentation
 *                                       for Win32 RegSetValueExA() for more information.
 */
LSTATUS TmRegistrySetKeyDword (PHKEY key, LPCSTR value_name, DWORD data);

#endif /* TM_REGISTRY_H */