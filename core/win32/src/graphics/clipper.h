/**
 * @file clipper.h
 *
 * Internal graphics clipper functions and structures.
 */

#ifndef TM_CLIPPER_H
#define TM_CLIPPER_H

/**
 * Create the clipper object for the graphics context's primary surface. If the
 * clipper already exists, attach it to the primary surface.
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID CreateClipper (GraphicsContext* graphics);

/**
 * Release the clipper object for the primary surface, if it exists.
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID ReleaseClipper (GraphicsContext* graphics);

#endif /* TM_CLIPPER_H */