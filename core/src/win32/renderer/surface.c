/**
 * @file surface.c
 *
 * Renderer internal surface functions.
 */

#include "renderer_internal.h"

static int surface_ref_count = 0;  // @address 0x00C85004

static HRESULT UnkLoadingFunc ()
{  // @address 0x00497390
  /**
   * TODO TODO TODO: This function appears to be some kind of loading
   * operation, since it displays strings like "loading textures".
   *
   * Separation of concerns utterly falls apart here. This function is certainly
   * application code, since it calls several other functions which use
   * main's TmRenderer instance.
   *
   * However, this function gets called by RestoreSurfaces() in the renderer module,
   * and it also calls ReleaseUnkTextureStructs() to release textures... so textures
   * (and the textures array defined in renderer.c) must be application code as well.
   * Furthermore, ReleaseHalDevice() calls ReleaseUnkTextureStructs() too, and that's an
   * internal renderer function.
   *
   * It's possible that the *entire* renderer is application code.
   *
   * This will need to be revisited once we make progress on application code
   * and potentially find other similar functions. We might need to reorganize
   * the codebase again.
   *
   * My preliminary thought is that the current "core/renderer" is more like "core/graphics"
   * and that the real renderer (which is higher level and performs texture/text operations)
   * is in application code.
   *
   */
  return DD_OK;
}

/**
 * Create a DirectDraw surface descended from the renderer's
 * DirectDraw context.
 *
 * @address        0x004B4828
 *
 * @param[in,out]  renderer            Renderer context.
 * @param[out]     surface             Variable which will hold the created surface.
 * @param[in]      width               Surface width. Optional, set to 0 if not using.
 * @param[in]      height              Surface height. Optional, set to 0 if not using.
 * @param[in]      caps                DirectDraw surface capabilities caps.
 * @param[in]      back_buffer_count   Number of back buffers for the surface.
 *
 * @return         HRESULT             Error code from DirectDraw surface creation.
 */
HRESULT CreateSurface (TmRenderer* renderer,
                       LPDIRECTDRAWSURFACE2 surface,
                       int width,
                       int height,
                       int caps,
                       int back_buffer_count)
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

  HRESULT result = IDirectDraw2_CreateSurface (renderer->ddraw2, &desc, surface, NULL);
  if (SUCCEEDED (result)) {
    surface_ref_count++;
  }
  return result;
}

/**
 * Release the renderer's DirectDraw surfaces.
 *
 * @address        0x004B4374
 *
 * @param[in,out]  renderer            Renderer context.
 */
void ReleaseSurfaces (TmRenderer* renderer)
{
  // NB: result is undefined in this function if neither if statement
  // is hit.
  HRESULT result;

  ReleaseClipper (renderer);

  if (renderer->membuffer_surface && renderer->membuffer_surface != renderer->backbuffer_surface) {
    result = IDirectDrawSurface2_Release (renderer->membuffer_surface);
    surface_ref_count--;
  }
  if (renderer->primary_surface) {
    result = IDirectDrawSurface2_Release (renderer->primary_surface);
    surface_ref_count--;
  }

  if (SUCCEEDED (result)) {
    renderer->primary_surface = NULL;
    renderer->membuffer_surface = NULL;
    renderer->backbuffer_surface = NULL;
  }
}

/**
 * Restore the renderer's DirectDraw surfaces if they are lost.
 *
 * @address        0x004B4508
 *
 * @param[in,out]  renderer            Renderer context.
 *
 * @return         DD_OK               Surfaces were successfully restored.
 * @return         others              An error occurred while restoring DirectDraw surfaces.
 */
HRESULT RestoreSurfaces (TmRenderer* renderer)
{
  HRESULT result;

  if (renderer->primary_surface) {
    result = IDirectDrawSurface2_Restore (renderer->primary_surface);
  }
  if (renderer->membuffer_surface &&
      (renderer->membuffer_surface != renderer->backbuffer_surface)) {
    result = IDirectDrawSurface2_Restore (renderer->membuffer_surface);
  }
  if (unk_renderer_init_flag) {
    result = UnkLoadingFunc ();
  }
  return result;
}

/**
 * Fill a DirectDraw surface with the specified color.
 *
 * @address        0x004B4DA3
 *
 * @param[in,out]  renderer            Renderer context.
 * @param[in]      color               16-bit RGB color to use.
 * @param[in,out]  surface             Surface to fill. If NULL, the renderer's memory
 *                                     buffer surface will be used if it exists.
 *
 * @return         DD_OK               The fill was successful.
 * @return         E_FAIL              surface was NULL, and the renderer's memory buffer
 *                                     was uninitialized.
 * @return         others              Error code from DirectDraw fill operation.
 */
HRESULT FillSurface (TmRenderer* renderer, int color, LPDIRECTDRAWSURFACE2 surface)
{
  if (!surface) {
    surface = renderer->membuffer_surface;
    if (!surface) {
      return E_FAIL;
    }
  }

  DDBLTFX fx;
  memset (&fx, 0, sizeof (fx));
  fx.dwFillColor = color;
  fx.dwSize = sizeof (fx);
  return IDirectDrawSurface2_Blt (surface, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &fx);
}
