/**
 * @file graphics.c
 *
 * Implementation for public graphics API.
 */

#include "graphics_internal.h"

static GraphicsContext graphics;  // @address 0x00C84840

/**
 * Initialize the graphics module.
 *
 * @address        N/A
 *
 * @param[in]      unknown_graphics_flag     TODO: What is this flag?
 *
 * @return         0                         The graphics context was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
int TmGraphicsInit (int unknown_graphics_flag)
{
  return graphics.Init (unknown_graphics_flag);
}

/**
 * Swap the primary and back framebuffers.
 *
 * @address        0x00495B14
 *
 * @return         0                   The framebuffer was swapped successfully.
 * @return         others              An error occurred swapping the framebuffers.
 */
int TmGraphicsSwapFramebuffer ()
{
  return graphics.SwapBuffers ();
}

/**
 * Fill the primary framebuffer with the specified color.
 *
 * @address        0x00495B48
 *
 * @param[in]      color               16-bit color to fill the framebuffer.
 *
 * @return         0                   The framebuffer was successfully filled.
 * @return         others              An error occurred filling the framebuffer.
 */
int TmGraphicsFillFramebuffer (uint16_t color)
{
  return graphics.FillBuffer (color, NULL);
}

/**
 * Fill all framebuffers with the specified color.
 *
 * @address        0x00495C94
 *
 * @param[in]      color               16-bit color to fill the framebuffers.
 */
void TmGraphicsFillFramebuffers (uint16_t color)
{
  for (int i = 0; i < MAX_BUFFERS; i++) {
    TmGraphicsFillFramebuffer (color);
    TmGraphicsSwapFramebuffer ();
  }
}

/**
 * Get the current display width.
 *
 * @address        0x004948D8
 *
 * @return         int                 Display width, in pixels.
 */
int TmGraphicsGetDisplayWidth ()
{
  return graphics.GetDisplayWidth ();
}

/**
 * Get the current display height.
 *
 * @address        0x004958E4
 *
 * @return         int                 Display height, in pixels.
 */
int TmGraphicsGetDisplayHeight ()
{
  return graphics.GetDisplayHeight ();
}

/**
 * Get the RGBA bit masks used for the draw surface.
 *
 * @address        N/A
 *
 * @param[out]     red_mask            Pointer to store red bit mask in. Optional.
 * @param[out]     green_mask          Pointer to store green bit mask in. Optional.
 * @param[out]     blue_mask           Pointer to store blue bit mask in. Optional.
 * @param[out]     alpha_mask          Pointer to store alpha bit mask in. Optional.
 */
void TmGraphicsGetColorDepthMasks (int* red_mask, int* green_mask, int* blue_mask, int* alpha_mask)
{
  return graphics.GetColorDepthMasks ((DWORD*)red_mask, (DWORD*)green_mask, (DWORD*)blue_mask,
                                      (DWORD*)alpha_mask);
}

int TmGraphicsSetDisplayMode (int width, int height, int bits_per_pixel, int monitor_freq)
{
  return graphics.SetDisplayMode (width, height, bits_per_pixel, monitor_freq);
}
