/**
 * @file texture.h
 *
 * Internal renderer texture functions and structures.
 */

typedef struct {
  // TODO: These are pointers to D3D types.
  int unk1;
  int unk2;
  int unk3;
  int unk4;
  int unk5;
  int unk6;
} UnkTextureStruct;

/**
 * Release a texture's Direct3D resources.
 *
 * @param[in,out]  texture             Texture to release.
 */
void ReleaseUnkTextureStruct (UnkTextureStruct* texture);