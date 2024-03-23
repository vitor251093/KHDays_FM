#ifndef TEXREPLACE_H
#define TEXREPLACE_H

#include "types.h"

namespace melonDS
{

class TextureUtils {
public:
    static unsigned char* LoadTextureFromFile(const char* path, int* width, int* height, int* channels);
    static void ExportTextureAsFile(unsigned char* data, const char* path, u32 width, u32 height, u32 channels);
};

}

#endif