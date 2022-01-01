/**
 * @file surface.h
 *
 * Graphics context internal surface functions and structures.
 */

/**
 * Create a DirectDraw surface descended from the graphics context's
 * DirectDraw context.
 *
 * @param[in,out]  graphics            Graphics context.
 * @param[out]     surface             Variable which will hold the created surface.
 * @param[in]      width               Surface width. Optional, set to 0 if not using.
 * @param[in]      height              Surface height. Optional, set to 0 if not using.
 * @param[in]      caps                DirectDraw surface capabilities caps.
 * @param[in]      back_buffer_count   Number of back buffers for the surface.
 *
 * @return         HRESULT             Error code from DirectDraw surface creation.
 */
HRESULT CreateSurface (TmGraphics* graphics,
                       LPDIRECTDRAWSURFACE2 surface,
                       DWORD width,
                       DWORD height,
                       DWORD caps,
                       DWORD back_buffer_count);

/**
 * Release the graphics context's DirectDraw surfaces.
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID ReleaseSurfaces (TmGraphics* graphics);

/**
 * Restore the graphics context's DirectDraw surfaces if they are lost.
 *
 * @param[in,out]  graphics            Graphics context.
 *
 * @return         DD_OK               Surfaces were successfully restored.
 * @return         others              An error occurred while restoring DirectDraw surfaces.
 */
HRESULT RestoreSurfaces (TmGraphics* graphics);