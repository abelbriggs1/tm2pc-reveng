/**
 * @file surface.c
 *
 * Graphics context internal surface functions.
 */

#include "graphics_internal.h"

static DWORD surface_ref_count = 0;  // @address 0x00C85004

static HRESULT UnkLoadingFunc ()
{  // @address 0x00497390
  /**
   * TODO: This function appears to be some kind of loading
   * operation, since it displays strings like "loading textures". This function
   * is likely renderer code from a different module.
   */
  return DD_OK;
}

/**
 * Create a DirectDraw surface descended from the graphics context's
 * DirectDraw context.
 *
 * @address        0x004B4828
 *
 * @param[out]     surface             Variable which will hold the created surface.
 * @param[in]      width               Surface width. Optional, set to 0 if not using.
 * @param[in]      height              Surface height. Optional, set to 0 if not using.
 * @param[in]      caps                DirectDraw surface capabilities caps.
 * @param[in]      back_buffer_count   Number of back buffers for the surface.
 *
 * @return         HRESULT             Error code from DirectDraw surface creation.
 */
HRESULT GraphicsContext::CreateSurface (LPDIRECTDRAWSURFACE2 surface,
                                        DWORD width,
                                        DWORD height,
                                        DWORD caps,
                                        DWORD back_buffer_count)
{
  // TODO: DDSURFACEDESC or DDSURFACEDESC2? IDA struggles to decompile
  // this function.
  DDSURFACEDESC desc;
  memset (&desc, 0, sizeof (desc));

  desc.ddsCaps.dwCaps = caps;
  desc.dwSize = sizeof (desc);
  desc.dwBackBufferCount = back_buffer_count;
  if (width || height) {
    desc.dwWidth = width;
    desc.dwHeight = height;
    desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  } else {
    desc.dwFlags = DDSD_CAPS;
  }
  if (back_buffer_count) {
    desc.dwFlags |= DDSD_BACKBUFFERCOUNT;
  }

  HRESULT result = this->ddraw2->CreateSurface (&desc, (LPDIRECTDRAWSURFACE*)&surface, NULL);
  if (SUCCEEDED (result)) {
    surface_ref_count++;
  }
  return result;
}

/**
 * Release the graphics context's DirectDraw surfaces.
 *
 * @address        0x004B4374
 */
VOID GraphicsContext::ReleaseSurfaces ()
{
  // NB: result is undefined in this function if neither if statement
  // is hit.
  HRESULT result;

  ReleaseClipper ();

  if (this->membuffer_surface && this->membuffer_surface != this->backbuffer_surface) {
    result = this->membuffer_surface->Release ();
    surface_ref_count--;
  }
  if (this->primary_surface) {
    result = this->primary_surface->Release ();
    surface_ref_count--;
  }

  if (SUCCEEDED (result)) {
    this->primary_surface = NULL;
    this->membuffer_surface = NULL;
    this->backbuffer_surface = NULL;
  }
}

/**
 * Restore the graphics context's DirectDraw surfaces if they are lost.
 *
 * @address        0x004B4508
 *
 * @return         DD_OK               Surfaces were successfully restored.
 * @return         others              An error occurred while restoring DirectDraw surfaces.
 */
HRESULT GraphicsContext::RestoreSurfaces ()
{
  HRESULT result;

  if (this->primary_surface) {
    result = this->primary_surface->Restore ();
  }
  if (this->membuffer_surface && (this->membuffer_surface != this->backbuffer_surface)) {
    result = this->membuffer_surface->Restore ();
  }
  if (unk_graphics_init_flag) {
    result = UnkLoadingFunc ();
  }
  return result;
}
