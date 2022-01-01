/**
 * @file clipper.c
 *
 * Internal graphics clipper functions.
 */

#include "graphics_internal.h"

#include "win32/error.h"

/**
 * Create the clipper object for the graphics context's primary surface. If the
 * clipper already exists, attach it to the primary surface.
 *
 * @address        0x004B4BB4
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID CreateClipper (TmGraphics* graphics)
{
  HRESULT result;
  if (graphics->unk_ddclipper) {
    if (graphics->primary_surface) {
      IDirectDrawSurface2_SetClipper (graphics->primary_surface, graphics->unk_ddclipper);
      return;
    }
  }

  if (graphics->ddraw2) {
    result = IDirectDraw2_CreateClipper (graphics->ddraw2, NULL, &graphics->unk_ddclipper, NULL);
    if (FAILED (result)) {
      TmErrorMessage (TM_ERROR_DRAW_CREATE_CLIPPER_FAIL, HRESULT_CODE (result));
      return;
    }
  }

  result = IDirectDrawClipper_SetHWnd (graphics->unk_ddclipper, NULL, graphics->hwnd);
  if (SUCCEEDED (result)) {
    if (graphics->primary_surface) {
      IDirectDrawSurface2_SetClipper (graphics->primary_surface, graphics->unk_ddclipper);
    }
  } else {
    TmErrorMessage (TM_ERROR_DRAW_ASSOC_CLIPPER_FAIL, HRESULT_CODE (result));
  }
}

/**
 * Release the clipper object for the primary surface, if it exists.
 *
 * @address        0x004B4C34
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID ReleaseClipper (TmGraphics* graphics)
{
  LPDIRECTDRAWCLIPPER clipper;

  if (graphics->primary_surface &&
      SUCCEEDED (IDirectDrawSurface2_GetClipper (graphics->primary_surface, &clipper))) {
    IDirectDrawSurface2_SetClipper (graphics->primary_surface, NULL);
  }

  if (graphics->unk_ddclipper) {
    IDirectDrawClipper_Release (graphics->unk_ddclipper);
    graphics->unk_ddclipper = NULL;
  }
}