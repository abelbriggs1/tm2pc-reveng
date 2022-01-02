/**
 * @file win32_internal.h
 *
 * Internal header for Win32 implementation of engine which
 * exposes internal functions and variables.
 */

#ifndef TM_WIN32_INTERNAL_H
#define TM_WIN32_INTERNAL_H

#ifdef UNICODE
#undef UNICODE
#endif

// TM2 mostly uses D3D/DirectDraw2 structures, but was compiled
// with DX5.
#define DIRECT3D_VERSION 0x0500

#include <windows.h>

#endif /* TM_WIN32_INTERNAL_H */