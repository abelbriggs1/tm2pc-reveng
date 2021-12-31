/**
 * @file clipper.c
 *
 * Internal renderer clipper functions.
 */

#include "renderer_internal.h"

#include "win32/error.h"

/**
 * Create the clipper object for the renderer's primary surface. If the
 * clipper already exists, attach it to the primary surface.
 *
 * @address        0x004B4BB4
 *
 * @param[in,out]  renderer            Renderer context.
 */
VOID CreateClipper (TmRenderer* renderer)
{
  HRESULT result;
  if (renderer->unk_ddclipper) {
    if (renderer->primary_surface) {
      IDirectDrawSurface2_SetClipper (renderer->primary_surface, renderer->unk_ddclipper);
      return;
    }
  }

  if (renderer->ddraw2) {
    result = IDirectDraw2_CreateClipper (renderer->ddraw2, NULL, &renderer->unk_ddclipper, NULL);
    if (FAILED (result)) {
      TmErrorMessage (TM_ERROR_DRAW_CREATE_CLIPPER_FAIL, HRESULT_CODE (result));
      return;
    }
  }

  result = IDirectDrawClipper_SetHWnd (renderer->unk_ddclipper, NULL, renderer->hwnd);
  if (SUCCEEDED (result)) {
    if (renderer->primary_surface) {
      IDirectDrawSurface2_SetClipper (renderer->primary_surface, renderer->unk_ddclipper);
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
 * @param[in,out]  renderer            Renderer context.
 */
VOID ReleaseClipper (TmRenderer* renderer)
{
  LPDIRECTDRAWCLIPPER clipper;

  if (renderer->primary_surface &&
      SUCCEEDED (IDirectDrawSurface2_GetClipper (renderer->primary_surface, &clipper))) {
    IDirectDrawSurface2_SetClipper (renderer->primary_surface, NULL);
  }

  if (renderer->unk_ddclipper) {
    IDirectDrawClipper_Release (renderer->unk_ddclipper);
    renderer->unk_ddclipper = NULL;
  }
}