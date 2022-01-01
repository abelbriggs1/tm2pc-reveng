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
  return GraphicsInit (&graphics, unknown_graphics_flag);
}

/**
 * Get the current display width.
 *
 * @address        N/A
 *
 * @return         int                 Display width, in pixels.
 */
int TmGraphicsGetDisplayWidth ()
{
  return GraphicsGetDisplayWidth (&graphics);
}

/**
 * Get the current display height.
 *
 * @address        N/A
 *
 * @return         int                 Display height, in pixels.
 */
int TmGraphicsGetDisplayHeight ()
{
  return GraphicsGetDisplayHeight (&graphics);
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
  return GraphicsGetColorDepthMasks (&graphics, red_mask, green_mask, blue_mask, alpha_mask);
}
