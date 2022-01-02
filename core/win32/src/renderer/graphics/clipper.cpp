/**
 * @file clipper.c
 *
 * Internal graphics clipper functions.
 */

#include "graphics_internal.h"

/**
 * Create the clipper object for the graphics context's primary surface. If the
 * clipper already exists, attach it to the primary surface.
 *
 * @address        0x004B4BB4
 */
VOID GraphicsContext::CreateClipper ()
{
  HRESULT result;
  if (this->unk_ddclipper) {
    if (this->primary_surface) {
      this->primary_surface->SetClipper (this->unk_ddclipper);
      return;
    }
  }

  if (this->ddraw2) {
    result = this->ddraw2->CreateClipper (NULL, &this->unk_ddclipper, NULL);
    if (FAILED (result)) {
      TmErrorMessage (TM_ERROR_DRAW_CREATE_CLIPPER_FAIL, HRESULT_CODE (result));
      return;
    }
  }

  result = this->unk_ddclipper->SetHWnd (NULL, this->hwnd);
  if (SUCCEEDED (result)) {
    if (this->primary_surface) {
      this->primary_surface->SetClipper (this->unk_ddclipper);
    }
  } else {
    TmErrorMessage (TM_ERROR_DRAW_ASSOC_CLIPPER_FAIL, HRESULT_CODE (result));
  }
}

/**
 * Release the clipper object for the primary surface, if it exists.
 *
 * @address        0x004B4C34
 */
VOID GraphicsContext::ReleaseClipper ()
{
  LPDIRECTDRAWCLIPPER clipper;

  if (this->primary_surface && SUCCEEDED (this->primary_surface->GetClipper (&clipper))) {
    this->primary_surface->SetClipper (NULL);
  }

  if (this->unk_ddclipper) {
    this->unk_ddclipper->Release ();
    this->unk_ddclipper = NULL;
  }
}