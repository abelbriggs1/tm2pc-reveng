/**
 * @file graphics.h
 *
 * Public API for graphics functions.
 */

#ifndef TM_GRAPHICS_H
#define TM_GRAPHICS_H

/**
 * Initialize the graphics module.
 *
 * @param[in]      unknown_graphics_flag     TODO: What is this flag?
 *
 * @return         0                         The graphics context was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
int TmGraphicsInit (int unknown_graphics_flag);

/**
 * Get the current display width.
 *
 * @return         int                 Display width, in pixels.
 */
int TmGraphicsGetDisplayWidth ();

/**
 * Get the current display height.
 *
 * @return         int                 Display height, in pixels.
 */
int TmGraphicsGetDisplayHeight ();

/**
 * Get the RGBA bit masks used for the draw surface.
 *
 * @param[out]     red_mask            Pointer to store red bit mask in. Optional.
 * @param[out]     green_mask          Pointer to store green bit mask in. Optional.
 * @param[out]     blue_mask           Pointer to store blue bit mask in. Optional.
 * @param[out]     alpha_mask          Pointer to store alpha bit mask in. Optional.
 */
void TmGraphicsGetColorDepthMasks (int* red_mask, int* green_mask, int* blue_mask, int* alpha_mask);

#endif /* TM_GRAPHICS_H */