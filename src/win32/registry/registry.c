/**
 * @file registry.c
 *
 * Wrapper library for Win32 registry operations.
 */

#include <windows.h>

#include "registry.h"

static char key_buffer[408];          // @ 0x00BDFB38
static BOOL key_buffer_empty = TRUE;  // @ 0x004E8528

/**
 * Initialize the registry wrapper library by setting the name string for
 * the main TM2 registry key.
 *
 * If key is NULL, on the first call, the string will be set to "Software\SingleTrac\default".
 * On subsequent calls with key == NULL, an error code will be returned.
 *
 * There are two versions of this function in the binary - the only difference appears
 * to be their return value. The first is only called by WinMain() and returns 1
 * if (!key && !key_buffer_empty), 0 otherwise. The other version is called in many places
 * across the binary and returns no value. For our purposes, we can simply document the
 * one used in WinMain.
 *
 * @address    0x004AC4B0
 *             0x004AC4E8
 *
 * @param[in]  key_name            Name to use for the registry key.
 * @return     0                   The library was initialized.
 * @return     1                   key_name was NULL, but the library had already been initialized
 *                                 before this call.
 */
int RegistryInit (LPCSTR key_name)
{
  if (key_name) {
    lstrcpyA (key_buffer, key_name);
  } else if (key_buffer_empty) {
    key_buffer_empty = FALSE;
    lstrcpyA (key_buffer, "Software\\SingleTrac\\default");
  } else {
    // We don't want to overwrite a custom key name with the default string,
    // so error out.
    return 1;
  }
  return 0;
}

/**
 * Create a registry key with the given parameters, or open the key if it
 * already exists.
 *
 * If key_name is NULL, the key path set previously by RegistryInit() will be used.
 *
 * @address        0x004AC550
 *
 * @param[in,out]  result              Pointer to a variable which will receive the handle
 *                                     for the created/opened key.
 * @param[in]      key                 Handle for an open registry key.
 * @param[in]      key_name            Name of the subkey to create.
 */
void RegistryCreateKey (PHKEY result, HKEY key, LPCSTR key_name)
{
  if (key_name) {
    RegCreateKeyExA (key, key_name, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, result, NULL);
  } else {
    RegCreateKeyExA (key, key_buffer, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, result, NULL);
  }
}

/**
 * Open a registry key.
 *
 * If key_name is NULL, the key path set previously by RegistryInit() will be used.
 *
 * @address        0x004AC528
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
LSTATUS RegistryOpenKey (PHKEY result, HKEY key, LPCSTR key_name)
{
  LSTATUS result;

  if (key_name) {
    result = RegOpenKeyExA (key, key_name, NULL, KEY_ALL_ACCESS, result);
  } else {
    result = RegOpenKeyExA (key, key_buffer, NULL, KEY_ALL_ACCESS, result);
  }
}

/**
 * Close a registry key.
 *
 * @address        0x004AC590
 *
 * @param[in]      key                 Pointer to a key handle.
 *
 * @return         ERROR_SUCCESS       The key was successfully closed.
 * @return         other               An error occurred closing the key. See the documentation
 *                                     for Win32 RegCloseKey() for more information.
 */
LSTATUS RegistryCloseKey (PHKEY key)
{
  return RegCloseKey (key);
}

/**
 * Retrieve data for the registry value associated with the given open registry key.
 *
 * @address        0x004AC5E4
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
LSTATUS RegistryQueryKeyRaw (PHKEY key, LPCSTR value_name, LPBYTE data, LPDWORD size)
{
  return RegQueryValueExA (*key, value_name, NULL, NULL, data, size);
}

/**
 * Retrieve a DWORD for a registry value associated with the given open registry key.
 *
 * @address        0x004AC5BC
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
LSTATUS RegistryQueryKeyDword (PHKEY key, LPCSTR value_name, LPDWORD data)
{
  DWORD dataSize = 4;
  return RegQueryValueExA (*key, value_name, NULL, NULL, data, &dataSize);
}

/**
 * Set raw byte data for a registry value associated with the given open registry key.
 *
 * @address        0x004AC65C
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
LSTATUS RegistrySetKeyRaw (PHKEY key, LPCSTR value_name, LPCBYTE data, DWORD size)
{
  return RegSetValueExA (*key, value_name, NULL, REG_BINARY, data, size);
}

/**
 * Set a DWORD for a registry value associated with the given open registry key.
 *
 * @address        0x004AC614
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
LSTATUS RegistrySetKeyDword (PHKEY key, LPCSTR value_name, DWORD data)
{
  return RegSetValueExA (*key, value_name, NULL, REG_DWORD, data, sizeof (DWORD));
}
