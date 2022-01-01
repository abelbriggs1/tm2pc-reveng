/**
 * @file window_internal.h
 *
 * Internal window header which exposes module-level variables and includes
 * for other window module files. This header should be included by all .c
 * files in the window module. Any new module-level header file should be
 * added to the includes in this file.
 */

#ifndef TM_WINDOW_INTERNAL_H
#define TM_WINDOW_INTERNAL_H

#include <windows.h>

#include "tm2.h"

#include "internal/win32_internal.h"

#include "window.h"

#include "window_impl.h"

#endif /* TM_WINDOW_INTERNAL_H */