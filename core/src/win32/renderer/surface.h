/**
 * @file surface.h
 *
 * Renderer internal surface functions and structures.
 */

/**
 * Create a DirectDraw surface descended from the renderer's
 * DirectDraw context.
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
                       int back_buffer_count);

/**
 * Release the renderer's DirectDraw surfaces.
 *
 * @param[in,out]  renderer            Renderer context.
 */
void ReleaseSurfaces (TmRenderer* renderer);

/**
 * Restore the renderer's DirectDraw surfaces if they are lost.
 *
 * @param[in,out]  renderer            Renderer context.
 *
 * @return         DD_OK               Surfaces were successfully restored.
 * @return         others              An error occurred while restoring DirectDraw surfaces.
 */
HRESULT RestoreSurfaces (TmRenderer* renderer);