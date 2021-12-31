/**
 * @file texture.h
 *
 * Internal renderer texture functions and structures.
 */

typedef struct {
  // TODO: These are pointers to D3D types.
  DWORD unk1;
  DWORD unk2;
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD unk6;
} UnkTextureStruct;

/**
 * Release a texture's Direct3D resources.
 *
 * @param[in,out]  texture             Texture to release.
 */
VOID ReleaseUnkTextureStruct (UnkTextureStruct* texture);